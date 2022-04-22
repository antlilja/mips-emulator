#pragma once
#include "mips-emulator/instruction.hpp"
#include "memory.hpp"
#include "register_file.hpp"

namespace mips_emulator {
    namespace Executor {
        // Returns the higher 32 bits of a multiplication
        template <typename small_t, typename big_t>
        small_t hi_mul(small_t a, small_t b) {
            const big_t a64 = static_cast<big_t>(a);
            const big_t b64 = static_cast<big_t>(b);
            return static_cast<small_t>((a64 * b64) >> 32);
        };

        [[nodiscard]] inline static bool
        handle_rtype_instr(const Instruction instr, RegisterFile& reg_file) {

            using Register = RegisterFile::Register;
            using Func = Instruction::Func;

            const Register rs = reg_file.get(instr.rtype.rs);
            const Register rt = reg_file.get(instr.rtype.rt);

            const Func func = static_cast<Func>(instr.rtype.func);

            // SOP helper function
            // GPR[rd] <- shamt2 if shamt = 2 else shamt 3.
            auto sop_set_rd = [&](uint32_t shamt2, uint32_t shamt3) {
                reg_file.set_signed(instr.rtype.rd,
                                    instr.rtype.shamt == 2 ? shamt2 : shamt3);
            };

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
                case Func::e_sop30: { // Shamt: 2 = mul, 3 = muh
                    sop_set_rd(rs.s * rt.s,
                               hi_mul<int32_t, int64_t>(rs.s, rt.s));
                    break;
                }
                case Func::e_sop31: { // Shamt: 2 = mulu, 3 = muhu
                    sop_set_rd(rs.u * rt.u,
                               hi_mul<uint32_t, uint64_t>(rs.u, rt.u));
                    break;
                }
                case Func::e_sop32: { // Shamt: 2 = div, 3 = mod
                    // division by zero check
                    if (rt.s == 0) return false;

                    sop_set_rd(rs.s / rt.s, rs.s % rt.s);
                    break;
                }
                case Func::e_sop33: { // Shamt: 2 = divu, 3 = modu
                    // division by zero check
                    if (rt.s == 0) return false;

                    sop_set_rd(rs.u / rt.u, rs.u % rt.u);
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
                    reg_file.delayed_branch(rs.u);
                    break;
                }
                case Func::e_slt: {
                    reg_file.set_unsigned(instr.rtype.rd, rs.s < rt.s);
                    break;
                }
                case Func::e_sltu: {
                    reg_file.set_unsigned(instr.rtype.rd, rs.u < rt.u);
                    break;
                }
                case Func::e_jalr: {
                    reg_file.set_unsigned(RegisterName::e_ra,
                                          reg_file.get_pc());
                    reg_file.delayed_branch(rs.u);
                    break;
                }
                case Func::e_sll: {
                    reg_file.set_unsigned(instr.rtype.rd,
                                          rt.u << instr.rtype.shamt);
                    break;
                }
                case Func::e_sllv: {
                    // rt is shifted left by the number specified by the lower 5
                    // bits of rs and then stored in rd
                    reg_file.set_unsigned(instr.rtype.rd,
                                          rt.u << (rs.u & 0x1F));
                    break;
                }
                case Func::e_sra: {
                    // Arithmetic right shift for signed values are
                    // implementation dependent, doing this to ensure
                    // portability
                    const auto reg_bit_size =
                        (sizeof(RegisterFile::Unsigned) * 8);
                    const RegisterFile::Unsigned ext =
                        (~0) << (reg_bit_size - instr.rtype.shamt);
                    reg_file.set_unsigned(
                        instr.rtype.rd,
                        (ext * ((rt.u >> (reg_bit_size - 1)) & 1)) |
                            rt.u >> instr.rtype.shamt);
                    break;
                }
                case Func::e_srav: {
                    // Arithmetic right shift for signed values are
                    // implementation dependent, doing this to ensure
                    // portability
                    const auto shift_amount = rs.u & 0x1F;
                    const auto reg_bit_size =
                        (sizeof(RegisterFile::Unsigned) * 8);
                    const RegisterFile::Unsigned ext =
                        (~0) << (reg_bit_size - shift_amount);
                    reg_file.set_unsigned(
                        instr.rtype.rd,
                        (ext * ((rt.u >> (reg_bit_size - 1)) & 1)) |
                            rt.u >> shift_amount);
                    break;
                }
                case Func::e_srl: {
                    auto res = rt.u >> instr.rtype.shamt;

                    // ROTR: Rotate word if rs field & 1.
                    if (instr.rtype.rs & 1)
                        res |= (rt.u << (32 - instr.rtype.shamt));

                    reg_file.set_unsigned(instr.rtype.rd, res);
                    break;
                }
                case Func::e_srlv: {
                    // rt is shifted right by the number specified by the lower
                    // 5 bits of rs, inserting 0's, and then stored in rd
                    const auto shift = rs.u & 0x1F;
                    auto res = rt.u >> shift;

                    // ROTRV: Rotate word if shamt & 1.
                    if (instr.rtype.shamt & 1) res |= (rt.u << (32 - shift));

                    reg_file.set_unsigned(instr.rtype.rd, res);
                    break;
                }
                case Func::e_seleqz: {
                    reg_file.set_unsigned(instr.rtype.rd, rt.u ? 0 : rs.u);
                    break;
                }
                case Func::e_selnez: {
                    reg_file.set_unsigned(instr.rtype.rd, rt.u ? rs.u : 0);
                    break;
                }
                case Func::e_clz: {
                    // Counts the number of leading zeros
                    auto x = rs.s;
                    auto count = (x == 0) ? sizeof(x) * 8 : 0;
                    // x is interpret as a two-complements number, so
                    // negative means a leading one which means we're done.
                    // x == 0 is handled above
                    while (x > 0) {
                        count++;
                        x <<= 1;
                    }
                    reg_file.set_unsigned(instr.rtype.rd, count);
                    break;
                }
                case Func::e_clo: {
                    // Count the number of leading ones
                    auto x = rs.s;
                    auto count = 0;
                    // x is interpret as a two-complements number, so
                    // anything other than a negative number means we're done
                    // counting.
                    while (x < 0) {
                        count++;
                        x <<= 1;
                    }
                    reg_file.set_unsigned(instr.rtype.rd, count);
                    break;
                }
                default: return false;
            }

