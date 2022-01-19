#pragma once
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
        PACKED(struct RType {
            // Func constants for R-Type instructions
            static constexpr uint8_t e_add = 32;
            static constexpr uint8_t e_addu = 33;
            static constexpr uint8_t e_sub = 34;
            static constexpr uint8_t e_subu = 35;
            static constexpr uint8_t e_mul = 24;
            static constexpr uint8_t e_mulu = 26;
            static constexpr uint8_t e_and = 36;
            static constexpr uint8_t e_nor = 39;
            static constexpr uint8_t e_or = 37;
            static constexpr uint8_t e_xor = 38;
            static constexpr uint8_t e_jr = 8;
            static constexpr uint8_t e_jalr = 9;
            static constexpr uint8_t e_slt = 42;
            static constexpr uint8_t e_sltu = 43;
            static constexpr uint8_t e_sll = 0;
            static constexpr uint8_t e_sllv = 4;
            static constexpr uint8_t e_sra = 3;
            static constexpr uint8_t e_srl = 2;
            static constexpr uint8_t e_srlv = 6;

            uint8_t op : 6;
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
            uint32_t imm : 26;
        });

        // Make sure R, I and J type instruction structs are the same size
        static_assert(sizeof(RType) == 4);
        static_assert(sizeof(IType) == 4);
        static_assert(sizeof(JType) == 4);

        // R-Type: 0
        // I-Type: 2
        // J-Type: Otherwise
        inline uint8_t get_type() const { return rtype.op & ~1; }

        uint32_t raw;

        RType rtype;
        IType itype;
        JType jtype;
    };

    // Make sure Instruction union matches the MIPS instruction size
    static_assert(sizeof(Instruction) == 4);
} // namespace mips_emulator

// Undefine macro in order to not leak it out of the header
#undef PACKED
