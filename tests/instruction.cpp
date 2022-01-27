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
