#include "mips-emulator/executor.hpp"
#include "mips-emulator/instruction.hpp"
#include "mips-emulator/register_file.hpp"
#include "mips-emulator/register_name.hpp"
#include "mips-emulator/static_memory.hpp"

#include <catch2/catch.hpp>

using namespace mips_emulator;

using Func1 = Instruction::PCRelFunc1;
using Func2 = Instruction::PCRelFunc2;

using Unsigned = typename RegisterFile::Unsigned;

RegisterFile reg_file;
StaticMemory<256> memory;

// pcrel Type 1 tests
TEST_CASE("addiupc", "[Executor]") {
    reg_file.set_pc(0x10beef00);

    SECTION("Trivial imm-value") {
        Instruction instr(RegisterName::e_t0, Func1::e_addiupc, 0x01234);

        const bool no_error = 
            Executor::handle_pcrel_type1_instr(instr, reg_file, memory);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == 0x10bf37d0);
    }

    SECTION("Min imm-value") {
        Instruction instr(RegisterName::e_t0, Func1::e_addiupc, -INT32_MAX);

        const bool no_error = 
            Executor::handle_pcrel_type1_instr(instr, reg_file, memory);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == 0x10beef04);
    }

    SECTION("Imm-value 0") {
        Instruction instr(RegisterName::e_t0, Func1::e_addiupc, 0);

        const bool no_error = 
            Executor::handle_pcrel_type1_instr(instr, reg_file, memory);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == 0x10beef00);
    }

    SECTION("Max imm-value") {
        Instruction instr(RegisterName::e_t0, Func1::e_addiupc, INT32_MAX);

        const bool no_error = 
            Executor::handle_pcrel_type1_instr(instr, reg_file, memory);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == 0x10deeefc);
    }
}

// TEST_CASE("lwpc", "[Executor]") {
//     reg_file.set_pc(0x10beef00);

//     Result v = memory.template store<uint32_t>(4, (uint32_t)0x80222280);
//     REQUIRE(!v.is_error());

//     SECTION("Trivial imm-value") {
//         Instruction instr(RegisterName::e_t0, Func1::e_lwpc, 0x0123);

//         const bool no_error = 
//             Executor::handle_pcrel_type1_instr(instr, reg_file, memory);
//         REQUIRE(no_error);

//         REQUIRE(reg_file.get(RegisterName::e_t0).u == 0);
//     }
// }


// pcrel Type 2 tests
TEST_CASE("auipc", "[Executor]") {
    reg_file.set_pc(0x10beef00);

    SECTION("Trivial imm-value") {
        Instruction instr(RegisterName::e_t0, Func2::e_auipc, 0x0123);

        const bool no_error = Executor::handle_pcrel_type2_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == 0x11e1ef00);
    }
    
    SECTION("Min imm-value") {
        Instruction instr(RegisterName::e_t0, Func2::e_auipc, (uint16_t)(-INT32_MAX)); // INT32_MIN apparently does not want to cooperate

        const bool no_error = Executor::handle_pcrel_type2_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == 0x10bfef00);
    }

    SECTION("Imm-value zero") {
        Instruction instr(RegisterName::e_t0, Func2::e_auipc, 0);

        const bool no_error = Executor::handle_pcrel_type2_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == 0x10beef00);
    }

    SECTION("Max imm-value") {
        Instruction instr(RegisterName::e_t0, Func2::e_auipc, (uint16_t)(INT32_MAX));

        const bool no_error = Executor::handle_pcrel_type2_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == 0x10bdef00);
    }
}

TEST_CASE("aluipc", "[Executor]") {
    reg_file.set_pc(0x10beef00);

    SECTION("Trivial imm-value") {
        Instruction instr(RegisterName::e_t0, Func2::e_aluipc, 0x0123);

        const bool no_error = Executor::handle_pcrel_type2_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == 0x11e10000);
    }

    SECTION("Min imm-value") {
        Instruction instr(RegisterName::e_t0, Func2::e_aluipc, (uint16_t)(-INT32_MAX));

        const bool no_error = Executor::handle_pcrel_type2_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == 0x10bf0000);
    }

    SECTION("Imm-value zero") {
        Instruction instr(RegisterName::e_t0, Func2::e_aluipc, 0);

        const bool no_error = Executor::handle_pcrel_type2_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == 0x10be0000);
    }

    SECTION("Max imm-value") {
        Instruction instr(RegisterName::e_t0, Func2::e_aluipc, (uint16_t)(INT32_MAX));

        const bool no_error = Executor::handle_pcrel_type2_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == 0x10bd0000);
    }
}