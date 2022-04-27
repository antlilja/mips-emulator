#pragma once
#include "mips-emulator/register_name.hpp"
#include "mips-emulator/result.hpp"

#include <cstdint>

// Macros used to make sure structs are packed to fit the MIPS instruction size

// GCC and Clang version
#if defined(__GNUC__) || defined(__clang__)
#    define PACKED(__Declaration__) __Declaration__ __attribute__((__packed__))
#endif

// MSVC version
#ifdef _MSC_VER
#    define PACKED(__Declaration__) \
        __pragma(pack(push, 1)) __Declaration__ __pragma(pack(pop))
#endif

// Error for unsupported compilers
#ifndef PACKED
#    error "Unsupported compiler"
#endif

namespace mips_emulator {
    union Instruction {
        enum class Type {
            e_rtype,
            e_itype,
            e_jtype,
            e_fpu_rtype,
            e_fpu_btype,
            e_fpu_ttype,
            e_special3_type_bshfl,
            e_special3_type_ext,
            e_special3_type_ins,
            e_regimm_itype,
            e_pcrel_type1,
            e_pcrel_type2,
            e_longimm_itype,
        };

        enum class Func : uint8_t {
            e_add = 32,
            e_addu = 33,
            e_sub = 34,
            e_subu = 35,
            e_sop30 = 0b011000, // SOP30 - shamt: 2 = mul,  3 = muh
            e_sop31 = 0b011001, // SOP31 - shamt: 2 = mulu, 3 = muhu
            e_sop32 = 0b011010, // SOP32 - shamt: 2 = div,  3 = mod
            e_sop33 = 0b011011, // SOP33 - shamt: 2 = divu, 3 = modu
            e_seleqz = 0b110101,
            e_selnez = 0b110111,
            e_and = 36,
            e_nor = 39,
            e_or = 37,
            e_xor = 38,
            e_jr = 8,
            e_jalr = 9,
            e_slt = 42,
            e_sltu = 43,
            e_sll = 0,
            e_sllv = 4,
            e_sra = 3,
            e_srav = 7,
            e_srl = 2,
            e_srlv = 6,
            e_clz = 16,
            e_clo = 11,
            e_teq = 0b110100,
            e_tge = 0b110000,
            e_tgeu = 0b110001,
            e_tlt = 0b110010,
            e_tltu = 0b110011,
            e_tne = 0b110110,
        };

        // Enum for FPUR-type instructions
        enum class FPUFunc : uint8_t {
            e_abs = 0b000101,
            e_add = 0b000000,
            e_ceil_l = 0b001010,
            e_ceil_w = 0b001110,
            e_class = 0b011011,
            e_cvt_d = 0b100001,
            e_cvt_l = 0b100101,
            e_cvt_s = 0b100000,
            e_cvt_w = 0b100100,
            e_div = 0b000011,
            e_floor_l = 0b001011,
            e_floor_w = 0b001111,
            e_maddf = 0b011000,
            e_msubf = 0b011001,
            e_max = 0b011110,
            e_maxa = 0b011111,
            e_min = 0b011100,
            e_mina = 0b011101,
            e_mov = 0b000110,
            e_mul = 0b000010,
            e_neg = 0b000111,
            e_recip = 0b010101,
            e_rint = 0b011010,
            e_round_l = 0b001000,
            e_round_w = 0b001100,
            e_rsqrt = 0b010110,
            e_sel = 0b010000,
            e_seleqz = 0b010100,
            e_selnqz = 0b010111,
            e_sqrt = 0b000100,
            e_sub = 0b000001,
            e_trunc_l = 0b001001,
            e_trunc_w = 0b001101,
        };

