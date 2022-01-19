#include "mips-emulator/executor.hpp"
#include "mips-emulator/instruction.hpp"
#include "mips-emulator/register_file.hpp"

#include <catch2/catch.hpp>

using namespace mips_emulator;

using Func = Instruction::Func;
using Register = Instruction::Register;

TEMPLATE_TEST_CASE("add", "[Executor]", RegisterFile32, RegisterFile64) {
    SECTION("Positive numbers") {
        using TestExecutor = Executor<TestType>;
        using Address = typename TestExecutor::Address;
        TestType reg_file;
        Address pc = 0;

        reg_file.set_register_signed(8, 1);
        reg_file.set_register_signed(9, 5);

        Instruction instr(Func::e_add, Register::e_t2, Register::e_t0,
                          Register::e_t1);

        TestExecutor::handle_rtype_instr(instr, pc, reg_file);

        REQUIRE(reg_file.get_register(static_cast<uint8_t>(Register::e_t2)).s ==
                6);
    }
}
