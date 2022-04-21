#include "mips-emulator/executor.hpp"
#include "mips-emulator/instruction.hpp"
#include "mips-emulator/register_file.hpp"
#include "mips-emulator/register_name.hpp"
#include "mips-emulator/static_memory.hpp"

#include <catch2/catch.hpp>

using namespace mips_emulator;

using IOp = Instruction::ITypeOpcode;

typedef struct {
    uint8_t rs, rt;
    uint32_t rs_val, rt_val;
    bool will_branch, will_link;
} BranchTest;

// Test imm branch without delay slot
void test_branch_compact(const BranchTest tcase, IOp op) {
    RegisterFile reg_file;
    const uint32_t start_pc = 0x1000;
    reg_file.set_unsigned(31, 0); // $ra
    reg_file.set_unsigned(tcase.rs, tcase.rs_val);
    reg_file.set_unsigned(tcase.rt, tcase.rt_val);
    reg_file.set_pc(start_pc);

    reg_file.inc_pc();
    Instruction instr(op, static_cast<RegisterName>(tcase.rt),
                      static_cast<RegisterName>(tcase.rs), 0xFFB0);

    const bool no_error = Executor::handle_itype_instr(instr, reg_file);
    REQUIRE(no_error);

    REQUIRE(reg_file.get_pc() == start_pc - 0x50 * tcase.will_branch * 4 + 4);
    REQUIRE(reg_file.get(31).u == (start_pc + 4) * tcase.will_link);
};

// Test with delay slot
void test_branch(const BranchTest tcase, IOp op) {
    RegisterFile reg_file;
    reg_file.set_unsigned(31, 0); // $ra
    reg_file.set_unsigned(tcase.rs, tcase.rs_val);
    reg_file.set_unsigned(tcase.rt, tcase.rt_val);

    Instruction instr(op, static_cast<RegisterName>(tcase.rs), 16);

    reg_file.inc_pc(); // Emulate step
    const bool no_error = Executor::handle_itype_instr(instr, reg_file);
    REQUIRE(no_error);

    REQUIRE(reg_file.get_pc() == 4);
    reg_file.update_pc(); // moves past delays slot

    const uint32_t expected_pc = tcase.will_branch ? 4 + 16 * 4 : 8;
    REQUIRE(reg_file.get_pc() == expected_pc);
};

TEST_CASE("addiu", "[Executor]") {
    // "Simple" might be a better name, but let's stick with convention
    SECTION("Positive numbers") {
        using Address = typename RegisterFile::Unsigned;
        RegisterFile reg_file;
        reg_file.set_unsigned(RegisterName::e_t0, 123456);

        Instruction instr(IOp::e_addiu, RegisterName::e_t1, RegisterName::e_t0,
                          333);

        const bool no_error = Executor::handle_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t1).u == 123789);
    }
    // No overflow-errors ever - it's 32-bit modulo
    SECTION("No overflow error") {
        using Address = typename RegisterFile::Unsigned;
        RegisterFile reg_file;
        reg_file.set_unsigned(RegisterName::e_t0, UINT32_MAX - 1);

        Instruction instr(IOp::e_addiu, RegisterName::e_t1, RegisterName::e_t0,
                          15);

        const bool no_error = Executor::handle_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t1).u == 13);
    }
}

TEST_CASE("andi", "[Executor]") {
    SECTION("Positive numbers") {
        using Address = typename RegisterFile::Unsigned;
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, 0b1100);

        Instruction instr(IOp::e_andi, RegisterName::e_t1, RegisterName::e_t0,
                          0b1010);

        const bool no_error = Executor::handle_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t1).u == 0b1000);
    }
}

TEST_CASE("ori", "[Executor]") {
    SECTION("Positive numbers") {
        using Address = typename RegisterFile::Unsigned;
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, 0b1100);

        Instruction instr(IOp::e_ori, RegisterName::e_t1, RegisterName::e_t0,
                          0b1010);

        const bool no_error = Executor::handle_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t1).u == 0b1110);
    }
}

TEST_CASE("xori", "[Executor]") {
    SECTION("Positive numbers") {
        using Address = typename RegisterFile::Unsigned;
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, 0b1100);

        Instruction instr(IOp::e_xori, RegisterName::e_t1, RegisterName::e_t0,
                          0b1010);

        const bool no_error = Executor::handle_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t1).u == 0b0110);
    }
}