        enum class ITypeOpcode : uint8_t {
            e_beq = 4,
            e_bne = 5,
            e_addiu = 9,
            e_aui = 15,
            e_slti = 10,
            e_sltiu = 11,
            e_andi = 12,
            e_ori = 13,
            e_xori = 14,
            e_lb = 32,
            e_lbu = 36,
            e_lw = 35,
            e_sb = 40,
            e_sw = 43,
            e_lh = 0b100001,
            e_lhu = 0b100101,
            e_sh = 0b101001,
            e_pop06 = 0b000110,
            e_pop07 = 0b000111,
            e_pop10 = 0b001000,
            e_pop26 = 0b010110,
            e_pop27 = 0b010111,
            e_pop30 = 0b011000,
            e_pop66 = 0b110110,
            e_pop76 = 0b111110,

        };

        enum class JTypeOpcode : uint8_t {
            e_j = 2,
            e_jal = 3,
            e_bc = 0b110010,
            e_balc = 0b111010,
        };

        // Opcode for coprocessor instruction
        enum class COPOpcode : uint8_t {
            e_cop0 = 0b010000,
            e_cop1 = 0b010001,
            e_cop2 = 0b010010,
        };

        enum class FPUTTypeOp {
            e_mf = 0,
            e_cf = 2,
            e_mfh = 3,
            e_mt = 4,
            e_ct = 6,
            e_mth = 7,
        };

        enum class FPUBTypeOp {
            e_bc1eqz = 9,  // Branch if bit 0 = 0
            e_bc1nez = 13, // Branch if bit 0 != 0
        };

        // Floating Point Format field
        enum class FPURTypeOp : uint8_t {
            e_fmt_s = 16, // Use Single
            e_fmt_d = 17, // Use Double
            e_fmt_w = 20, // Use 32 bit
            e_fmt_l = 21, // Use 64 bit
            e_cmp_condn_s = 24,
            e_cmp_condn_d = 25,
        };

        // Func enum for special3 instructions
        enum class Special3Func : uint8_t {
            e_ext = 0,
            e_ins = 0b000100,
            e_bshfl = 0b100000,
        };

        // Opcode enum for special3 bshfl instructions
        // Special3 instructions have a bunch of different layouts depending on
        // the func field
        enum class Special3BSHFLFunc : uint8_t {
            e_bitswap = 0,
            e_wsbh = 0b00010,
            e_seh = 0b11000,
            e_seb = 0b10000,
        };

        // regimm function types are dependent on value in bits 16 to 20
        enum class RegimmITypeOp : uint8_t {
            e_bgez = 1,
            e_bltz = 0,
        };

        // PC-relative functions
        enum class PCRelFunc1 : uint8_t {
            e_addiupc = 0,
            e_lwpc = 1,
        };

        enum class PCRelFunc2 : uint8_t {
            e_auipc = 30,
            e_aluipc = 31,
        };

        PACKED(struct General {
            uint32_t reserved : 26;
            uint32_t op : 6;
        });

        PACKED(struct RType {
            uint32_t func : 6;
            uint32_t shamt : 5;
            uint32_t rd : 5;
            uint32_t rt : 5;
            uint32_t rs : 5;
            uint32_t zero : 6;
        });

        PACKED(struct IType {
            uint32_t imm : 16;
            uint32_t rt : 5;
            uint32_t rs : 5;
            uint32_t op : 6;
        });

        PACKED(struct JType {
            uint32_t address : 26;
            uint32_t op : 6;
        });

        // Coprocessor 1 structs
        // These are made up names, as I (Emil) couldn't find any specified
        // type names in the mip32 specifications.
        // FPURType is called so because it's similar to R type instructions
        // FPUBType is called so because it does branching.
        // FPUTType is called so because it (T)ransfers to and from the FPU

        PACKED(struct FPURType {
            uint32_t func : 6;
            uint32_t fd : 5;
            uint32_t fs : 5;
            uint32_t ft : 5;
            uint32_t fmt : 5;
            uint32_t cop1 : 6;
        });

        PACKED(struct FPUBType {
            uint32_t offset : 16;
            uint32_t ft : 5;
            uint32_t bc : 5;
            uint32_t cop1 : 6;
        });

        PACKED(struct FPUTType {
            uint32_t zero : 11;
            uint32_t fs : 5;
            uint32_t rt : 5;
            uint32_t op : 5;
            uint32_t cop1 : 6;
        });

