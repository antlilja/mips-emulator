#pragma once
#include "mips-emulator/instruction.hpp"

namespace mips_emulator {
    namespace Executor {
        template <typename RegisterFile, typename Memory>
        [[nodiscard]] inline static bool
        step(RegisterFile& reg_file, Memory& memory,
             typename RegisterFile::Unsigned& pc) {
            using Type = Instruction::Type;

            const Instruction instr = memory.template read<Instruction>(pc);

            pc += sizeof(Instruction);

            switch (instr.get_type()) {
                case Type::e_rtype:
                    return handle_rtype_instr(instr, pc, reg_file);
                case Type::e_itype: {
                    // TODO: Handle I-Type instructions
                    return false;
                }
                case Type::e_j_type: {
                    // TODO: Handle J-Type instructions
                    return false;
                }
                case Type::e_fpu_rtype: {
                    // TODO: Handle FPU R-Type instructions
                    return false;
                }
                case Type::e_fpu_ttype: {
                    // TODO: Handle FPU Transfer instructions
                    return false;
                }
                case Type::e_fpu_btype: {
                    // TODO: Handle FPU Brach instructions
                    return false;
                }
            }
        }

        template <typename RegisterFile>
        [[nodiscard]] inline static bool
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
                default: return false;
            }

            return true;
        }
    }; // namespace Executor
} // namespace mips_emulator
