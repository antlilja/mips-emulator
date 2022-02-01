#pragma once
#include "mips-emulator/span.hpp"
#include "mips-emulator/result.hpp"
#include "mips-emulator/memory_error.hpp"

#include <memory>
#include <memory_resource>
#include <type_traits>

namespace mips_emulator {
    template <typename Address>
    struct NullMMIO {};

    template <typename UnsignedWord,
              template <typename> typename MMIOHandler = NullMMIO>
    class StaticMemory {
    public:
        using Address = UnsignedWord;
        using Size = UnsignedWord;

        using MMIO = MMIOHandler<Address>;

        StaticMemory(const Size size, MMIO&& mmio,
                     std::shared_ptr<std::pmr::memory_resource>
                         memory_resource = nullptr)
            : memory(size, 0, memory_resource), mmio(mmio) {}

        // NOTE:
        // This method is deliberately left as non-const because the
        // MMIOHandler read might not be const, and alter some internal state.
        template <typename T>
        Result<T, MemoryError> read(const Address address) {
            static_assert(sizeof(T) <= sizeof(Address),
                          "Can't read larger than word size");

            // NOTE: Types of size 1 are always aligned
            if constexpr (sizeof(T) > 1) {
                if (!is_aligned<T>(address)) {
                    return MemoryError::unaligned_access;
                }
            }

            // Try to read from MMIO handler
            if constexpr (std::is_member_function_pointer_v<
                              decltype(&MMIO::template read<T>)>) {
                const auto mmio_value = mmio.template read<T>(address);
                if (mmio_value.has_value()) return mmio_value.value();
            }

            // NOTE: Bounds check after MMIO, MMIO could have a bigger range
            if (!is_in_bounds<T>(address)) {
                return MemoryError::out_of_bounds_access;
            }

            return *reinterpret_cast<T*>(&memory[address]);
        }

        template <typename T>
        Result<void, MemoryError> store(const Address address, const T value) {
            static_assert(sizeof(T) <= sizeof(Address),
                          "Can't read larger than word size");

            // NOTE: Types of size 1 are always aligned
            if constexpr (sizeof(T) > 1) {
                if (!is_aligned<T>(address)) {
                    return MemoryError::unaligned_access;
                }
            }

            // Try to write to MMIO handler
            if constexpr (std::is_member_function_pointer_v<
                              decltype(&MMIO::template store<T>)>) {
                if (mmio.template store<T>(address, value)) return {};
            }

            // NOTE: Bounds check after MMIO, MMIO could have a bigger range
            if (!is_in_bounds<T>(address)) {
                return MemoryError::out_of_bounds_access;
            }

            *reinterpret_cast<T*>(&memory[address]) = value;

            return {};
        }

        Span<uint8_t> get_memory() {
            return {
                memory.data(),
                memory.size(),
            };
        }

    private:
        template <typename T>
        inline static bool is_aligned(const Address address) {
            return (address & sizeof(T)) == 0;
        }

        template <typename T>
        inline bool is_in_bounds(const Address address) {
            return (address + sizeof(T)) < memory.size();
        }

    private:
        MMIOHandler<Address> mmio;
        std::pmr::vector<uint8_t> memory;
    };
} // namespace mips_emulator
