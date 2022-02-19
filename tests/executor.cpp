#include "mips-emulator/executor.hpp"
#include "mips-emulator/instruction.hpp"
#include "mips-emulator/register_file.hpp"
#include "mips-emulator/register_name.hpp"

#include <catch2/catch.hpp>

using namespace mips_emulator;

using Func = Instruction::Func;
using IOp = Instruction::ITypeOpcode;
using JOp = Instruction::JTypeOpcode;

TEST_CASE("add", "[Executor]") {
    SECTION("Positive numbers") {
        using Address = typename RegisterFile::Unsigned;
        RegisterFile reg_file;

        reg_file.set_signed(RegisterName::e_t0, 1);
        reg_file.set_signed(RegisterName::e_t1, 5);

        Instruction instr(Func::e_add, RegisterName::e_t2, RegisterName::e_t0,
                          RegisterName::e_t1);

        const bool no_error = Executor::handle_rtype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t2).s == 6);
    }
}

TEST_CASE("sub", "[Executor]") {
    SECTION("Positive numbers") {
        using Address = typename RegisterFile::Unsigned;
        RegisterFile reg_file;

        reg_file.set_signed(RegisterName::e_t0, 10);
        reg_file.set_signed(RegisterName::e_t1, 1);

        Instruction instr(Func::e_sub, RegisterName::e_t2, RegisterName::e_t0,
                          RegisterName::e_t1);

        const bool no_error = Executor::handle_rtype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t2).s == 9);
    }

    SECTION("Negative numbers") {
        using Address = typename RegisterFile::Unsigned;
        RegisterFile reg_file;
        Address pc = 0;

        reg_file.set_signed(RegisterName::e_t0, -3);
        reg_file.set_signed(RegisterName::e_t1, -5);

        Instruction instr(Func::e_sub, RegisterName::e_t2, RegisterName::e_t0,
                          RegisterName::e_t1);

        const bool no_error = Executor::handle_rtype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t2).s == 2);
    }
}

TEST_CASE("or", "[Executor]") {
    SECTION("Positive numbers") {
        using Address = typename RegisterFile::Unsigned;
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, 0b1);   // 1
        reg_file.set_unsigned(RegisterName::e_t1, 0b110); // 6

        Instruction instr(Func::e_or, RegisterName::e_t2, RegisterName::e_t0,
                          RegisterName::e_t1);

        const bool no_error = Executor::handle_rtype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t2).u == 0b111); // 7
    }
}

TEST_CASE("beq", "[Executor]") {
    SECTION("Positive numbers") {
        using Address = typename RegisterFile::Unsigned;
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, 7);
        reg_file.set_unsigned(RegisterName::e_t1, 7);
        Instruction instr(IOp::e_beq, RegisterName::e_t0, RegisterName::e_t1,
                          16);

        const bool no_error = Executor::handle_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get_pc() == 68);
    }
}

TEST_CASE("bne", "[Executor]") {
    SECTION("Positive numbers") {
        using Address = typename RegisterFile::Unsigned;
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, 7);
        reg_file.set_unsigned(RegisterName::e_t1, 7);

        Instruction instr(IOp::e_bne, RegisterName::e_t0, RegisterName::e_t1,
                          16);

        const bool no_error = Executor::handle_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get_pc() == 0);
    }
}

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

TEST_CASE("sll", "[Executor]") {
    SECTION("Shift number") {
        using Address = typename RegisterFile::Unsigned;
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t1, 123456);

        const Instruction instr(Func::e_sll, RegisterName::e_t0,
                                RegisterName::e_0, RegisterName::e_t1, 4);

        const bool no_error = Executor::handle_rtype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == 1975296);
    }
}

