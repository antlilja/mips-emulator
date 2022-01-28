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
