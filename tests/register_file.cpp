#include "mips-emulator/register_file.hpp"

#include <catch2/catch.hpp>

#include <limits>

using namespace mips_emulator;

TEMPLATE_TEST_CASE("Default initialization is zero initalized",
                   "[RegisterFile]", RegisterFile32, RegisterFile64) {
    TestType state;
    for (int i = 0; i < TestType::REGISTER_COUNT; ++i)
        REQUIRE(state.get(i).u == 0);
}

TEMPLATE_TEST_CASE("Set register unsigned", "[RegisterFile]", RegisterFile32,
                   RegisterFile64) {
    SECTION("to 1") {
        TestType reg_file;
        for (int i = 1; i < TestType::REGISTER_COUNT; ++i) {
            reg_file.set_unsigned(i, 1);
            REQUIRE(reg_file.get(i).u == 1);
        }
    }

    SECTION("to MAX") {
        constexpr typename TestType::Unsigned U_MAX =
            std::numeric_limits<typename TestType::Unsigned>::max();

        TestType reg_file;
        for (int i = 1; i < TestType::REGISTER_COUNT; ++i) {
            reg_file.set_unsigned(i, U_MAX);
            REQUIRE(reg_file.get(i).u == U_MAX);
        }
    }
}

TEMPLATE_TEST_CASE("Set register signed", "[RegisterFile]", RegisterFile32,
                   RegisterFile64) {
    SECTION("to 1") {
        TestType reg_file;
        for (int i = 1; i < TestType::REGISTER_COUNT; ++i) {
            reg_file.set_signed(i, 1);
            REQUIRE(reg_file.get(i).s == 1);
        }
    }

    SECTION("to -1") {
        TestType reg_file;
        for (int i = 1; i < TestType::REGISTER_COUNT; ++i) {
            reg_file.set_signed(i, -1);
            REQUIRE(reg_file.get(i).s == -1);
        }
    }

    SECTION("to MAX") {
        constexpr typename TestType::Signed S_MAX =
            std::numeric_limits<typename TestType::Signed>::max();

        TestType reg_file;
        for (int i = 1; i < TestType::REGISTER_COUNT; ++i) {
            reg_file.set_signed(i, S_MAX);
            REQUIRE(reg_file.get(i).s == S_MAX);
        }
    }

    SECTION("to MIN") {

        constexpr typename TestType::Signed S_MIN =
            std::numeric_limits<typename TestType::Signed>::min();

        TestType reg_file;
        for (int i = 1; i < TestType::REGISTER_COUNT; ++i) {
            reg_file.set_signed(i, S_MIN);
            REQUIRE(reg_file.get(i).s == S_MIN);
        }
    }
}

TEMPLATE_TEST_CASE("Register $0 is always zero", "[RegisterFile]",
                   RegisterFile32, RegisterFile64) {
    constexpr typename TestType::Unsigned U_MAX =
        std::numeric_limits<typename TestType::Unsigned>::max();

    constexpr typename TestType::Unsigned S_MAX =
        std::numeric_limits<typename TestType::Signed>::max();

    TestType reg_file;
    reg_file.set_unsigned(0, 1);
    REQUIRE(reg_file.get(0).u == 0);

    reg_file.set_unsigned(0, U_MAX);
    REQUIRE(reg_file.get(0).u == 0);

    reg_file.set_signed(0, S_MAX);
    REQUIRE(reg_file.get(0).u == 0);

    reg_file.set_signed(0, -5);
    REQUIRE(reg_file.get(0).u == 0);
}
