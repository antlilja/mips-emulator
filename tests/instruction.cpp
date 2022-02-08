#include "mips-emulator/instruction.hpp"
#include "mips-emulator/register_name.hpp"

#include <catch2/catch.hpp>

using namespace mips_emulator;

using Func = Instruction::Func;
using IOp = Instruction::ITypeOpcode;
using JOp = Instruction::JTypeOpcode;

TEST_CASE("R-Type instruction", "[Instruction]") {
    SECTION("add - zero registers") {
        const Instruction instr(Func::e_add, RegisterName::e_0,
                                RegisterName::e_0, RegisterName::e_0);

        REQUIRE(instr.raw == 0x20);
    }

    SECTION("add - non zero registers") {
        const Instruction instr(Func::e_add, RegisterName::e_t0,
                                RegisterName::e_t5, RegisterName::e_a0);

        REQUIRE(instr.raw == 0x01a44020);
    }

    SECTION("sll") {
        const Instruction instr(Func::e_sll, RegisterName::e_t0, RegisterName::e_0, RegisterName::e_t1, 4);

        REQUIRE(instr.raw == 0x00094100);
    }

    SECTION("sllv") {
        const Instruction instr(Func::e_sllv, RegisterName::e_t0, RegisterName::e_t2, RegisterName::e_t1);

        REQUIRE(instr.raw == 0x01494004);
    }

    SECTION("sra") {
        const Instruction instr(Func::e_sra, RegisterName::e_t0, RegisterName::e_0, RegisterName::e_t1, 4);

        REQUIRE(instr.raw == 0x00094103);
    }

    SECTION("srav") {
        const Instruction instr(Func::e_srav, RegisterName::e_t0, RegisterName::e_t2, RegisterName::e_t1);

        REQUIRE(instr.raw == 0x01494007);
    }

    SECTION("srl") {
        const Instruction instr(Func::e_srl, RegisterName::e_t0, RegisterName::e_0, RegisterName::e_t1, 4);

        REQUIRE(instr.raw == 0x00094102);
    }

    SECTION("srlv") {
        const Instruction instr(Func::e_srlv, RegisterName::e_t0, RegisterName::e_t2, RegisterName::e_t1);

        REQUIRE(instr.raw == 0x01494006);
    }
}

TEST_CASE("I-Type instruction", "[Instruction]") {
    SECTION("addi - zero registers and zero imm") {
        const Instruction instr(IOp::e_addi, RegisterName::e_0,
                                RegisterName::e_0, 0);

        REQUIRE(instr.raw == 0x20000000);
    }

    SECTION("addi - non zero registers and imm") {
        const Instruction instr(IOp::e_addi, RegisterName::e_t0,
                                RegisterName::e_t5, 0xffff);

        REQUIRE(instr.raw == 0x21a8ffff);
    }
}

// FPU Instruction Tests
TEST_CASE("FPU R Type", "[Instruction]") {
    using Type = Instruction::Type;
    using R = Instruction::FPURTypeOp;
    using Func = Instruction::FPUFunc;

    SECTION("get_type") {
        R instr[] = {R::e_fmt_s, R::e_fmt_d, R::e_fmt_w, R::e_fmt_l, R::e_cmp_condn_s, R::e_cmp_condn_d};
        for(auto const v: instr) {
            const auto inst = Instruction(v, 0, 2, 3, Func::e_floor_l);
            REQUIRE(inst.get_type() == Type::e_fpu_rtype);
        }
    }

    SECTION("ceil.w.d $f1, $f2") {
        Instruction r(R::e_fmt_d, 0, 2, 1, Func::e_ceil_w);
        REQUIRE(r.raw == 0x4620104eU);
    }

    SECTION("abs.s $f24, $f26") {
        Instruction r(R::e_fmt_s, 0, 26, 24, Func::e_abs);
        REQUIRE(r.raw == 0x4600d605U);
    }
    
    SECTION("add.d $f2, $f1, $f20") {
        Instruction r(R::e_fmt_d, 20, 1, 2, Func::e_add);
        REQUIRE(r.raw == 0x46340880);
    }
}

TEST_CASE("FPU T Type", "[Instruction]") {
    using Type = Instruction::Type;
    using T = Instruction::FPUTTypeOp;

    SECTION("get_type") {
        T instr[] = {T::e_cf, T::e_ct, T::e_mf, T::e_mfh, T::e_mt, T::e_mth};
        for(auto const v: instr) {
            const auto inst = Instruction(v, RegisterName::e_k0, 0);
            REQUIRE(inst.get_type() == Type::e_fpu_ttype);
        }
    }

    SECTION("mfc1 $t1, $f8") {
        Instruction t(T::e_mf, RegisterName::e_t1, 8);
        REQUIRE(t.raw == 0x44094000U);
    }

    SECTION("mtc1 $t0, $f31") {
        Instruction t(T::e_mt, RegisterName::e_t0, 31);
        REQUIRE(t.raw == 0x4488f800U);
    }
}

TEST_CASE("FPU B Type", "[Instruction]") {
    using Type = Instruction::Type;
    using B = Instruction::FPUBTypeOp;

    SECTION("get_type") {
        B instr[] = {B::e_bc1eqz, B::e_bc1nez};
        for(auto const v: instr) {
            const auto inst = Instruction(v, 31, 25);
            REQUIRE(inst.get_type() == Type::e_fpu_btype);
        }
    }
}