            return true;
        }

        template <typename T>
        const uint32_t sign_ext_imm(const T imm) {
            const uint32_t ext = (~0U) << 16;
            return ((ext * ((imm >> 15) & 1)) | imm);
        }

        [[nodiscard]] inline static bool
        handle_itype_instr(const Instruction instr, RegisterFile& reg_file) {
            using Register = RegisterFile::Register;
            using IOp = Instruction::ITypeOpcode;

            const Register rs = reg_file.get(instr.rtype.rs);
            const Register rt = reg_file.get(instr.rtype.rt);

            const IOp op = static_cast<IOp>(instr.itype.op);

            // set PC after successful branch
            const uint32_t branch_target =
                reg_file.get_pc() + (sign_ext_imm(instr.itype.imm) * 4);

            switch (op) {
                case IOp::e_beq: {
                    if (rt.u == rs.u) {
                        reg_file.delayed_branch(branch_target);
                    }
                    break;
                }
                case IOp::e_bne: {
                    if (rt.u != rs.u) {
                        reg_file.delayed_branch(branch_target);
                    }
                    break;
                }

                case IOp::e_addiu: {
                    reg_file.set_unsigned(instr.itype.rt,
                                          rs.u + sign_ext_imm(instr.itype.imm));
                    break;
                }
                case IOp::e_aui: {
                    reg_file.set_unsigned(
                        instr.itype.rt,
                        rs.u + sign_ext_imm(instr.itype.imm << 16));
                    break;
                }
                case IOp::e_slti: {
                    reg_file.set_unsigned(
                        instr.itype.rt,
                        rs.s < (RegisterFile::Signed)sign_ext_imm(
                                   instr.itype.imm));
                    break;
                }
                case IOp::e_sltiu: {
                    reg_file.set_unsigned(instr.itype.rt,
                                          rs.u < sign_ext_imm(instr.itype.imm));
                    break;
                }
                case IOp::e_andi: {
                    reg_file.set_unsigned(instr.itype.rt,
                                          rs.u & instr.itype.imm);
                    break;
                }
                case IOp::e_ori: {
                    reg_file.set_unsigned(instr.itype.rt,
                                          rs.u | instr.itype.imm);
                    break;
                }
                case IOp::e_xori: {
                    reg_file.set_unsigned(instr.itype.rt,
                                          rs.u ^ instr.itype.imm);
                    break;
                }

                //  HERE LIES MADNESS... i hate POP
                case IOp::e_pop06: {
                    if (instr.itype.rt == 0) {
                        // BLEZ
                        if (rs.s <= 0) {
                            reg_file.delayed_branch(branch_target);
                        }
                    }
                    else if (instr.itype.rs == 0 && instr.itype.rt != 0) {
                        // BLEZALC
                        if (rt.s <= 0) {
                            reg_file.set_unsigned(31, reg_file.get_pc());
                            reg_file.set_pc(
                                reg_file.get_pc() +
                                (sign_ext_imm(instr.itype.imm) * 4));
                        }
                    }
                    else if (instr.itype.rs == instr.itype.rt &&
                             instr.itype.rt != 0) {
                        // BGEZALC
                        if (rt.s >= 0) {
                            reg_file.set_unsigned(31, reg_file.get_pc());
                            reg_file.set_pc(
                                reg_file.get_pc() +
                                (sign_ext_imm(instr.itype.imm) * 4));
                        }
                    }
                    else if (instr.itype.rs != instr.itype.rt &&
                             instr.itype.rs != 0 && instr.itype.rt != 0) {
                        // BGEUC
                        if (rs.u >= rt.u) {
                            reg_file.set_pc(
                                reg_file.get_pc() +
                                (sign_ext_imm(instr.itype.imm) * 4));
                        }
                    }
                    break;
                }
                case IOp::e_pop07: {
                    if (instr.itype.rt == 0) {
                        // BGTZ
                        if (rs.s > 0) {
                            reg_file.delayed_branch(branch_target);
                        }
                    }
                    else if (instr.itype.rs == 0 && instr.itype.rt != 0) {
                        // BGTZALC
                        if (rt.s > 0) {
                            reg_file.set_unsigned(31, reg_file.get_pc());
                            reg_file.set_pc(
                                reg_file.get_pc() +
                                (sign_ext_imm(instr.itype.imm) * 4));
                        }
                    }
                    else if (instr.itype.rs == instr.itype.rt &&
                             instr.itype.rt != 0) {
                        // BLTZALC
                        if (rt.s < 0) {
                            reg_file.set_unsigned(31, reg_file.get_pc());
                            reg_file.set_pc(
                                reg_file.get_pc() +
                                (sign_ext_imm(instr.itype.imm) * 4));
                        }
                    }
                    else if (instr.itype.rs != instr.itype.rt &&
                             instr.itype.rs != 0 && instr.itype.rt != 0) {
                        // BLTUC
                        if (rs.u < rt.u) {
                            reg_file.set_pc(
                                reg_file.get_pc() +
                                (sign_ext_imm(instr.itype.imm) * 4));
                        }
                    }
                    break;
                }

                case IOp::e_pop10: {
                    if (instr.itype.rs == 0 && instr.itype.rt != 0 &&
                        instr.itype.rs < instr.itype.rt) { // rs < rt???????
                        // BEQZALC
                        if (!rt.u) {
                            reg_file.set_unsigned(31, reg_file.get_pc());
                            reg_file.set_pc(
                                reg_file.get_pc() +
                                (sign_ext_imm(instr.itype.imm) * 4));
                        }
                    }
                    else if (instr.itype.rs != 0 && instr.itype.rt != 0 &&
                             instr.itype.rs <
                                 instr.itype.rt) { // rs < rt???????
                        // BEQC
                        if (rt.u == rs.u) reg_file.set_pc(branch_target);
                    }
                    else if (instr.itype.rs >= instr.itype.rt) {
                        // BOVC

                        const bool carry = rs.u + rt.u < rs.u;
                        const bool is_signed = ((rs.u + rt.u) & 0x80000000) > 0;
                        const bool sum_overflow = carry != is_signed;
                        if (sum_overflow) reg_file.set_pc(branch_target);
                    }
                    break;
                }
                case IOp::e_pop30: {
                    if (instr.itype.rs == 0 && instr.itype.rt != 0 &&
                        instr.itype.rs < instr.itype.rt) {

                        // BNEZALC
                        if (rt.u) {
                            reg_file.set_unsigned(31, reg_file.get_pc());
                            reg_file.set_pc(
                                reg_file.get_pc() +
                                (sign_ext_imm(instr.itype.imm) * 4));
                        }
                    }
                    else if (instr.itype.rs != 0 && instr.itype.rt != 0 &&
                             instr.itype.rs < instr.itype.rt) {
                        // BNEC
                        if (rt.u != rs.u) reg_file.set_pc(branch_target);
                    }
                    else if (instr.itype.rs >= instr.itype.rt) {
                        // BNVC
                        const bool carry = rs.u + rt.u < rs.u;
                        const bool is_signed = ((rs.u + rt.u) & 0x80000000) > 0;
                        const bool sum_overflow = carry != is_signed;
                        if (!sum_overflow) reg_file.set_pc(branch_target);
                    }
                    break;
                }

                case IOp::e_pop26: {
                    if (instr.itype.rs == 0 && instr.itype.rt != 0) {
                        // BLEZC
                    }
                    else if (instr.itype.rs != 0 && instr.itype.rt != 0 &&
                             instr.itype.rt == instr.itype.rs) {
                        // BGEZC
                    }
                    else if (instr.itype.rs != 0 && instr.itype.rt != 0 &&
                             instr.itype.rt != instr.itype.rs) {
                        // BGEC
                    }
                    break;
                }
                case IOp::e_pop27: {
                    if (instr.itype.rs == 0 && instr.itype.rt != 0) {
                        // BGTZC
                    }
                    else if (instr.itype.rs != 0 && instr.itype.rt != 0 &&
                             instr.itype.rt == instr.itype.rs) {
                        // BLTZC
                    }
                    else if (instr.itype.rs != 0 && instr.itype.rt != 0 &&
                             instr.itype.rt != instr.itype.rs) {
                        // BLTC
                    }
                    break;
                }

                case IOp::e_pop66: {
                    // TODO
                    // Maybe these should be their own Instruction type?????
                    break;
                }
                case IOp::e_pop76: {
                    // TODO
                    // Maybe these should be their own Instruction type?????
                    break;
                }

                default: return false;
            }

            return true;
        }

