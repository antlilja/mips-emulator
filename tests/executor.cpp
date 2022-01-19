#include "mips-emulator/executor.hpp"
#include "mips-emulator/register_state.hpp"

#include <catch2/catch.hpp>

using namespace mips_emulator;

TEMPLATE_TEST_CASE("add", "[Executor]", RegisterState32, RegisterState64) {
    SECTION("Positive numbers") {
        using TestExecutor = Executor<TestType>;
        using Address = typename TestExecutor::Address;
        TestType state;
        Address pc = 0;

        state.set_register_signed(8, 1);
        state.set_register_signed(9, 5);

        // TODO: Create a helper function to do this
        Instruction instr;
        instr.raw = 0;
        instr.rtype.func = 32;
        instr.rtype.rs = 8;
        instr.rtype.rt = 9;
        instr.rtype.rd = 10;

        TestExecutor::handle_rtype_instr(instr, pc, state);

        REQUIRE(state.get_register(10).s == 6);
    }
}