        // Special3 structs
        PACKED(struct Special3Type {
            uint32_t func : 6;
            uint32_t extra : 5;
            uint32_t rd : 5;
            uint32_t rt : 5;
            uint32_t rs : 5;
            uint32_t special3 : 6;
        });

        PACKED(struct Special3TypeBSHFL {
            uint32_t bshfl : 6;
            uint32_t func : 5;
            uint32_t rd : 5;
            uint32_t rt : 5;
            uint32_t zero : 5;
            uint32_t special3 : 6;
        });

        PACKED(struct Special3TypeEXT {
            uint32_t ext : 6;
            uint32_t lsb : 5;
            uint32_t msbd : 5;
            uint32_t rt : 5;
            uint32_t rs : 5;
            uint32_t special3 : 6;
        });

        PACKED(struct Special3TypeINS {
            uint32_t ins : 6;
            uint32_t lsb : 5;
            uint32_t msb : 5;
            uint32_t rt : 5;
            uint32_t rs : 5;
            uint32_t special3 : 6;
        });

        // regimm structs
        // similar to itype instructions but all share the same op-code
        // function type is determined by bits 16-20
        PACKED(struct RegimmIType {
            uint32_t imm : 16;
            uint32_t op : 5;
            uint32_t rs : 5;
            uint32_t regimm : 6;
        });

        // Struct for PC relative instructions
        PACKED(struct PCRelType1 {
            uint32_t imm : 19;
            uint32_t func : 2;
            uint32_t rs : 5;
            uint32_t pcrel : 6;
        });

        PACKED(struct PCRelType2 {
            uint32_t imm : 16;
            uint32_t func : 5;
            uint32_t rs : 5;
            uint32_t pcrel : 6;
        });

        PACKED(struct LongimmIType {
            uint32_t imm : 21;
            uint32_t rs : 5;
            uint32_t op : 6;
        });

        // Make sure internal structs are the same size
        static_assert(sizeof(General) == 4,
                      "Instruction::General bitfield is not 4 bytes in size");
        static_assert(sizeof(RType) == 4,
                      "Instruction::RType bitfield is not 4 bytes in size");
        static_assert(sizeof(IType) == 4,
                      "Instruction::IType bitfield is not 4 bytes in size");
        static_assert(sizeof(JType) == 4,
                      "Instruction::JType bitfield is not 4 bytes in size");
        static_assert(sizeof(FPURType) == 4,
                      "Instruction::FPURType bitfield is not 4 bytes in size");
        static_assert(sizeof(FPUBType) == 4,
                      "Instruction::FPUIType bitfield is not 4 bytes in size");
        static_assert(sizeof(FPUTType) == 4,
                      "Instruction::FPUTType bitfield is not 4 bytes in size");
        static_assert(
            sizeof(Special3Type) == 4,
            "Instruction::Special3Type bitfield is not 4 bytes in size");
        static_assert(
            sizeof(Special3TypeBSHFL) == 4,
            "Instruction::Special3TypeBSHFL bitfield is not 4 bytes in size");
        static_assert(
            sizeof(Special3TypeEXT) == 4,
            "Instruction::Special3TypeEXT bitfield is not 4 bytes in size");
        static_assert(
            sizeof(Special3TypeINS) == 4,
            "Instruction::Special3TypeINS bitfield is not 4 bytes in size");
        static_assert(
            sizeof(RegimmIType) == 4,
            "Instruction::RegimmIType bitfield is not 4 bytes in size");
        static_assert(
            sizeof(PCRelType1) == 4,
            "Instruction::PCRelType1 bitfield is not 4 bytes in size");
        static_assert(
            sizeof(PCRelType2) == 4,
            "Instruction::PCRelType2 bitfield is not 4 bytes in size");

        static constexpr uint8_t RTYPE_OPCODE = 0;
        static constexpr uint8_t REGIMM_OPCODE = 1;
        static constexpr uint8_t SPECIAL3_OPCODE = 31;
        static constexpr uint8_t PCREL_OPCODE = 59;