        template <typename Memory>
        [[nodiscard]] inline static bool
        handle_itype_instr(const Instruction instr, RegisterFile& reg_file,
                           Memory& memory) {
            using Register = RegisterFile::Register;
            using IOp = Instruction::ITypeOpcode;
            const Register rs = reg_file.get(instr.rtype.rs);
            const Register rt = reg_file.get(instr.rtype.rt);

            const IOp op = static_cast<IOp>(instr.itype.op);

            const auto store_val = [&](auto val) {
                const uint32_t address = rs.u + sign_ext_imm(instr.itype.imm);
                auto store_result =
                    memory.template store<decltype(val)>(address, val);

                return !store_result.is_error();
            };

            // Use Unused Variable a for its type.
            const auto load_val = [&](auto a) {
                const uint32_t address = rs.u + sign_ext_imm(instr.itype.imm);
                auto read_result = memory.template read<decltype(a)>(address);

                if (read_result.is_error()) return false;

                reg_file.set_signed(
                    instr.itype.rt,
                    static_cast<int32_t>(read_result.get_value()));

                return true;
            };

            const auto load_val_unsigned = [&](auto a) {
                const uint32_t address = rs.u + sign_ext_imm(instr.itype.imm);
                auto read_result = memory.template read<decltype(a)>(address);

                if (read_result.is_error()) return false;

                reg_file.set_unsigned(
                    instr.itype.rt,
                    static_cast<uint32_t>(read_result.get_value()));

                return true;
            };

            switch (op) {
                // Use signed int to automatically byte extend
                case IOp::e_lb: return load_val((int8_t)0);
                case IOp::e_lh: return load_val((int16_t)0);
                case IOp::e_lw: return load_val((int32_t)0);

                case IOp::e_lbu: return load_val_unsigned((uint8_t)0);
                case IOp::e_lhu: return load_val_unsigned((uint16_t)0);

                case IOp::e_sb: return store_val((uint8_t)rt.u);
                case IOp::e_sh: return store_val((uint16_t)rt.u);
                case IOp::e_sw: return store_val(rt.u);

                default: break;
            }

            return handle_itype_instr(instr, reg_file);
        }

