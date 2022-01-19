#include "mips-emulator/executor.hpp"
#include "mips-emulator/instruction.hpp"
#include "mips-emulator/register_state.hpp"

#include <catch2/catch.hpp>

using namespace mips_emulator;

using Func = Instruction::Func;
using Register = Instruction::Register;

TEMPLATE_TEST_CASE("add", "[Executor]", RegisterState32, RegisterState64) {
    SECTION("Positive numbers") {
        using TestExecutor = Executor<TestType>;
        using Address = typename TestExecutor::Address;
        TestType state;
        Address pc = 0;

        state.set_register_signed(8, 1);
        state.set_register_signed(9, 5);

        Instruction instr(Func::e_add, Register::e_t2, Register::e_t0,
                          Register::e_t1);

        TestExecutor::handle_rtype_instr(instr, pc, state);

        REQUIRE(state.get_register(static_cast<uint8_t>(Register::e_t2)).s ==
                6);
    }
}