        // R-Type
        Instruction(const Func func, const RegisterName rd,
                    const RegisterName rs, const RegisterName rt,
                    const uint8_t shift_amount = 0) {

            constexpr uint8_t MASK = (1 << 5) - 1;
            rtype.zero = 0;
            rtype.func = static_cast<uint8_t>(func);
            rtype.rd = static_cast<uint8_t>(rd);
            rtype.rs = static_cast<uint8_t>(rs);
            rtype.rt = static_cast<uint8_t>(rt);
            rtype.shamt = shift_amount & MASK;
        }

        // I-Type
        Instruction(const ITypeOpcode opcode, const RegisterName rt,
                    const RegisterName rs, const uint16_t immediate) {
            itype.op = static_cast<uint8_t>(opcode);
            itype.rt = static_cast<uint8_t>(rt);
            itype.rs = static_cast<uint8_t>(rs);
            itype.imm = immediate;
        }

        // I-Type that doesnt take rt as input
        Instruction(const ITypeOpcode opcode, const RegisterName rs,
                    const uint16_t immediate) {
            itype.op = static_cast<uint8_t>(opcode);
            itype.rt = 0;
            itype.rs = static_cast<uint8_t>(rs);
            itype.imm = immediate;
        }

        // J-Type
        Instruction(const JTypeOpcode opcode, const uint32_t address) {
            constexpr uint32_t MASK = (1 << 26) - 1;

            jtype.op = static_cast<uint8_t>(opcode);
            jtype.address = address & MASK;
        }

        // No need for FPU register naming since it's just $f[number]

        // FPU R-Type
        Instruction(const FPURTypeOp op, const uint8_t ft, const uint8_t fs,
                    const uint8_t fd, const FPUFunc func) {
            fpu_rtype.cop1 = static_cast<uint8_t>(COPOpcode::e_cop1);
            fpu_rtype.fmt = static_cast<uint8_t>(op);
            fpu_rtype.ft = ft & 31;
            fpu_rtype.fs = fs & 31;
            fpu_rtype.fd = fd & 31;
            fpu_rtype.func = static_cast<uint8_t>(func);
        }

        // FPU B-Type
        Instruction(const FPUBTypeOp op, const uint8_t ft, uint16_t offset) {
            fpu_btype.cop1 = static_cast<uint8_t>(COPOpcode::e_cop1);
            fpu_btype.bc = static_cast<uint8_t>(op);
            fpu_btype.ft = ft & 31;
            fpu_btype.offset = offset;
        }

        // FPU T-Type
        Instruction(const FPUTTypeOp op, const RegisterName rt,
                    const uint8_t fs) {
            fpu_ttype.cop1 = static_cast<uint8_t>(COPOpcode::e_cop1);
            fpu_ttype.op = static_cast<uint8_t>(op);
            fpu_ttype.rt = static_cast<uint8_t>(rt);
            fpu_ttype.fs = fs & 31;
            fpu_ttype.zero = 0;
        }

        // raw
        Instruction(const uint32_t value) { raw = value; }

        // Special3
        Instruction(const Special3Func func, const Special3BSHFLFunc op,
                    const RegisterName rd, const RegisterName rt) {
            special3_type.func = static_cast<uint8_t>(func);
            special3_type.extra = static_cast<uint8_t>(op);
            special3_type.rd = static_cast<uint8_t>(rd);
            special3_type.rt = static_cast<uint8_t>(rt);
            special3_type.rs = 0;
            special3_type.special3 = SPECIAL3_OPCODE;
        }

        Instruction(const Special3Func func, const uint8_t extra,
                    const uint8_t rd, const RegisterName rs,
                    const RegisterName rt) {
            special3_type.func = static_cast<uint8_t>(func);
            special3_type.extra = extra;
            special3_type.rd = rd;
            special3_type.rs = static_cast<uint8_t>(rs);
            special3_type.rt = static_cast<uint8_t>(rt);
            special3_type.special3 = SPECIAL3_OPCODE;
        }