        [[nodiscard]] inline static bool
        handle_jtype_instr(const Instruction instr, RegisterFile& reg_file) {

            using JOp = Instruction::JTypeOpcode;
            using Address = uint32_t;

            const Address address = static_cast<Address>(instr.jtype.address);
            const Address jta = (Address)(address << 2) |
                                (Address)(reg_file.get_pc() & (0xf << 28));

            const JOp op = static_cast<JOp>(instr.jtype.op);
            switch (op) {
                case JOp::e_j: {
                    reg_file.delayed_branch(jta);
                    break;
                }
                case JOp::e_jal: {
                    reg_file.set_unsigned(RegisterName::e_ra,
                                          reg_file.get_pc());
                    reg_file.delayed_branch(jta);
                    break;
                }

                default: return false;
            }

            return true;
        }

        template <typename RegisterFile>
        [[nodiscard]] inline static bool
        handle_special3_rtype_instr(const Instruction instr,
                                    RegisterFile& reg_file) {
            using Register = typename RegisterFile::Register;
            using ROp = Instruction::Special3RTypeOp;

            const Register rt = reg_file.get(instr.special3_rtype.rt);

            const ROp op = static_cast<ROp>(instr.special3_rtype.op);
            switch (op) {
                case ROp::e_bitswap: {
                    // Swaps (reverses) bits in a byte
                    // Example: 0b11001000 -> 0b00010011
                    const auto reverse_byte_bits = [&](uint8_t val) {
                        val = ((val >> 1) & 0x55) | ((val & 0x55) << 1);
                        val = ((val >> 2) & 0x33) | ((val & 0x33) << 2);
                        val = ((val >> 4) & 0x0f) | ((val & 0x0f) << 4);
                        return val;
                    };

                    // Swaps (reverses) the bits for each byte
                    uint32_t result = 0;
                    for (int i = 0; i < sizeof(uint32_t); i++)
                        result |= reverse_byte_bits((rt.u >> i * 8)) << (i * 8);

                    reg_file.set_unsigned(instr.special3_rtype.rd, result);

                    break;
                }
                case ROp::e_wsbh: {
                    // Word Swap Bytes Within Halfwords
                    reg_file.set_unsigned(instr.special3_rtype.rd,
                                          ((rt.u & 0xFF) << 8) |
                                              ((rt.u & 0xFF00) >> 8) |
                                              ((rt.u & 0xFF0000) << 8) |
                                              ((rt.u & 0xFF000000) >> 8));
                    break;
                }
                case ROp::e_seb: {
                    // Sign-extend Byte
                    reg_file.set_unsigned(instr.special3_rtype.rd,
                                          (((~0U) << 8) * ((rt.u >> 7) & 1)) |
                                              (rt.u & 0xFF));
                    break;
                }
                case ROp::e_seh: {
                    // Sign-extend Halfword
                    reg_file.set_unsigned(instr.special3_rtype.rd,
                                          (((~0U) << 16) * ((rt.u >> 15) & 1)) |
                                              (rt.u & 0xFFFF));
                    break;
                }
                default: return false;
            }

            return true;
        }

