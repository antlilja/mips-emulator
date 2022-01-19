#pragma once
#include "mips-emulator/instruction.hpp"
#include "mips-emulator/executor.hpp"

#include <utility>

namespace mips_emulator {
    template <typename RegisterState, typename Memory>
    class Emulator {
    public:
        using Register = typename RegisterState::Register;
        using Address = typename RegisterState::Unsigned;

        using Executor = Executor<RegisterState>;

        template <typename... Args>
        Emulator(Args&&... args) : memory(std::forward<Args>(args)...) {}

        RegisterState get_register_state() const noexcept { return regs; }

        void step() noexcept {
            const Instruction instr = memory.template read<Instruction>(pc);

            switch (instr.get_type()) {
                    // R-Type
                case 0: {
                    Executor::handle_rtype_instr(instr, pc, regs);
                    break;
                }
                    // I-Type
                case 2: {
                    // TODO: Handle I-Type instructions
                    break;
                }
                    // J-Type
                default: {
                    // TODO: Handle J-Type instructions
                    break;
                }
            }

            pc += sizeof(Instruction);
        }

    private:
        RegisterState regs;
        Memory memory;

        Address pc;
    };
} // namespace mips_emulator