TEST_CASE("sllv", "[Executor]") {
    SECTION("Shift number") {
        using Address = typename RegisterFile::Unsigned;
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t1, 123456);
        reg_file.set_unsigned(RegisterName::e_t2, 4);

        const Instruction instr(Func::e_sllv, RegisterName::e_t0,
                                RegisterName::e_t2, RegisterName::e_t1);

        const bool no_error = Executor::handle_rtype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == 1975296);
    }

    SECTION("Correct bits used") {
        using Address = typename RegisterFile::Unsigned;
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t1, 1);
        reg_file.set_unsigned(RegisterName::e_t2, 0xFFFFFFE1);

        const Instruction instr(Func::e_sllv, RegisterName::e_t0,
                                RegisterName::e_t2, RegisterName::e_t1);

        const bool no_error = Executor::handle_rtype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == 2);
    }
}

TEST_CASE("sra", "[Executor]") {
    SECTION("Shift positive number") {
        using Address = typename RegisterFile::Unsigned;
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t1, 123456);

        const Instruction instr(Func::e_sra, RegisterName::e_t0,
                                RegisterName::e_0, RegisterName::e_t1, 4);

        const bool no_error = Executor::handle_rtype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == 7716);
    }

    SECTION("Shift negative number") {
        using Address = typename RegisterFile::Unsigned;
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t1, -123456);

        const Instruction instr(Func::e_sra, RegisterName::e_t0,
                                RegisterName::e_0, RegisterName::e_t1, 4);

        const bool no_error = Executor::handle_rtype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == -7716);
    }
}

TEST_CASE("srav", "[Executor]") {
    SECTION("Shift positive number") {
        using Address = typename RegisterFile::Unsigned;
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t1, 123456);
        reg_file.set_unsigned(RegisterName::e_t2, 4);

        const Instruction instr(Func::e_srav, RegisterName::e_t0,
                                RegisterName::e_t2, RegisterName::e_t1);

        const bool no_error = Executor::handle_rtype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == 7716);
    }

    SECTION("Shift negative number") {
        using Address = typename RegisterFile::Unsigned;
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t1, -123456);
        reg_file.set_unsigned(RegisterName::e_t2, 4);

        const Instruction instr(Func::e_srav, RegisterName::e_t0,
                                RegisterName::e_t2, RegisterName::e_t1);

        const bool no_error = Executor::handle_rtype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == -7716);
    }

    SECTION("Correct bits used") {
        using Address = typename RegisterFile::Unsigned;
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t1, 2);
        reg_file.set_unsigned(RegisterName::e_t2, 0xFFFFFFE1);

        const Instruction instr(Func::e_srav, RegisterName::e_t0,
                                RegisterName::e_t2, RegisterName::e_t1);

        const bool no_error = Executor::handle_rtype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == 1);
    }
}

TEST_CASE("srl", "[Executor]") {
    SECTION("Shift number") {
        using Address = typename RegisterFile::Unsigned;
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t1, 123456);

        const Instruction instr(Func::e_srl, RegisterName::e_t0,
                                RegisterName::e_0, RegisterName::e_t1, 4);

        const bool no_error = Executor::handle_rtype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == 7716);
    }
}

TEST_CASE("srlv", "[Executor]") {
    SECTION("Shift number") {
        using Address = typename RegisterFile::Unsigned;
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t1, 123456);
        reg_file.set_unsigned(RegisterName::e_t2, 4);

        const Instruction instr(Func::e_srlv, RegisterName::e_t0,
                                RegisterName::e_t2, RegisterName::e_t1);

        const bool no_error = Executor::handle_rtype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == 7716);
    }

    SECTION("Correct bits used") {
        using Address = typename RegisterFile::Unsigned;
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t1, 2);
        reg_file.set_unsigned(RegisterName::e_t2, 0xFFFFFFE1);

        const Instruction instr(Func::e_srlv, RegisterName::e_t0,
                                RegisterName::e_t2, RegisterName::e_t1);

        const bool no_error = Executor::handle_rtype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == 1);
    }
}

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

