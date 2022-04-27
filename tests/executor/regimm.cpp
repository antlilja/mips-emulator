#include "mips-emulator/executor.hpp"
#include "mips-emulator/instruction.hpp"
#include "mips-emulator/register_file.hpp"
#include "mips-emulator/register_name.hpp"

#include <catch2/catch.hpp>

using namespace mips_emulator;

using IOp = Instruction::RegimmITypeOp;

typedef struct {
    uint8_t rs;
    uint32_t rs_val;
    bool will_branch;
} BranchTest;

// Test with delay slot
void test_branch(const BranchTest tcase, IOp op) {
    RegisterFile reg_file;
    reg_file.set_unsigned(tcase.rs, tcase.rs_val);
    Instruction instr(op, static_cast<RegisterName>(tcase.rs), 0xFFF0);

    reg_file.inc_pc(); // Emulate step
    const bool no_error = Executor::handle_regimm_itype_instr(instr, reg_file);
    REQUIRE(no_error);

    REQUIRE(reg_file.get_pc() == 4);
    reg_file.update_pc(); // moves past delays slot

    const uint32_t expected_pc = tcase.will_branch ? 4 - 16 * 4 : 8;
    REQUIRE(reg_file.get_pc() == expected_pc);
};

// Branch on Less Than Zero
TEST_CASE("bltz", "[Executor]") {
    const auto test = [](const BranchTest& tcase) {
        test_branch(tcase, IOp::e_bltz);
    };

    SECTION("Equal to 0 (Don't branch)") {
        test({10, 0, false});
    }

    SECTION("Less than 0 (Branch)") {
        test({10, (uint32_t)(-1), true});
        test({10, (uint32_t)(INT32_MIN), true});
        test({10, (uint32_t)(-0xF0000), true});
    }

    SECTION("Positive (Don't branch)") {
        test({10, INT32_MAX, false});
        test({10, 12000, false});
    }
}

// Branch on Greater Than or Equal to Zero
TEST_CASE("bgez", "[Executor]") {
    const auto test = [](const BranchTest& tcase) {
        test_branch(tcase, IOp::e_bgez);
    };

    SECTION("Equal to 0 (Branch)") {
        test({10, 0, true});
    }

    SECTION("Less than 0 (Dont Branch)") {
        test({10, (uint32_t)(-1), false});
        test({10, (uint32_t)(INT32_MIN), false});
        test({10, (uint32_t)(-0xF0000), false});
    }

    SECTION("Positive (Branch)") {
        test({10, INT32_MAX, true});
        test({10, 12000, true});
    }
}