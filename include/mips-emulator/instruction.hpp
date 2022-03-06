#pragma once
#include "mips-emulator/register_name.hpp"

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
            e_special3_rtype,
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
            e_addi = 8,
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
        };

        enum class JTypeOpcode : uint8_t {
            e_j = 2,
            e_jal = 3,
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

        // Opcode for special3 instruction
        enum class Special3Opcode : uint8_t {
            e_special3 = 0b011111,
        };

        // Func enum for special3 instructions
        enum class Special3Func : uint8_t { e_bshfl = 0b100000 };

        // Opcode enum for special3 r-type like instructions
        // Calling them R-type due to lack of better name
        // Special3 instructions have a bunch of different layouts depending on
        // the func field
        enum class Special3RTypeOp : uint8_t {
            e_bitswap = 0,
            e_wsbh = 0b00010,
            e_seh = 0b11000,
            e_seb = 0b10000,
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
        // Special3RType, similar to R type instruction
        // Calling it R-type due to lack of better name
        // Special3 instructions have a bunch of different layouts depending on
        // the func field
        PACKED(struct Special3RType {
            uint32_t func : 6;
            uint32_t op : 5;
            uint32_t rd : 5;
            uint32_t rt : 5;
            uint32_t zero : 5;
            uint32_t special3 : 6;
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
            sizeof(Special3RType) == 4,
            "Instruction::Special3RType bitfield is not 4 bytes in size");

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

        // Special3 R-Type
        Instruction(const Special3Func func, const Special3RTypeOp op,
                    const RegisterName rd, const RegisterName rt) {
            special3_rtype.func = static_cast<uint8_t>(func);
            special3_rtype.op = static_cast<uint8_t>(op);
            special3_rtype.rd = static_cast<uint8_t>(rd);
            special3_rtype.rt = static_cast<uint8_t>(rt);
            special3_rtype.zero = 0;
            special3_rtype.special3 =
                static_cast<uint8_t>(Special3Opcode::e_special3);
        }

        inline Type get_type() const {
            if (general.op == static_cast<uint8_t>(COPOpcode::e_cop1)) {
                if (fpu_rtype.fmt & 0b10000) return Type::e_fpu_rtype;

                if (fpu_rtype.fmt & 0b01000) return Type::e_fpu_btype;

                return Type::e_fpu_ttype;
            }
            else if (general.op ==
                     static_cast<uint8_t>(Special3Opcode::e_special3)) {
                // TODO, handle other types
                return Type::e_special3_rtype;
            }

            switch (general.op & ~1) {
                case 0: return Type::e_rtype;
                case 2: return Type::e_jtype;
            }
            return Type::e_itype;
        }

        uint32_t raw = 0;
        General general;

        RType rtype;
        IType itype;
        JType jtype;

        FPURType fpu_rtype;
        FPUTType fpu_ttype;
        FPUBType fpu_btype;

        Special3RType special3_rtype;
    }; // namespace mips_emulator

    // Make sure Instruction union matches the MIPS instruction size
    static_assert(sizeof(Instruction) == 4,
                  "Instruction union is not 4 bytes in size");
} // namespace mips_emulator

// Undefine macro in order to not leak it out of the header
#undef PACKED
