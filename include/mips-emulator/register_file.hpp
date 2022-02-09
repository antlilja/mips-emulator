#pragma once
#include "mips-emulator/register_name.hpp"

#include <cstdint>

namespace mips_emulator {
    class RegisterFile {
    public:
        using Unsigned = uint32_t;
        using Signed = int32_t;

        union Register {
            Unsigned u;
            Signed s;
        };

        // Make sure we only have one word size
        static_assert(sizeof(Unsigned) == sizeof(Signed),
                      "Unsigned and Signed types are not the same size");

        // Make sure union has the same size as the word size
        static_assert(
            sizeof(Register) == sizeof(Unsigned),
            "Register union is not the same size as Unsigned or Signed types");

        static constexpr uint8_t REGISTER_COUNT = 32;
        static constexpr uint8_t INDEX_MASK = REGISTER_COUNT - 1;

        Unsigned get_pc() const noexcept { return pc; }
        void set_pc(Unsigned new_pc) noexcept { pc = new_pc; }
        void inc_pc() noexcept { pc += 4; }

        Register get(const RegisterName reg) const noexcept {
            return get(static_cast<uint8_t>(reg));
        }

        Register get(const uint8_t index) const noexcept {
            // Mask index in order to not access any out of bounds memory
            return regs[index & INDEX_MASK];
        }

        void set_unsigned(const RegisterName reg,
                          const Unsigned value) noexcept {
            set_unsigned(static_cast<uint8_t>(reg), value);
        }

        void set_unsigned(const uint8_t index, const Unsigned value) noexcept {
            // Mask index in order to not access any out of bounds memory
            regs[index & INDEX_MASK].u = value;

            // Register $0 should always be set to zero
            regs[0].u = 0;
        }

        void set_signed(const RegisterName reg, const Signed value) noexcept {
            set_signed(static_cast<uint8_t>(reg), value);
        }

        void set_signed(const uint8_t index, const Signed value) noexcept {
            // Mask index in order to not access any out of bounds memory
            regs[index & INDEX_MASK].s = value;

            // Register $0 should always be set to zero
            regs[0].u = 0;
        }

        void zero_all() noexcept {
            for (int i = 0; i < REGISTER_COUNT; ++i)
                regs[i].u = 0;
        }

    private:
        Unsigned pc = 0;
        Register regs[REGISTER_COUNT] = {};
    };
} // namespace mips_emulator
