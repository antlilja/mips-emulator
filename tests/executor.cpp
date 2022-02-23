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
    SECTION("Positive numbers (Branch)") {
        using Address = typename RegisterFile::Unsigned;
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t0, 7);
        reg_file.set_unsigned(RegisterName::e_t1, 7);
        Instruction instr(IOp::e_beq, RegisterName::e_t0, RegisterName::e_t1,
                          16);

        const bool no_error = Executor::handle_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get_pc() == 0);

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

        const bool no_error = Executor::handle_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get_pc() == 0);

        reg_file.update_pc(); // moves past delays slot

        REQUIRE(reg_file.get_pc() == 4);
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

        const bool no_error = Executor::handle_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get_pc() == 0);

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

        const bool no_error = Executor::handle_itype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get_pc() == 0);

        reg_file.update_pc(); // moves past delays slot

        REQUIRE(reg_file.get_pc() == 4);
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

TEST_CASE("j", "[Executor]") {
    SECTION("Positive numbers") {
        using Unsigned = typename RegisterFile::Unsigned;
        RegisterFile reg_file;
        reg_file.set_pc(0x10beef00);

        Instruction instr(JOp::e_j, 0x003fc);

        const bool no_error = Executor::handle_jtype_instr(instr, reg_file);
        REQUIRE(no_error);

        reg_file.update_pc(); // moves past delays slot

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

        reg_file.update_pc(); // moves past delays slot

        REQUIRE(reg_file.get_pc() == 0x10000ff0);
        REQUIRE(reg_file.get(RegisterName::e_ra).u == 0x10beef04);
    }
}

TEST_CASE("jr", "[Executor]") {

    using Unsigned = typename RegisterFile::Unsigned;
    RegisterFile reg_file;
    reg_file.set_unsigned(RegisterName::e_t0, 0xbad);

    reg_file.set_pc(0x10000000);

    Instruction instr(Func::e_jr, RegisterName::e_0, RegisterName::e_t0,
                      RegisterName::e_0);

    const bool no_error = Executor::handle_rtype_instr(instr, reg_file);
    REQUIRE(no_error);

    reg_file.update_pc(); // moves past delays slot

    REQUIRE(reg_file.get_pc() == 0xbad);
}

TEST_CASE("jalr", "[Executor]") {

    using Unsigned = typename RegisterFile::Unsigned;
    RegisterFile reg_file;
    reg_file.set_unsigned(RegisterName::e_t0, 0xbad);

    reg_file.set_pc(0x10beef00);

    Instruction instr(Func::e_jalr, RegisterName::e_0, RegisterName::e_t0,
                      RegisterName::e_0);

    const bool no_error = Executor::handle_rtype_instr(instr, reg_file);
    REQUIRE(no_error);

    reg_file.update_pc(); // moves past delays slot

    REQUIRE(reg_file.get_pc() == 0xbad);
    REQUIRE(reg_file.get(RegisterName::e_ra).u == 0x10beef04);
}

