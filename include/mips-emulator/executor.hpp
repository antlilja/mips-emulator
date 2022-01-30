#pragma once
#include "mips-emulator/instruction.hpp"

namespace mips_emulator {
    namespace Executor {
        template <typename RegisterFile, typename Memory>
        [[nodiscard]] inline static bool step(RegisterFile& reg_file,
                                              Memory& memory) {
            using Type = Instruction::Type;

            const Instruction instr =
                memory.template read<Instruction>(reg_file.get_pc());

            reg_file.inc_pc();

            switch (instr.get_type()) {
                case Type::e_rtype: return handle_rtype_instr(instr, reg_file);
                case Type::e_itype: return handle_itype_instr(instr, reg_file);
                case Type::e_j_type: {
                    // TODO: Handle J-Type instructions
                    return false;
                }
            }
        }

        template <typename RegisterFile>
        [[nodiscard]] inline static bool
        handle_rtype_instr(const Instruction instr, RegisterFile& reg_file) {

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
                    reg_file.set_pc(rs.u);
                    break;
                }
                case Func::e_jalr: {
                    reg_file.set_unsigned(31, reg_file.get_pc());
                    reg_file.set_pc(rs.u);
                    break;
                }
                case Func::e_sll: {
                    reg_file.set_unsigned(instr.rtype.rd, rt.u << instr.rtype.shamt);
                    break;
                }
                case Func::e_sllv: {
                    // rt is shifted left by the number specified by the lower 5 bits of rs and then stored in rd
                    reg_file.set_unsigned(instr.rtype.rd, rt.u << (rs.u & 0x1F));
                    break;
                }
                case Func::e_sra: {
                    // Arithmetic right shift for signed values are implementation dependent, doing this to ensure portability
                    if (rt.s < 0) {
                        reg_file.set_signed(instr.rtype.rd, rt.s >> instr.rtype.shamt | ~(~0U >> instr.rtype.shamt));
                    } else {
                        reg_file.set_signed(instr.rtype.rd, rt.s >> instr.rtype.shamt);
                    }                    
                    break;
                }
                case Func::e_srav: {
                    // Arithmetic right shift for signed values are implementation dependent, doing this to ensure portability
                    if (rt.s < 0) {
                        reg_file.set_signed(instr.rtype.rd, rt.s >> (rs.u & 0x1F) | ~(~0U >> (rs.u & 0x1F)));
                    } else {
                        reg_file.set_signed(instr.rtype.rd, rt.s >> (rs.u & 0x1F));
                    }
                    break;
                }
                case Func::e_srl: {
                    reg_file.set_unsigned(instr.rtype.rd, rt.u >> instr.rtype.shamt);
                    break;
                }
                case Func::e_srlv: {
                    // rt is shifted right by the number specified by the lower 5 bits of rs, inserting 0's, and then stored in rd
                    reg_file.set_unsigned(instr.rtype.rd, rt.u >> (rs.u & 0x1F));
                    break;
                }
                default: return false;
            }

            return true;
        }

        template <typename RegisterFile>
        [[nodiscard]] inline static bool
        handle_itype_instr(const Instruction instr, RegisterFile& reg_file) {

            using Register = typename RegisterFile::Register;
            using IOp = Instruction::ITypeOpcode;

            const Register rs = reg_file.get(instr.rtype.rs);

            const IOp op = static_cast<IOp>(instr.itype.op);

            switch (op) {
                case IOp::e_addi: {
                    reg_file.set_signed(instr.itype.rt, rs.s + instr.itype.imm);
                    break;
                }

                default: return false;
            }

            return true;
        }
    }; // namespace Executor
} // namespace mips_emulator
