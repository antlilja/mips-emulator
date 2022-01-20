#pragma once
#include "mips-emulator/instruction.hpp"

#include <cassert>

namespace mips_emulator {
    namespace Executor {
        template <typename RegisterFile, typename Memory>
        inline static void step(RegisterFile& reg_file, Memory& memory,
                                typename RegisterFile::Unsigned& pc) {
            const Instruction instr = memory.template read<Instruction>(pc);

            switch (instr.get_type()) {
                    // R-Type
                case 0: {
                    handle_rtype_instr(instr, pc, reg_file);
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

        template <typename RegisterFile>
        inline static void
        handle_rtype_instr(const Instruction instr,
                           typename RegisterFile::Unsigned& pc,
                           RegisterFile& reg_file) {

            using Register = typename RegisterFile::Register;
            using Func = Instruction::Func;

            const Register rs = reg_file.get(instr.rtype.rs);
            const Register rt = reg_file.get(instr.rtype.rt);

            const Func func = static_cast<Func>(instr.rtype.func);

            switch (func) {
                case Func::e_add: {
                    reg_file.set_signed(instr.rtype.rd, rs.s + rt.s);
                    break;
                }
                case Func::e_addu: {
                    reg_file.set_unsigned(instr.rtype.rd, rs.u + rt.u);
                    break;
                }
                case Func::e_sub: {
                    reg_file.set_signed(instr.rtype.rd, rs.s - rt.s);
                    break;
                }
                case Func::e_subu: {
                    reg_file.set_unsigned(instr.rtype.rd, rs.u - rt.u);
                    break;
                }
                case Func::e_mul: {
                    reg_file.set_signed(instr.rtype.rd, rs.s * rt.s);
                    break;
                }
                case Func::e_mulu: {
                    reg_file.set_unsigned(instr.rtype.rd, rs.u * rt.u);
                    break;
                }
                case Func::e_and: {
                    reg_file.set_unsigned(instr.rtype.rd, rs.u & rt.u);
                    break;
                }
                case Func::e_nor: {
                    reg_file.set_unsigned(instr.rtype.rd, ~(rs.u | rt.u));
                    break;
                }
                case Func::e_or: {
                    reg_file.set_unsigned(instr.rtype.rd, rs.u | rt.u);
                    break;
                }
                case Func::e_xor: {
                    reg_file.set_unsigned(instr.rtype.rd, rs.u ^ rt.u);
                    break;
                }
                case Func::e_jr: {
                    pc = rs.u;
                    break;
                }
                case Func::e_jalr: {
                    reg_file.set_unsigned(31, pc);
                    pc = rs.u;
                    break;
                }
                    // TODO: Handle shift instructions
                default: {
                    // TODO: Don't asserts for error handling?
                    assert(false);
                    break;
                }
            }
        }

        // TODO: Handle I and J type instructions
    }; // namespace Executor
} // namespace mips_emulator
