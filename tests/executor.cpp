#include "mips-emulator/executor.hpp"
#include "mips-emulator/instruction.hpp"
#include "mips-emulator/register_file.hpp"
#include "mips-emulator/register_name.hpp"

#include <catch2/catch.hpp>

using namespace mips_emulator;

using Func = Instruction::Func;

TEMPLATE_TEST_CASE("add", "[Executor]", RegisterFile32, RegisterFile64) {
    SECTION("Positive numbers") {
        using TestExecutor = Executor<TestType>;
        using Address = typename TestExecutor::Address;
        TestType reg_file;
        Address pc = 0;

        reg_file.set_signed(RegisterName::e_t0, 1);
        reg_file.set_signed(RegisterName::e_t1, 5);

        Instruction instr(Func::e_add, RegisterName::e_t2, RegisterName::e_t0,
                          RegisterName::e_t1);

        TestExecutor::handle_rtype_instr(instr, pc, reg_file);

        REQUIRE(reg_file.get(RegisterName::e_t2).s == 6);
    }
}
