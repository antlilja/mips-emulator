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
        enum class Func : uint8_t {
            e_add = 32,
            e_addu = 33,
            e_sub = 34,
            e_subu = 35,
            e_mul = 24,
            e_mulu = 26,
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
            e_srl = 2,
            e_srlv = 6,
        };

        enum class ITypeOpcode : uint8_t {
            e_beq = 4,
            e_bne = 5,
            e_addi = 8,
            e_addiu = 9,
            e_slti = 10,
            e_sltiu = 11,
            e_andi = 12,
            e_ori = 13,
            e_xori = 14,
            e_lb = 32,
            e_lbu = 36,
            e_lui = 15,
            e_lw = 35,
            e_sb = 40,
            e_sw = 43,
        };

        enum class JTypeOpcode : uint8_t {
            e_j = 2,
            e_jal = 3,
        };

        PACKED(struct General {
            uint8_t op : 6;
            uint32_t reserved : 26;
        });

        PACKED(struct RType {
            uint8_t zero : 6;
            uint8_t rs : 5;
            uint8_t rt : 5;
            uint8_t rd : 5;
            uint8_t shamt : 5;
            uint8_t func : 6;
        });

        PACKED(struct IType {
            uint8_t op : 6;
            uint8_t rs : 5;
            uint8_t rt : 5;
            uint16_t imm : 16;
        });

        PACKED(struct JType {
            uint8_t op : 6;
            uint32_t address : 26;
        });

        // Make sure internal structs are the same size
        static_assert(sizeof(General) == 4);
        static_assert(sizeof(RType) == 4);
        static_assert(sizeof(IType) == 4);
        static_assert(sizeof(JType) == 4);

        // I-Type
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
        Instruction(const ITypeOpcode opcode, const RegisterName rs,
                    const RegisterName rt, const uint16_t immediate) {
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

        // R-Type: 0
        // I-Type: 2
        // J-Type: Otherwise
        inline uint8_t get_type() const { return general.op & ~1; }

        uint32_t raw = 0;
        General general;

        RType rtype;
        IType itype;
        JType jtype;
    };

    // Make sure Instruction union matches the MIPS instruction size
    static_assert(sizeof(Instruction) == 4);
} // namespace mips_emulator

// Undefine macro in order to not leak it out of the header
#undef PACKED
