#include "mips-emulator/executor.hpp"
#include "mips-emulator/instruction.hpp"
#include "mips-emulator/register_file.hpp"
#include "mips-emulator/register_name.hpp"

#include <catch2/catch.hpp>

using namespace mips_emulator;

using Func = Instruction::Func;

TEMPLATE_TEST_CASE("add", "[Executor]", RegisterFile32, RegisterFile64) {
    SECTION("Positive numbers") {
        using Address = typename TestType::Unsigned;
        TestType reg_file;
        Address pc = 0;

        reg_file.set_signed(RegisterName::e_t0, 1);
        reg_file.set_signed(RegisterName::e_t1, 5);

        Instruction instr(Func::e_add, RegisterName::e_t2, RegisterName::e_t0,
                          RegisterName::e_t1);

        const bool no_error = Executor::handle_rtype_instr(instr, pc, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t2).s == 6);
    }
}

TEMPLATE_TEST_CASE("or", "[Executor]", RegisterFile32, RegisterFile64) {
	SECTION("Positive numbers") {
		using Address = typename TestType::Unsigned;
		TestType reg_file;
		Address pc = 0;

		reg_file.set_unsigned(RegisterName::e_t0, 0b1); // 1
		reg_file.set_unsigned(RegisterName::e_t1, 0b110); // 6

		Instruction instr(Func::e_or, RegisterName::e_t2, RegisterName::e_t0,
				RegisterName::e_t1);

		const bool no_error = Executor::handle_rtype_instr(instr, pc, reg_file);
		REQUIRE(no_error);

		REQUIRE(reg_file.get(RegisterName::e_t2).u == 0b111); // 7
	}
}
