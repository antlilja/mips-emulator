#pragma once
#include "mips-emulator/result.hpp"
#include "mips-emulator/span.hpp"

#include <cstdint>
#include <memory>

namespace mips_emulator {
    enum class MemoryError : uint8_t {
        unaligned_access,
        out_of_bounds_access,
    };

    struct NullMMIO {};

    template <typename MemoryImplemantion, typename MMIOHandler = NullMMIO>
    class Memory {
    public:
        using Address = uint32_t;

        Memory(std::shared_ptr<MMIOHandler> mmio) : mmio(std::move(mmio)) {}

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
            if constexpr (!std::is_same_v<MMIOHandler, NullMMIO>) {
                const auto mmio_value = mmio->template read<T>(address);
                if (mmio_value.has_value()) return mmio_value.value();
            }

            // NOTE: Bounds check after MMIO, MMIO could have a bigger range
            if (!is_in_bounds<T>(address)) {
                return MemoryError::out_of_bounds_access;
            }

            return *reinterpret_cast<T*>(
                static_cast<MemoryImplemantion*>(this)->get_memory() + address);
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
            if constexpr (!std::is_same_v<MMIOHandler, NullMMIO>) {
                if (mmio->template store<T>(address, value)) return {};
            }

            // NOTE: Bounds check after MMIO, MMIO could have a bigger range
            if (!is_in_bounds<T>(address)) {
                return MemoryError::out_of_bounds_access;
            }

            *reinterpret_cast<T*>(
                static_cast<MemoryImplemantion*>(this)->get_memory() +
                address) = value;

            return {};
        }

        Span<uint8_t> get_memory() {
            return {
                static_cast<MemoryImplemantion*>(this)->get_memory(),
                static_cast<MemoryImplemantion*>(this)->get_size(),
            };
        }

    protected:
        template <typename T>
        inline static bool is_aligned(const Address address) {
            return (address & (sizeof(T) - 1)) == 0;
        }

        template <typename T>
        inline bool is_in_bounds(const Address address) {
            return (address + sizeof(T)) <
                   static_cast<MemoryImplemantion*>(this)->get_size();
        }

    protected:
        std::shared_ptr<MMIOHandler> mmio;
    };
} // namespace mips_emulator
