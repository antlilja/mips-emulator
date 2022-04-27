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

        // http://www.it.uu.se/education/course/homepage/os/vt18/module-1/mips-coprocessor-0/#cause-register-13
        enum class Exception {
            e_int = 0,
            e_ad_el = 4,
            e_ad_es = 5,
            // e_ibe = 6,
            // e_dbe = 7,
            e_sys = 8,
            e_bp = 9,
            e_ri = 10,
            e_cpu = 11,
            e_ov = 12,
            e_tr = 13,
            e_fpe = 14,
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

        void delayed_branch(Unsigned target) noexcept {
            branch_flag = true;
            branch_target = target;
        }

        void update_pc() noexcept {
            inc_pc();
            pc += branch_flag * (branch_target - pc);
            branch_flag = false;
        }

        // bad_instr is set on signalexceptions and will contain info on the
        // exception raised.
        void signal_exception(const Exception cause,
                              const uint32_t instr) noexcept {
            bad_instr = instr;
            cause_register = cause;
        }

        uint32_t get_bad_instr() const noexcept { return bad_instr; }
        uint8_t get_cause_register() const noexcept {
            return static_cast<uint8_t>(cause_register);
        }

    private:
        bool branch_flag = false;
        Unsigned branch_target = 0;

        // bad_instr (BadInstr in docs) contains instruction that signaled an
        // exception/trap)
        Unsigned bad_instr = 0;

        // cause register contains the cause of a signaled exception
        Exception cause_register;

        Unsigned pc = 0;
        Register regs[REGISTER_COUNT] = {};
    };
} // namespace mips_emulator
