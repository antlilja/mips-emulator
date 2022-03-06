#include "mips-emulator/executor.hpp"
#include "mips-emulator/instruction.hpp"
#include "mips-emulator/register_file.hpp"
#include "mips-emulator/register_name.hpp"
#include "mips-emulator/static_memory.hpp"

#include <catch2/catch.hpp>

using namespace mips_emulator;

using IOp = Instruction::ITypeOpcode;

TEST_CASE("addi", "[Executor]") {
    SECTION("Positive numbers") {
        using Address = typename RegisterFile::Unsigned;
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, 100202);

        Instruction instr(IOp::e_addi, RegisterName::e_t1, RegisterName::e_t0,
                          22020);

        const bool no_error = Executor::handle_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t1).u == 122222);
    }
}

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
        reg_file.set_unsigned(RegisterName::e_t0, UINT32_MAX-1);

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
TEST_CASE("blez", "[Executor]") {
    SECTION("Equal to 0 (Branch)") {
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, 0);

        Instruction instr(IOp::e_blez, RegisterName::e_t0, 16);

        reg_file.inc_pc(); // Emulate step
        const bool no_error = Executor::handle_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get_pc() == 4);

        reg_file.update_pc(); // moves past delays slot

        REQUIRE(reg_file.get_pc() == 68);
    }

    SECTION("Not equal to 0 (Don't branch)") {
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, 1);

        Instruction instr(IOp::e_blez, RegisterName::e_t0, 16);

        reg_file.inc_pc(); // Emulate step
        const bool no_error = Executor::handle_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get_pc() == 4);

        reg_file.update_pc(); // moves past delays slot

        REQUIRE(reg_file.get_pc() == 8);
    }

    SECTION("Largest positive immediate (dont branch)") {
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, 32767);

        Instruction instr(IOp::e_blez, RegisterName::e_t0, 16);

        reg_file.inc_pc(); // Emulate step
        const bool no_error = Executor::handle_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get_pc() == 4);

        reg_file.update_pc(); // moves past delays slot

        REQUIRE(reg_file.get_pc() == 8);
    }

    SECTION("Smallest negative immediate (branch)") {
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, -32768);

        Instruction instr(IOp::e_blez, RegisterName::e_t0, 16);

        reg_file.inc_pc(); // Emulate step
        const bool no_error = Executor::handle_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get_pc() == 4);

        reg_file.update_pc(); // moves past delays slot

        REQUIRE(reg_file.get_pc() == 68);
    }
}

// Branch on Greater Than Zero
TEST_CASE("bgtz", "[Executor]") {
    SECTION("Equal to 0 (Don't branch)") {
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, 0);

        Instruction instr(IOp::e_bgtz, RegisterName::e_t0, 16);

        reg_file.inc_pc(); // Emulate step
        const bool no_error = Executor::handle_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get_pc() == 4);

        reg_file.update_pc(); // moves past delays slot

        REQUIRE(reg_file.get_pc() == 8);
    }

    SECTION("Greater than 0 (Branch)") {
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, 1);

        Instruction instr(IOp::e_bgtz, RegisterName::e_t0, 16);

        reg_file.inc_pc(); // Emulate step
        const bool no_error = Executor::handle_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get_pc() == 4);

        reg_file.update_pc(); // moves past delays slot

        REQUIRE(reg_file.get_pc() == 68);
    }

    SECTION("Largest positive immediate (Branch)") {
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, 32767);

        Instruction instr(IOp::e_bgtz, RegisterName::e_t0, 16);

        reg_file.inc_pc(); // Emulate step
        const bool no_error = Executor::handle_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get_pc() == 4);

        reg_file.update_pc(); // moves past delays slot

        REQUIRE(reg_file.get_pc() == 68);
    }

    SECTION("Smallest negative immediate (Don't branch)") {
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, -32768);

        Instruction instr(IOp::e_bgtz, RegisterName::e_t0, 16);

        reg_file.inc_pc(); // Emulate step
        const bool no_error = Executor::handle_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get_pc() == 4);

        reg_file.update_pc(); // moves past delays slot

        REQUIRE(reg_file.get_pc() == 8);
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