TEST_CASE("bitswap", "[Executor") {
    using R = Instruction::Special3Func;
    using ROp = Instruction::Special3RTypeOp;

    SECTION("swaps") {
        uint32_t cases[][2] = {{0, 0},
                               {UINT32_MAX, UINT32_MAX},
                               {0x80808080, 0x01010101},
                               {0xf8f8f8f8, 0x1f1f1f1f},
                               {0x96969696, 0x69696969},
                               {0x18181818, 0x18181818},
                               {0xf8189680, 0x1f186901}};

        for (auto& test : cases) {
            RegisterFile reg_file;

            reg_file.set_unsigned(RegisterName::e_t0, 0);
            reg_file.set_unsigned(RegisterName::e_t1, test[0]);

            const Instruction instr(R::e_bshfl, ROp::e_bitswap,
                                    RegisterName::e_t0, RegisterName::e_t1);

            const bool no_error =
                Executor::handle_special3_rtype_instr(instr, reg_file);
            REQUIRE(no_error);

            REQUIRE(reg_file.get(RegisterName::e_t0).u == test[1]);
        }
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

TEST_CASE("rotr", "[Executor]") {
    const auto test_rotr = [](uint32_t amount, uint32_t input,
                              uint32_t output) {
        RegisterFile reg_file;
        reg_file.set_unsigned(RegisterName::e_t1, input);

        // ROTR is SRL with rs bit 1 = 1.
        const auto rotr_rs = static_cast<RegisterName>(1);
        const Instruction instr(Func::e_srl, RegisterName::e_t0, rotr_rs,
                                RegisterName::e_t1, amount);

        const bool no_error = Executor::handle_rtype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == output);
    };

    SECTION("Bit preservation") {
        for (int i = 0; i < 32; i++)
            test_rotr(i, ~0U, ~0U);
    }

    SECTION("Test Cases") {
        test_rotr(4, 0x162315u, 0x50016231u);
        test_rotr(2, 0x125623u, 0xc0049588u);
        test_rotr(3, 0x45324bfau, 0x48a6497fu);
        test_rotr(9, 0xdeadbeefu, 0x77ef56dfu);
        test_rotr(12, 0x124u, 0x12400000u);
        test_rotr(25, 0x4632132u, 0x31909902u);
    }
}

TEST_CASE("rotrv", "[Executor]") {
    const auto test_rotr = [](uint32_t amount, uint32_t input,
                              uint32_t output) {
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t1, input);
        reg_file.set_unsigned(RegisterName::e_t2, amount);

        // ROTRV is SRLV with shamt bit 1 = 1.
        const Instruction instr(Func::e_srlv, RegisterName::e_t0,
                                RegisterName::e_t2, RegisterName::e_t1, 1);

        const bool no_error = Executor::handle_rtype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == output);
    };

    // Reuse tests from rotr
    SECTION("Bit preservation") {
        for (int i = 0; i < 32; i++)
            test_rotr(i, ~0U, ~0U);
    }

    SECTION("Test Cases") {
        test_rotr(4, 0x162315u, 0x50016231u);
        test_rotr(2, 0x125623u, 0xc0049588u);
        test_rotr(3, 0x45324bfau, 0x48a6497fu);
        test_rotr(9, 0xdeadbeefu, 0x77ef56dfu);
        test_rotr(12, 0x124u, 0x12400000u);
        test_rotr(25, 0x4632132u, 0x31909902u);
    }
}

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

TEST_CASE("seh", "[Executor]") {
    using R = Instruction::Special3Func;
    using ROp = Instruction::Special3RTypeOp;

    SECTION("Leading 1") {
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t1, 0x0000F0F0);

        const Instruction instr(R::e_bshfl, ROp::e_seh, RegisterName::e_t0,
                                RegisterName::e_t1);

        const bool no_error =
            Executor::handle_special3_rtype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == 0xFFFFF0F0);
    }

    SECTION("Leading 0") {
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t1, 0x7ff1);

        const Instruction instr(R::e_bshfl, ROp::e_seh, RegisterName::e_t0,
                                RegisterName::e_t1);

        const bool no_error =
            Executor::handle_special3_rtype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == 0x7ff1);
    }
}

TEST_CASE("seb", "[Executor]") {
    using R = Instruction::Special3Func;
    using ROp = Instruction::Special3RTypeOp;

    SECTION("Leading 1") {
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t1, 0x000000F8);

        const Instruction instr(R::e_bshfl, ROp::e_seb, RegisterName::e_t0,
                                RegisterName::e_t1);

        const bool no_error =
            Executor::handle_special3_rtype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == 0xFFFFFFF8);
    }

    SECTION("Leading 0") {
        RegisterFile reg_file;

        reg_file.set_unsigned(RegisterName::e_t1, 0xFFFFFF7F);

        const Instruction instr(R::e_bshfl, ROp::e_seb, RegisterName::e_t0,
                                RegisterName::e_t1);

        const bool no_error =
            Executor::handle_special3_rtype_instr(instr, reg_file);
        REQUIRE(no_error);

        REQUIRE(reg_file.get(RegisterName::e_t0).u == 0x0000007F);
    }
}

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