TEST_CASE("aui", "[Executor]") {
    SECTION("Positive numbers") {
        using Address = typename RegisterFile::Unsigned;
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, 0x1337);

        Instruction instr(IOp::e_aui, RegisterName::e_t1, RegisterName::e_t0,
                          0xbeef);

        const bool no_error = Executor::handle_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t1).u == 0xbeef1337);
    }
}

// Set less than instructions
TEST_CASE("slti", "[Executor]") {
    SECTION("Positive numbers") {
        using Address = typename RegisterFile::Unsigned;
        RegisterFile reg_file;

        reg_file.set_signed(RegisterName::e_t0, -5);

        Instruction instr(IOp::e_slti, RegisterName::e_t1, RegisterName::e_t0,
                          2);

        const bool no_error = Executor::handle_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t1).u == 1);
    }
}

TEST_CASE("sltiu", "[Executor]") {
    SECTION("Positive numbers") {
        using Address = typename RegisterFile::Unsigned;
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, -5);

        Instruction instr(IOp::e_sltiu, RegisterName::e_t1, RegisterName::e_t0,
                          2);

        const bool no_error = Executor::handle_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t1).u == 0);
    }
}

// Branch instructions
TEST_CASE("beq", "[Executor]") {
    SECTION("Positive numbers (Branch)") {
        using Address = typename RegisterFile::Unsigned;
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, 7);
        reg_file.set_unsigned(RegisterName::e_t1, 7);
        Instruction instr(IOp::e_beq, RegisterName::e_t0, RegisterName::e_t1,
                          16);

        reg_file.inc_pc(); // emulate step
        const bool no_error = Executor::handle_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get_pc() == 4);

        reg_file.update_pc(); // moves past delays slot

        REQUIRE(reg_file.get_pc() == 68);
    }

    SECTION("Positive numbers (Don't Branch)") {
        using Address = typename RegisterFile::Unsigned;
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, 7);
        reg_file.set_unsigned(RegisterName::e_t1, 9);
        Instruction instr(IOp::e_beq, RegisterName::e_t0, RegisterName::e_t1,
                          16);

        reg_file.inc_pc(); // emulate step
        const bool no_error = Executor::handle_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get_pc() == 4);

        reg_file.update_pc(); // moves past delays slot

        REQUIRE(reg_file.get_pc() == 8);
    }
}

TEST_CASE("bne", "[Executor]") {
    SECTION("Positive numbers (Branch)") {
        using Address = typename RegisterFile::Unsigned;
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, 7);
        reg_file.set_unsigned(RegisterName::e_t1, 9);

        Instruction instr(IOp::e_bne, RegisterName::e_t0, RegisterName::e_t1,
                          16);

        reg_file.inc_pc(); // Emulate step
        const bool no_error = Executor::handle_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get_pc() == 4);

        reg_file.update_pc(); // moves past delays slot

        REQUIRE(reg_file.get_pc() == 68);
    }

    SECTION("Positive numbers (Don't Branch)") {
        using Address = typename RegisterFile::Unsigned;
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, 7);
        reg_file.set_unsigned(RegisterName::e_t1, 7);

        Instruction instr(IOp::e_bne, RegisterName::e_t0, RegisterName::e_t1,
                          16);

        reg_file.inc_pc(); // Emulate step
        const bool no_error = Executor::handle_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get_pc() == 4);

        reg_file.update_pc(); // moves past delays slot

        REQUIRE(reg_file.get_pc() == 8);
    }
}

// Branch on Less Than or Equal to Zero
TEST_CASE("pop06 - BLEZ", "[Executor]") {
    auto test = [](const BranchTest tcase) {
        test_branch(tcase, IOp::e_pop06);
    };

    SECTION("Equal to 0 (Branch)") { test({10, 0, 0, 0, true, false}); }

    SECTION("Not equal to 0 (Don't branch)") {
        test({10, 0, 1, 0, false, false});
    }

    SECTION("Largest positive immediate (dont branch)") {
        test({10, 0, 32767, 0, false, false});
    }

    SECTION("Smallest negative immediate (branch)") {
        test({10, 0, (uint32_t)(-32768), 0, true, false});
    }
}

