#pragma once
#include "mips-emulator/instruction.hpp"

#include <cassert>

namespace mips_emulator {
    template <typename RegisterState>
    struct Executor {
        using Register = typename RegisterState::Register;
        using Address = typename RegisterState::Unsigned;

        static void handle_rtype_instr(const Instruction instr, Address& pc,
                                       RegisterState& regs) {
            const Register rs = regs.get_register(instr.rtype.rs);
            const Register rt = regs.get_register(instr.rtype.rt);

            switch (instr.rtype.func) {
                case Instruction::RType::e_add: {
                    regs.set_register_signed(instr.rtype.rd, rs.s + rt.s);
                    break;
                }
                case Instruction::RType::e_addu: {
                    regs.set_register_unsigned(instr.rtype.rd, rs.u + rt.u);
                    break;
                }
                case Instruction::RType::e_sub: {
                    regs.set_register_signed(instr.rtype.rd, rs.s - rt.s);
                    break;
                }
                case Instruction::RType::e_subu: {
                    regs.set_register_unsigned(instr.rtype.rd, rs.u - rt.u);
                    break;
                }
                case Instruction::RType::e_mul: {
                    regs.set_register_signed(instr.rtype.rd, rs.s * rt.s);
                    break;
                }
                case Instruction::RType::e_mulu: {
                    regs.set_register_unsigned(instr.rtype.rd, rs.u * rt.u);
                    break;
                }
                case Instruction::RType::e_and: {
                    regs.set_register_unsigned(instr.rtype.rd, rs.u & rt.u);
                    break;
                }
                case Instruction::RType::e_nor: {
                    regs.set_register_unsigned(instr.rtype.rd, ~(rs.u | rt.u));
                    break;
                }
                case Instruction::RType::e_or: {
                    regs.set_register_unsigned(instr.rtype.rd, rs.u | rt.u);
                    break;
                }
                case Instruction::RType::e_xor: {
                    regs.set_register_unsigned(instr.rtype.rd, rs.u ^ rt.u);
                    break;
                }
                case Instruction::RType::e_jr: {
                    pc = rs.u;
                    break;
                }
                case Instruction::RType::e_jalr: {
                    regs.set_register_unsigned(31, pc);
                    pc = rs.u;
                    break;
                }
                    // TODO: Handle shift instructions
                default: {
                    assert(false);
                    break;
                }
            }
        }

        // TODO: Handle I and J type instructions
    };
} // namespace mips_emulator
