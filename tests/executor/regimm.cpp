#include "mips-emulator/executor.hpp"
#include "mips-emulator/instruction.hpp"
#include "mips-emulator/register_file.hpp"
#include "mips-emulator/register_name.hpp"
#include "mips-emulator/static_memory.hpp"

#include <catch2/catch.hpp>

using namespace mips_emulator;

using IOp = Instruction::RegimmITypeOp;

// Branch on Less Than Zero
TEST_CASE("bltz", "[Executor]") {
    SECTION("Equal to 0 (Don't branch)") {
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, 0);

        Instruction instr(IOp::e_bltz, RegisterName::e_t0, 16);

        reg_file.inc_pc(); // Emulate step
        const bool no_error =
            Executor::handle_regimm_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get_pc() == 4);

        reg_file.update_pc(); // moves past delays slot

        REQUIRE(reg_file.get_pc() == 8);
    }

    SECTION("Less than 0 (Branch)") {
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, -1);

        Instruction instr(IOp::e_bltz, RegisterName::e_t0, 16);

        reg_file.inc_pc(); // Emulate step
        const bool no_error =
            Executor::handle_regimm_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get_pc() == 4);

        reg_file.update_pc(); // moves past delays slot

        REQUIRE(reg_file.get_pc() == 68);
    }

    SECTION("Largest positive immediate (Don't branch)") {
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, 32767);

        Instruction instr(IOp::e_bltz, RegisterName::e_t0, 16);

        reg_file.inc_pc(); // Emulate step
        const bool no_error =
            Executor::handle_regimm_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get_pc() == 4);

        reg_file.update_pc(); // moves past delays slot

        REQUIRE(reg_file.get_pc() == 8);
    }

    SECTION("Smallest negative immediate (Branch)") {
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, -32768);

        Instruction instr(IOp::e_bltz, RegisterName::e_t0, 16);

        reg_file.inc_pc(); // Emulate step
        const bool no_error =
            Executor::handle_regimm_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get_pc() == 4);

        reg_file.update_pc(); // moves past delays slot

        REQUIRE(reg_file.get_pc() == 68);
    }
}

// Branch on Greater Than or Equal to Zero
TEST_CASE("bgez", "[Executor]") {
    SECTION("Equal to 0 (Branch)") {
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, 0);

        Instruction instr(IOp::e_bgez, RegisterName::e_t0, 16);

        reg_file.inc_pc(); // Emulate step
        const bool no_error =
            Executor::handle_regimm_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get_pc() == 4);

        reg_file.update_pc(); // moves past delays slot

        REQUIRE(reg_file.get_pc() == 68);
    }

    SECTION("Less than 0 (Don't branch)") {
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, -1);

        Instruction instr(IOp::e_bgez, RegisterName::e_t0, 16);

        reg_file.inc_pc(); // Emulate step
        const bool no_error =
            Executor::handle_regimm_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get_pc() == 4);

        reg_file.update_pc(); // moves past delays slot

        REQUIRE(reg_file.get_pc() == 8);
    }

    SECTION("Largest positive immediate (Branch)") {
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, 32767);

        Instruction instr(IOp::e_bgez, RegisterName::e_t0, 16);

        reg_file.inc_pc(); // Emulate step
        const bool no_error =
            Executor::handle_regimm_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get_pc() == 4);

        reg_file.update_pc(); // moves past delays slot

        REQUIRE(reg_file.get_pc() == 68);
    }

    SECTION("Smallest negative immediate (Don't branch)") {
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, -32768);

        Instruction instr(IOp::e_bgez, RegisterName::e_t0, 16);

        reg_file.inc_pc(); // Emulate step
        const bool no_error =
            Executor::handle_regimm_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get_pc() == 4);

        reg_file.update_pc(); // moves past delays slot

        REQUIRE(reg_file.get_pc() == 8);
    }
}