// Branch on Greater Than Zero
TEST_CASE("pop07 - BGTZ", "[Executor]") {
    auto test = [](const BranchTest tcase) {
        test_branch(tcase, IOp::e_pop07);
    };

    SECTION("Equal to 0 (Don't branch)") { test({10, 0, 0, 0, false, false}); }

    SECTION("Greater than 0 (Branch)") { test({10, 0, 1, 0, true, false}); }

    SECTION("Largest positive immediate (Branch)") {
        test({10, 0, 32767, 0, true, false});
    }

    SECTION("Smallest negative immediate (Don't branch)") {
        test({10, 0, (uint32_t)(-32768), 0, false, false});
    }
}

// Memory instructions
TEST_CASE("load instructions", "[Executor]") {
    StaticMemory<256> memory;
    Result v = memory.template store<uint32_t>(4, (uint32_t)0x80222280);
    REQUIRE(!v.is_error());

    SECTION("byte positive") {
        RegisterFile reg_file;
        reg_file.set_unsigned(RegisterName::e_t0, 4);

        Instruction instr(IOp::e_lb, RegisterName::e_t1, RegisterName::e_t0, 1);

        const bool no_error =
            Executor::handle_itype_instr(instr, reg_file, memory);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t1).u == 0x22);
    }

    SECTION("byte negative") {
        RegisterFile reg_file;
        reg_file.set_unsigned(RegisterName::e_t0, 4);

        Instruction instr(IOp::e_lb, RegisterName::e_t1, RegisterName::e_t0, 0);

        const bool no_error =
            Executor::handle_itype_instr(instr, reg_file, memory);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t1).u == 0xFFFFFF80);
    }

    SECTION("halfword positive") {
        RegisterFile reg_file;
        reg_file.set_unsigned(RegisterName::e_t0, 4);

        Instruction instr(IOp::e_lh, RegisterName::e_t1, RegisterName::e_t0, 0);

        const bool no_error =
            Executor::handle_itype_instr(instr, reg_file, memory);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t1).u == 0x2280);
    }

    SECTION("halfword negative") {
        RegisterFile reg_file;
        reg_file.set_unsigned(RegisterName::e_t0, 4);

        Instruction instr(IOp::e_lh, RegisterName::e_t1, RegisterName::e_t0, 2);

        const bool no_error =
            Executor::handle_itype_instr(instr, reg_file, memory);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t1).u == 0xFFFF8022);
    }

    SECTION("halfword unsigned (lhu)") {
        RegisterFile reg_file;
        reg_file.set_unsigned(RegisterName::e_t0, 4);

        Instruction instr(IOp::e_lhu, RegisterName::e_t1, RegisterName::e_t0,
                          2);

        const bool no_error =
            Executor::handle_itype_instr(instr, reg_file, memory);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t1).u == 0x00008022);
    }

    SECTION("byte unsigned (lbu)") {
        RegisterFile reg_file;
        reg_file.set_unsigned(RegisterName::e_t0, 4);

        Instruction instr(IOp::e_lbu, RegisterName::e_t1, RegisterName::e_t0,
                          0);

        const bool no_error =
            Executor::handle_itype_instr(instr, reg_file, memory);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t1).u == 0x80);
    }

    SECTION("word neg offset") {
        RegisterFile reg_file;
        reg_file.set_unsigned(RegisterName::e_t0, 20);

        // 20 - 16 = 4
        Instruction instr(IOp::e_lw, RegisterName::e_t1, RegisterName::e_t0,
                          0xFFF0);

        const bool no_error =
            Executor::handle_itype_instr(instr, reg_file, memory);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t1).u == 0x80222280);
    }
}

