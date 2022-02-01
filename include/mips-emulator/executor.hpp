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
                case Type::e_jtype: return handle_jtype_instr(instr, reg_file);
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
                    // TODO: Handle shift instructions
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
            const Register rt = reg_file.get(instr.rtype.rt);

            const auto sign_ext_imm = [](const typename RegisterFile::Unsigned imm) {
                const typename RegisterFile::Unsigned ext = ( ~((typename RegisterFile::Unsigned) 0)) << 16;
                return ((ext * ((imm >> 15) & 1)) | imm);
            };

            const IOp op = static_cast<IOp>(instr.itype.op);

            switch (op) {
                case IOp::e_beq: {
                    if (rt.u == rs.u) {
                        reg_file.set_pc( reg_file.get_pc() + (sign_ext_imm(instr.itype.imm) * 4) + 4);
                    }
                    break;
                }
                case IOp::e_bne: {
                    if (rt.u != rs.u) {
                        reg_file.set_pc( reg_file.get_pc() + (sign_ext_imm(instr.itype.imm) * 4) + 4);
                    }
                    break;
                }
                case IOp::e_addi: {
                    reg_file.set_signed(instr.itype.rt, rs.s + sign_ext_imm(instr.itype.imm));
                    break;
                }
                case IOp::e_addiu: {
                    reg_file.set_unsigned(instr.itype.rt, rs.u + sign_ext_imm(instr.itype.imm));
                    break;
                }
                case IOp::e_slti: {
                    reg_file.set_unsigned(instr.itype.rt, rs.s < (typename RegisterFile::Signed) sign_ext_imm(instr.itype.imm));
                    break;
                }
                case IOp::e_sltiu: {
                    reg_file.set_unsigned(instr.itype.rt, rs.u < sign_ext_imm(instr.itype.imm));
                    break;
                }
                case IOp::e_andi: {
                    reg_file.set_unsigned(instr.itype.rt, rs.u & instr.itype.imm);
                    break;
                }
                case IOp::e_ori: {
                    reg_file.set_unsigned(instr.itype.rt, rs.u | instr.itype.imm);
                    break;
                }
                case IOp::e_xori: {
                    reg_file.set_unsigned(instr.itype.rt, rs.u ^ instr.itype.imm);
                    break;
                }
                case IOp::e_lui: {
                    reg_file.set_unsigned(instr.itype.rt, (typename RegisterFile::Unsigned) instr.itype.imm << 16);
                    break;
                }

                default: return false;
            }

            return true;
        }

        template <typename RegisterFile>
        [[nodiscard]] inline static bool
        handle_jtype_instr(const Instruction instr, RegisterFile& reg_file) {

            using JOp = Instruction::JTypeOpcode;
            using Address = uint32_t;

            const Address address = static_cast<Address>(instr.jtype.address);
            const Address jta = (Address)(address << 2) | (Address)(reg_file.get_pc() & (0xf << 28));

            const JOp op = static_cast<JOp>(instr.jtype.op);
            
            switch (op) {
                case JOp::e_j: {
                    reg_file.set_pc(jta);
                    break;
                }
                case JOp::e_jal: {
                    reg_file.set_unsigned(RegisterName::e_ra, reg_file.get_pc() + 4);
                    reg_file.set_pc(jta);
                    break;
                }
                
                default: return false;
            }

            return true;
        }

    }; // namespace Executor
} // namespace mips_emulator