        [[nodiscard]] inline static bool
        handle_regimm_itype_instr(const Instruction instr,
                                  RegisterFile& reg_file) {

            using Register = RegisterFile::Register;
            using IOp = Instruction::RegimmITypeOp;

            const Register rs = reg_file.get(instr.regimm_itype.rs);

            const IOp op = static_cast<IOp>(instr.regimm_itype.op);
            switch (op) {
                case IOp::e_bgez: {
                    if (rs.s >= 0) {
                        reg_file.delayed_branch(
                            reg_file.get_pc() +
                            (sign_ext_imm(instr.itype.imm) * 4));
                    }
                    break;
                }
                case IOp::e_bltz: {
                    if (rs.s < 0) {
                        reg_file.delayed_branch(
                            reg_file.get_pc() +
                            (sign_ext_imm(instr.itype.imm) * 4));
                    }
                    break;
                }

                default: return false;
            }

            return true;
        }

        template <typename Memory>
        [[nodiscard]] inline static bool step(RegisterFile& reg_file,
                                              Memory& memory) {
            using Type = Instruction::Type;

            auto read_result =
                memory.template read<uint32_t>(reg_file.get_pc());

            if (read_result.is_error()) return false;
            const auto instr = Instruction(read_result.get_value());

            reg_file.update_pc();

            switch (instr.get_type()) {
                case Type::e_rtype: return handle_rtype_instr(instr, reg_file);
                case Type::e_itype:
                    return handle_itype_instr(instr, reg_file, memory);
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
                case Type::e_special3_rtype:
                    return handle_special3_rtype_instr(instr, reg_file);
                case Type::e_regimm_itype:
                    return handle_regimm_itype_instr(instr, reg_file);

                default: return false;
            }
        }
    }; // namespace Executor
} // namespace mips_emulator