TEST_CASE("lui", "[Executor]") {
    SECTION("Positive numbers") {
        using Address = typename RegisterFile::Unsigned;
        RegisterFile reg_file;

        Instruction instr(IOp::e_lui, RegisterName::e_t1, RegisterName::e_0,
                          0xbeef);

        const bool no_error = Executor::handle_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t1).u == 0xbeef0000);
    }
}

TEST_CASE("j", "[Executor]") {
    SECTION("Positive numbers") {
        using Unsigned = typename RegisterFile::Unsigned;
        RegisterFile reg_file;
        reg_file.set_pc(0x10beef00);

        Instruction instr(JOp::e_j, 0x003fc);

        const bool no_error = Executor::handle_jtype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get_pc() == 0x10000ff0);
    }
}

TEST_CASE("jal", "[Executor]") {
    SECTION("Positive numbers") {
        using Unsigned = typename RegisterFile::Unsigned;
        RegisterFile reg_file;
        reg_file.set_pc(0x10beef00);

        Instruction instr(JOp::e_jal, 0x003fc);

        const bool no_error = Executor::handle_jtype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get_pc() == 0x10000ff0);
        REQUIRE(reg_file.get(RegisterName::e_ra).u == 0x10beef04);
    }
}

TEST_CASE("wsbh", "[Executor]") {
    using R = Instruction::Special3Func;
    using ROp = Instruction::Special3RTypeOp;

    SECTION("Swapping") {
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, 0);
        reg_file.set_unsigned(RegisterName::e_t1, 0xFF00FF00);

        const Instruction instr(R::e_bshfl, ROp::e_wsbh, RegisterName::e_t0,
                                RegisterName::e_t1);

        const bool no_error =
            Executor::handle_special3_rtype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == 0x00FF00FF);
    }

    SECTION("swapping zeros") {
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, 0);
        reg_file.set_unsigned(RegisterName::e_t1, 0);

        const Instruction instr(R::e_bshfl, ROp::e_wsbh, RegisterName::e_t0,
                                RegisterName::e_t1);

        const bool no_error =
            Executor::handle_special3_rtype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == 0);
    }

    SECTION("swapping ones") {
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, 0);
        reg_file.set_unsigned(RegisterName::e_t1, ~0U);

        const Instruction instr(R::e_bshfl, ROp::e_wsbh, RegisterName::e_t0,
                                RegisterName::e_t1);

        const bool no_error =
            Executor::handle_special3_rtype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == ~0U);
    }
}

TEST_CASE("sop30", "[Executor]") {
    SECTION("mul") {
        int32_t values[] = {-0x6FF,   0x55,        0x125,    0x7564,
                            0x523522, -0x7FCCA241, 0x23525,  0x1247,
                            0xFFFF,   INT32_MAX,   INT32_MIN};

        for (int val1 : values) {
            for (int val2 : values) {
                RegisterFile reg_file;
                reg_file.set_signed(RegisterName::e_t0, val1);
                reg_file.set_signed(RegisterName::e_t1, val2);

                Instruction instr(Func::e_sop30, RegisterName::e_t0,
                                  RegisterName::e_t0, RegisterName::e_t1, 2);

                const bool no_error =
                    Executor::handle_rtype_instr(instr, reg_file);
                REQUIRE(no_error);

                REQUIRE(reg_file.get(RegisterName::e_t0).s == val1 * val2);
            }
        }
    }

    // Using mars mult mfhi
    SECTION("muh") {
        int32_t cases[][3] = {{-0x126373, -0x126373, (int32_t)0x00000152},
                              {-0x126373, 0xF2A373, (int32_t)0xffffee92},
                              {0xABC1235, 0xF2A373, (int32_t)0x000a2ca3}};

        for (auto& test : cases) {
            const int32_t val1 = test[0];
            const int32_t val2 = test[1];
            const int32_t res = test[2];

            RegisterFile reg_file;
            reg_file.set_signed(RegisterName::e_t0, val1);
            reg_file.set_signed(RegisterName::e_t1, val2);

            Instruction instr(Func::e_sop30, RegisterName::e_t0,
                              RegisterName::e_t0, RegisterName::e_t1, 3);

            const bool no_error = Executor::handle_rtype_instr(instr, reg_file);
            REQUIRE(no_error);

            REQUIRE(reg_file.get(RegisterName::e_t0).s == res);
        }
    }
}

