#include "mips-emulator/register_file.hpp"

#include <catch2/catch.hpp>

#include <limits>

using namespace mips_emulator;

TEST_CASE("Default initialization is zero initalized", "[RegisterFile]") {
    RegisterFile state;
    for (int i = 0; i < RegisterFile::REGISTER_COUNT; ++i)
        REQUIRE(state.get(i).u == 0);
}

TEST_CASE("Set register unsigned", "[RegisterFile]") {
    SECTION("to 1") {
        RegisterFile reg_file;
        for (int i = 1; i < RegisterFile::REGISTER_COUNT; ++i) {
            reg_file.set_unsigned(i, 1);
            REQUIRE(reg_file.get(i).u == 1);
        }
    }

    SECTION("to MAX") {
        constexpr typename RegisterFile::Unsigned U_MAX =
            std::numeric_limits<typename RegisterFile::Unsigned>::max();

        RegisterFile reg_file;
        for (int i = 1; i < RegisterFile::REGISTER_COUNT; ++i) {
            reg_file.set_unsigned(i, U_MAX);
            REQUIRE(reg_file.get(i).u == U_MAX);
        }
    }
}

TEST_CASE("Set register signed", "[RegisterFile]") {
    SECTION("to 1") {
        RegisterFile reg_file;
        for (int i = 1; i < RegisterFile::REGISTER_COUNT; ++i) {
            reg_file.set_signed(i, 1);
            REQUIRE(reg_file.get(i).s == 1);
        }
    }

    SECTION("to -1") {
        RegisterFile reg_file;
        for (int i = 1; i < RegisterFile::REGISTER_COUNT; ++i) {
            reg_file.set_signed(i, -1);
            REQUIRE(reg_file.get(i).s == -1);
        }
    }

    SECTION("to MAX") {
        constexpr typename RegisterFile::Signed S_MAX =
            std::numeric_limits<typename RegisterFile::Signed>::max();

        RegisterFile reg_file;
        for (int i = 1; i < RegisterFile::REGISTER_COUNT; ++i) {
            reg_file.set_signed(i, S_MAX);
            REQUIRE(reg_file.get(i).s == S_MAX);
        }
    }

    SECTION("to MIN") {

        constexpr typename RegisterFile::Signed S_MIN =
            std::numeric_limits<typename RegisterFile::Signed>::min();

        RegisterFile reg_file;
        for (int i = 1; i < RegisterFile::REGISTER_COUNT; ++i) {
            reg_file.set_signed(i, S_MIN);
            REQUIRE(reg_file.get(i).s == S_MIN);
        }
    }
}

TEST_CASE("Register $0 is always zero", "[RegisterFile]") {
    constexpr typename RegisterFile::Unsigned U_MAX =
        std::numeric_limits<typename RegisterFile::Unsigned>::max();

    constexpr typename RegisterFile::Unsigned S_MAX =
        std::numeric_limits<typename RegisterFile::Signed>::max();

    RegisterFile reg_file;
    reg_file.set_unsigned(0, 1);
    REQUIRE(reg_file.get(0).u == 0);

    reg_file.set_unsigned(0, U_MAX);
    REQUIRE(reg_file.get(0).u == 0);

    reg_file.set_signed(0, S_MAX);
    REQUIRE(reg_file.get(0).u == 0);

    reg_file.set_signed(0, -5);
    REQUIRE(reg_file.get(0).u == 0);
}

TEST_CASE("update pc", "[Executor]") {
    RegisterFile reg_file;
    reg_file.set_pc(0);

    SECTION("increment") {
        reg_file.update_pc();

        REQUIRE(reg_file.get_pc() == 4);
    }

    SECTION("branch") {
        reg_file.delayed_branch(0xdad);
        reg_file.update_pc();

        REQUIRE(reg_file.get_pc() == 0xdad);

        // test that the branch_flag was reset
        reg_file.update_pc();

        REQUIRE(reg_file.get_pc() == 0xdb1);
    }
}
