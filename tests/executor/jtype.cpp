#include "mips-emulator/executor.hpp"
#include "mips-emulator/instruction.hpp"
#include "mips-emulator/register_file.hpp"
#include "mips-emulator/register_name.hpp"

#include <catch2/catch.hpp>

using namespace mips_emulator;

using JOp = Instruction::JTypeOpcode;

TEST_CASE("j", "[Executor]") {
    SECTION("Positive numbers") {
        using Unsigned = typename RegisterFile::Unsigned;
        RegisterFile reg_file;
        reg_file.set_pc(0x10beef00);

        Instruction instr(JOp::e_j, 0x003fc);

        reg_file.inc_pc(); // emulate step
        const bool no_error = Executor::handle_jtype_instr(instr, reg_file);
        REQUIRE(no_error);

        reg_file.update_pc(); // moves past delays slot

        REQUIRE(reg_file.get_pc() == 0x10000ff0);
    }
}

TEST_CASE("jal", "[Executor]") {
    SECTION("Positive numbers") {
        using Unsigned = typename RegisterFile::Unsigned;
        RegisterFile reg_file;
        reg_file.set_pc(0x10beef00);

        Instruction instr(JOp::e_jal, 0x003fc);

        reg_file.update_pc(); // emulate step
        const bool no_error = Executor::handle_jtype_instr(instr, reg_file);
        REQUIRE(no_error);

        reg_file.update_pc(); // moves past delays slot

        REQUIRE(reg_file.get_pc() == 0x10000ff0);
        REQUIRE(reg_file.get(RegisterName::e_ra).u == 0x10beef04);
    }
}

TEST_CASE("bc", "[Executor]") {

    using Unsigned = typename RegisterFile::Unsigned;
    RegisterFile reg_file;
    reg_file.set_pc(0x10bef00);

    Instruction instr(JOp::e_bc, -(0xf00 >> 2));

    const bool no_error = Executor::handle_jtype_instr(instr, reg_file);
    REQUIRE(no_error);

    REQUIRE(reg_file.get_pc() == 0x10be000);
}

TEST_CASE("balc", "[Executor]") {

    using Unsigned = typename RegisterFile::Unsigned;
    RegisterFile reg_file;
    reg_file.set_pc(0x10bef00);

    Instruction instr(JOp::e_balc, -(0xf00 >> 2));

    const bool no_error = Executor::handle_jtype_instr(instr, reg_file);
    REQUIRE(no_error);

    REQUIRE(reg_file.get_pc() == 0x10be000);
    REQUIRE(reg_file.get(RegisterName::e_ra).u == 0x10bef00);
}