TEST_CASE("sop31", "[Executor]") {
    SECTION("mul") {
        uint32_t values[] = {0x6FF,    0x55,       0x125,   0x7564,
                             0x523522, 0x7FCCA241, 0x23525, 0x1247,
                             0xFFFF,   UINT32_MAX, 0};

        for (int val1 : values) {
            for (int val2 : values) {
                RegisterFile reg_file;
                reg_file.set_unsigned(RegisterName::e_t0, val1);
                reg_file.set_unsigned(RegisterName::e_t1, val2);

                Instruction instr(Func::e_sop31, RegisterName::e_t0,
                                  RegisterName::e_t0, RegisterName::e_t1, 2);

                const bool no_error =
                    Executor::handle_rtype_instr(instr, reg_file);
                REQUIRE(no_error);

                REQUIRE(reg_file.get(RegisterName::e_t0).u == val1 * val2);
            }
        }
    }

    // Using mars multu mfhi
    SECTION("muh") {
        uint32_t cases[][3] = {{0x126373, 0x126373, 0x00000152},
                               {0x126373, 0xF2A373, 0x0000116d},
                               {0xABC1235, 0xF2A373, 0x000a2ca3}};

        for (auto& test : cases) {
            const int32_t val1 = test[0];
            const int32_t val2 = test[1];
            const int32_t res = test[2];

            RegisterFile reg_file;
            reg_file.set_signed(RegisterName::e_t0, val1);
            reg_file.set_signed(RegisterName::e_t1, val2);

            Instruction instr(Func::e_sop30, RegisterName::e_t0,
                              RegisterName::e_t0, RegisterName::e_t1, 3);

            const bool no_error = Executor::handle_rtype_instr(instr, reg_file);
            REQUIRE(no_error);

            REQUIRE(reg_file.get(RegisterName::e_t0).u == res);
        }
    }
}

TEST_CASE("slt", "[Executor]") {
    SECTION("Equal numbers") {
        RegisterFile reg_file;

        reg_file.set_signed(RegisterName::e_t0, 5);
        reg_file.set_signed(RegisterName::e_t1, 5);

        Instruction instr(Func::e_slt, RegisterName::e_t2, RegisterName::e_t0,
                          RegisterName::e_t1);

        const bool no_error = Executor::handle_rtype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t2).s == 0);
    }

    SECTION("Lesser number") {
        RegisterFile reg_file;

        reg_file.set_signed(RegisterName::e_t0, 2);
        reg_file.set_signed(RegisterName::e_t1, 5);

        Instruction instr(Func::e_slt, RegisterName::e_t2, RegisterName::e_t0,
                          RegisterName::e_t1);

        const bool no_error = Executor::handle_rtype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t2).s == 1);
    }
}

TEST_CASE("sltu", "[Executor]") {
    SECTION("Equal numbers") {
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, 5);
        reg_file.set_unsigned(RegisterName::e_t1, 5);

        Instruction instr(Func::e_slt, RegisterName::e_t2, RegisterName::e_t0,
                          RegisterName::e_t1);

        const bool no_error = Executor::handle_rtype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t2).s == 0);
    }

    SECTION("Lesser number") {
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, 2);
        reg_file.set_unsigned(RegisterName::e_t1, 5);

        Instruction instr(Func::e_slt, RegisterName::e_t2, RegisterName::e_t0,
                          RegisterName::e_t1);

        const bool no_error = Executor::handle_rtype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t2).s == 1);
    }
}
