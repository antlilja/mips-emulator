#include "mips-emulator/executor.hpp"
#include "mips-emulator/instruction.hpp"
#include "mips-emulator/register_file.hpp"
#include "mips-emulator/register_name.hpp"
#include "mips-emulator/static_memory.hpp"

#include <catch2/catch.hpp>

using namespace mips_emulator;

using Func = Instruction::Func;
using IOp = Instruction::ITypeOpcode;
using JOp = Instruction::JTypeOpcode;

TEST_CASE("step", "[Executor]") {
    StaticMemory<256> memory;
    RegisterFile reg_file;

    reg_file.set_unsigned(RegisterName::e_t0, 5);
    reg_file.set_unsigned(RegisterName::e_t1, 2);

    reg_file.set_pc(0);

    SECTION("step rtype") {

        Instruction instr(Func::e_add, RegisterName::e_t2, RegisterName::e_t0,
                          RegisterName::e_t1);

        auto store_result = memory.template store<uint32_t>(0, instr.raw);
        REQUIRE_FALSE(store_result.is_error());

        const bool no_error =
            Executor::step<StaticMemory<256>>(reg_file, memory);

        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t2).s == 7);
        REQUIRE(reg_file.get_pc() == 4);
    }

    SECTION("step itype") {

        Instruction instr(IOp::e_ori, RegisterName::e_t2, RegisterName::e_t1,
                          1);

        auto store_result = memory.template store<uint32_t>(0, instr.raw);
        REQUIRE_FALSE(store_result.is_error());

        const bool no_error =
            Executor::step<StaticMemory<256>>(reg_file, memory);

        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t2).u == 3);
        REQUIRE(reg_file.get_pc() == 4);
    }
}

TEST_CASE("delay slot", "[Executor]") {
    StaticMemory<256> memory;
    RegisterFile reg_file;

    reg_file.set_unsigned(RegisterName::e_t0, 0);

    reg_file.set_pc(0);

    SECTION("step rtype") {

        Instruction instr_jump(JOp::e_j, 0x5);

        Instruction instr_delay(IOp::e_addi, RegisterName::e_t0,
                                RegisterName::e_0, 777);

        memory.template store<uint32_t>(0, instr_jump.raw);
        memory.template store<uint32_t>(4, instr_delay.raw);

        const bool no_error1 =
            Executor::step<StaticMemory<256>>(reg_file, memory);
        REQUIRE(no_error1);
        REQUIRE(reg_file.get_pc() == 4);

        const bool no_error2 =
            Executor::step<StaticMemory<256>>(reg_file, memory);
        REQUIRE(no_error2);

        REQUIRE(reg_file.get_pc() == 0x14);
        REQUIRE(reg_file.get(RegisterName::e_t0).u == 777);
    }
}
