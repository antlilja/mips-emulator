#pragma once
#include <cstdint>
#include <cassert>

namespace mips_emulator {
    template <typename TUnsigned, typename TSigned>
    class RegisterFile {
    public:
        using Unsigned = TUnsigned;
        using Signed = TSigned;

        union Register {
            Unsigned u;
            Signed s;
        };

        // Make sure we only have one word size
        static_assert(sizeof(Unsigned) == sizeof(Signed));

        // Make sure union has the same size as the word size
        static_assert(sizeof(Register) == sizeof(Unsigned));

        static constexpr std::size_t REGISTER_COUNT = 32;

        Register get_register(uint8_t index) const noexcept {
            // TODO: Maybe use something other than asserts here,
            // return some error code instead?
            assert(index < REGISTER_COUNT);
            return regs[index];
        }

        void set_register_unsigned(uint8_t index, Unsigned value) noexcept {
            // TODO: Maybe use something other than asserts here,
            // return some error code instead?
            assert(index < REGISTER_COUNT);
            regs[index].u = value;

            // Register $0 should always be set to zero
            regs[0].u = 0;
        }

        void set_register_signed(uint8_t index, Signed value) noexcept {
            // TODO: Maybe use something other than asserts here,
            // return some error code instead?
            assert(index < REGISTER_COUNT);
            regs[index].s = value;

            // Register $0 should always be set to zero
            regs[0].u = 0;
        }

        void zero_all_registers() noexcept {
            for (int i = 0; i < REGISTER_COUNT; ++i)
                regs[i].u = 0;
        }

    private:
        Register regs[REGISTER_COUNT] = {};
    };

    using RegisterFile32 = RegisterFile<uint32_t, int32_t>;
    using RegisterFile64 = RegisterFile<uint64_t, int64_t>;
} // namespace mips_emulator
