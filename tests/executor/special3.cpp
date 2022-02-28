#include "mips-emulator/executor.hpp"
#include "mips-emulator/instruction.hpp"
#include "mips-emulator/register_file.hpp"
#include "mips-emulator/register_name.hpp"

#include <catch2/catch.hpp>

using namespace mips_emulator;

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