        // Regimm I-Type
        Instruction(const RegimmITypeOp opcode, const RegisterName rs,
                    const uint16_t immediate) {
            regimm_itype.op = static_cast<uint8_t>(opcode);
            regimm_itype.rs = static_cast<uint8_t>(rs);
            regimm_itype.imm = immediate;
            regimm_itype.regimm = REGIMM_OPCODE;
        }

        // PC relative
        Instruction(const RegisterName rs, const PCRelFunc1 func,
                    const uint32_t immediate) {
            pcrel_type1.pcrel = PCREL_OPCODE;
            pcrel_type1.rs = static_cast<uint8_t>(rs);
            pcrel_type1.func = static_cast<uint8_t>(func);
            pcrel_type1.imm = immediate;
        }

        Instruction(const RegisterName rs, const PCRelFunc2 func,
                    const uint16_t immediate) {
            pcrel_type2.pcrel = PCREL_OPCODE;
            pcrel_type2.rs = static_cast<uint8_t>(rs);
            pcrel_type2.func = static_cast<uint8_t>(func);
            pcrel_type2.imm = immediate;
        }

        inline Result<Type, void> get_type() const {
            switch (general.op) {
                    // R-Type
                case RTYPE_OPCODE:
                    return Type::e_rtype;

                    // J-Type
                case 2:
                case 3:
                case 0b110010:
                case 0b111010:
                    return Type::e_jtype;

                    // I-Type
                case 4:
                case 6:
                case 5:
                case 7:
                case 8:
                case 9:
                case 15:
                case 10:
                case 11:
                case 12:
                case 13:
                case 14:
                case 32:
                case 33:
                case 36:
                case 37:
                case 35:
                case 40:
                case 41:
                case 43:
                case 24:
                case 22:
                case 23:
                    return Type::e_itype;

                    // REGIMM
                case REGIMM_OPCODE:
                    return Type::e_regimm_itype;

                    // Special3
                case SPECIAL3_OPCODE: {
                    switch (static_cast<Special3Func>(special3_type.func)) {
                        case Special3Func::e_bshfl:
                            return Type::e_special3_type_bshfl;
                        case Special3Func::e_ext:
                            return Type::e_special3_type_ext;
                        case Special3Func::e_ins:
                            return Type::e_special3_type_ins;
                    }
                    break;
                }

                    // PC relative
                case PCREL_OPCODE: {
                    if ((pcrel_type1.func & 0b10) == 0) {
                        return Type::e_pcrel_type1;
                    }
                    else {
                        return Type::e_pcrel_type2;
                    }
                }

                    // Coprocessor 1
                case 17: {
                    if (fpu_rtype.fmt & 0b10000) return Type::e_fpu_rtype;

                    if (fpu_rtype.fmt & 0b01000) return Type::e_fpu_btype;

                    return Type::e_fpu_ttype;
                }
                case 62:
                case 54:
                    if (itype.rs != 0) {
                        return Type::e_longimm_itype;
                    }
                    else {
                        return Type::e_itype;
                    }
            }

            return Result<Type, void>();
        }

        uint32_t raw = 0;
        General general;

        RType rtype;
        IType itype;
        JType jtype;

        FPURType fpu_rtype;
        FPUTType fpu_ttype;
        FPUBType fpu_btype;

        Special3Type special3_type;
        Special3TypeBSHFL special3_type_bshfl;
        Special3TypeEXT special3_type_ext;
        Special3TypeINS special3_type_ins;

        RegimmIType regimm_itype;
        LongimmIType longimm_itype;

        PCRelType1 pcrel_type1;
        PCRelType2 pcrel_type2;

    }; // namespace mips_emulator

    // Make sure Instruction union matches the MIPS instruction size
    static_assert(sizeof(Instruction) == 4,
                  "Instruction union is not 4 bytes in size");
} // namespace mips_emulator

// Undefine macro in order to not leak it out of the header
#undef PACKED