TEST_CASE("store instructions", "[Executor]") {
    StaticMemory<256> memory;

    SECTION("byte") {
        RegisterFile reg_file;
        reg_file.set_unsigned(RegisterName::e_t0, 20);
        reg_file.set_unsigned(RegisterName::e_t1, 0x96);

        Instruction instr(IOp::e_sb, RegisterName::e_t1, RegisterName::e_t0,
                          0xFFF0);
        const bool no_error =
            Executor::handle_itype_instr(instr, reg_file, memory);
        REQUIRE(no_error);

        auto read_mem = memory.template read<uint8_t>(4);
        REQUIRE(!read_mem.is_error());

        REQUIRE(read_mem.get_value() == 0x96);
    }

    SECTION("halfword") {
        RegisterFile reg_file;
        reg_file.set_unsigned(RegisterName::e_t0, 20);
        reg_file.set_unsigned(RegisterName::e_t1, 0x10096);

        Instruction instr(IOp::e_sh, RegisterName::e_t1, RegisterName::e_t0, 0);
        const bool no_error =
            Executor::handle_itype_instr(instr, reg_file, memory);
        REQUIRE(no_error);

        auto read_mem = memory.template read<uint16_t>(20);
        REQUIRE(!read_mem.is_error());

        REQUIRE(read_mem.get_value() == 0x96);
    }

    SECTION("word") {
        RegisterFile reg_file;
        reg_file.set_unsigned(RegisterName::e_t0, 20);
        reg_file.set_unsigned(RegisterName::e_t1, 0x96);

        Instruction instr(IOp::e_sw, RegisterName::e_t1, RegisterName::e_t0, 0);
        const bool no_error =
            Executor::handle_itype_instr(instr, reg_file, memory);
        REQUIRE(no_error);

        auto read_mem = memory.template read<uint32_t>(20);
        REQUIRE(!read_mem.is_error());

        REQUIRE(read_mem.get_value() == 0x96);
    }
}

TEST_CASE("pop10") {
    auto test = [](const BranchTest tcase) {
        test_branch_compact(tcase, IOp::e_pop10);
    };

    SECTION("BEQZALC - No Branch") {
        test({0, 10, 0, 20, false, false});
        test({0, 10, 0, 1, false, false});
        test({0, 10, 0, 0xf0000000, false, false});
    }

    SECTION("BEQZALC - Branch") {
        test({0, 10, 0, 0, true, true});
        test({0, 15, 10, 0, true, true});
    }

    SECTION("BOVC - Branch") { // Branch not overflow
        test({10, 10, 0x7FFFFFFF, 0x7FFFFFFF, true, false});
        test({10, 5, 0x1, INT32_MAX, true, false});
        test({10, 5, INT32_MAX, 0x1, true, false});
    }

    SECTION("BOVC - No Branch") {
        test({10, 10, 0, 0, false, false});
        test({10, 5, 10, 0, false, false});
    }

    SECTION("BEQC - No Branch") {
        test({4, 10, 0, 1, false, false});
        test({4, 10, 100, 1, false, false});
        test({4, 10, 0xf0000, 0, false, false});
        test({4, 10, 0xf0000, 0x00f00000, false, false});
    }

    SECTION("BEQC - Branch") {
        test({4, 10, 0, 0, true, false});
        test({7, 15, 50, 50, true, false});
        test({7, 15, 0xff000000, 0xff000000, true, false});
    }
}

TEST_CASE("pop30") {
    auto test = [](const BranchTest tcase) {
        test_branch_compact(tcase, IOp::e_pop30);
    };

    SECTION("BNVC - No Branch") { // Branch not overflow
        test({10, 10, 0x40000000, 0x40000000, false, false});
        test({10, 5, 1, INT32_MAX, false, false});
        test({10, 4, INT32_MAX, 1, false, false});
    }   

    SECTION("BNVC - Branch") {
        test({10, 10, 0, 0, true, false});
        test({10, 5, 10, 0, true, false});
    }

    SECTION("BNEZALC - Branch") {
        test({0, 10, 0, 20, true, true});
        test({0, 10, 0, 1, true, true});
        test({0, 10, 0, 0xf0000000, true, true});
    }

    SECTION("BNEZALC - No Branch") {
        test({0, 10, 0, 0, false, false});
        test({0, 15, 10, 0, false, false});
    }

    SECTION("BNEC - Branch") {
        test({4, 10, 0, 1, true, false});
        test({4, 10, 100, 1, true, false});
        test({4, 10, 0xf0000, 0, true, false});
        test({4, 10, 0xf0000, 0x00f00000, true, false});
    }

    SECTION("BNEC - No Branch") {
        test({4, 10, 0, 0, false, false});
        test({7, 15, 50, 50, false, false});
        test({7, 15, 0xff000000, 0xff000000, false, false});
    }
}
