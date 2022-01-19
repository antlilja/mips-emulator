#pragma once
#include "mips-emulator/instruction.hpp"

#include <cassert>

namespace mips_emulator {
    template <typename RegisterFile>
    struct Executor {
        using Register = typename RegisterFile::Register;
        using Address = typename RegisterFile::Unsigned;

        static void handle_rtype_instr(const Instruction instr, Address& pc,
                                       RegisterFile& reg_file) {
            const Register rs = reg_file.get_register(instr.rtype.rs);
            const Register rt = reg_file.get_register(instr.rtype.rt);

            using Func = Instruction::Func;

            const Func func = static_cast<Func>(instr.rtype.func);

            switch (func) {
                case Func::e_add: {
                    reg_file.set_register_signed(instr.rtype.rd, rs.s + rt.s);
                    break;
                }
                case Func::e_addu: {
                    reg_file.set_register_unsigned(instr.rtype.rd, rs.u + rt.u);
                    break;
                }
                case Func::e_sub: {
                    reg_file.set_register_signed(instr.rtype.rd, rs.s - rt.s);
                    break;
                }
                case Func::e_subu: {
                    reg_file.set_register_unsigned(instr.rtype.rd, rs.u - rt.u);
                    break;
                }
                case Func::e_mul: {
                    reg_file.set_register_signed(instr.rtype.rd, rs.s * rt.s);
                    break;
                }
                case Func::e_mulu: {
                    reg_file.set_register_unsigned(instr.rtype.rd, rs.u * rt.u);
                    break;
                }
                case Func::e_and: {
                    reg_file.set_register_unsigned(instr.rtype.rd, rs.u & rt.u);
                    break;
                }
                case Func::e_nor: {
                    reg_file.set_register_unsigned(instr.rtype.rd,
                                                   ~(rs.u | rt.u));
                    break;
                }
                case Func::e_or: {
                    reg_file.set_register_unsigned(instr.rtype.rd, rs.u | rt.u);
                    break;
                }
                case Func::e_xor: {
                    reg_file.set_register_unsigned(instr.rtype.rd, rs.u ^ rt.u);
                    break;
                }
                case Func::e_jr: {
                    pc = rs.u;
                    break;
                }
                case Func::e_jalr: {
                    reg_file.set_register_unsigned(31, pc);
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
    };
} // namespace mips_emulator
