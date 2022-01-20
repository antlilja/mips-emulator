#pragma once
#include "mips-emulator/instruction.hpp"
#include "mips-emulator/executor.hpp"

#include <utility>

namespace mips_emulator {
    template <typename RegisterFile, typename Memory>
    class Emulator {
    public:
        using Register = typename RegisterFile::Register;
        using Address = typename RegisterFile::Unsigned;

        template <typename... Args>
        Emulator(Args&&... args) : memory(std::forward<Args>(args)...) {}

        RegisterFile get_register_file() const noexcept { return reg_file; }

        void step() noexcept {
            const Instruction instr = memory.template read<Instruction>(pc);

            switch (instr.get_type()) {
                    // R-Type
                case 0: {
                    Executor::handle_rtype_instr(instr, pc, reg_file);
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
        RegisterFile reg_file;
        Memory memory;

        Address pc;
    };
} // namespace mips_emulator
