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
    SECTION("Positive numbers") {
        reg_file.set_pc(0x10beef00);

        Instruction instr(RegisterName::e_t0, Func1::e_addiupc, 0x01234);

        const bool no_error = 
            Executor::handle_pcrel_type1_instr(instr, reg_file, memory);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == 0x10bf37d0);
    }
}

// TEST_CASE("lwpc", "[Executor]") {
//     Result v = memory.template store<uint32_t>(4, (uint32_t)0x0320);
//     REQUIRE(!v.is_error());

//     SECTION("Positive numbers") {
//         reg_file.set_pc(0x10beef00);

//         Instruction instr(RegisterName::e_t0, Func1::e_lwpc, 0x0123);

//         const bool no_error = 
//             Executor::handle_pcrel_type1_instr(instr, reg_file, memory);
//         REQUIRE(no_error);

//         REQUIRE(reg_file.get(RegisterName::e_t0).u == 0x80222280);
//     }
// }


// pcrel Type 2 tests
TEST_CASE("auipc", "[Executor]") {
    SECTION("Positive numbers") {
        reg_file.set_pc(0x10beef00);

        Instruction instr(RegisterName::e_t0, Func2::e_auipc, 0x0123);

        const bool no_error = Executor::handle_pcrel_type2_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == 0x11e1ef00);
    }
}

TEST_CASE("aluipc", "[Executor]") {
    SECTION("Positive numbers") {
        reg_file.set_pc(0x10beef00);

        Instruction instr(RegisterName::e_t0, Func2::e_aluipc, 0x0123);

        const bool no_error = Executor::handle_pcrel_type2_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == 0x11e10000);
    }
}