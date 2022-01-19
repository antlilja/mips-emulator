#include "mips-emulator/register_state.hpp"

#include <catch2/catch.hpp>

#include <limits>

using namespace mips_emulator;

TEMPLATE_TEST_CASE("Default initialization is zero initalized",
                   "[RegisterState]", RegisterState32, RegisterState64) {
    TestType state;
    for (int i = 0; i < TestType::REGISTER_COUNT; ++i)
        REQUIRE(state.get_register(i).u == 0);
}

TEMPLATE_TEST_CASE("Set register unsigned", "[RegisterState]", RegisterState32,
                   RegisterState64) {
    SECTION("to 1") {
        TestType state;
        for (int i = 1; i < TestType::REGISTER_COUNT; ++i) {
            state.set_register_unsigned(i, 1);
            REQUIRE(state.get_register(i).u == 1);
        }
    }

    SECTION("to MAX") {
        constexpr typename TestType::Unsigned U_MAX =
            std::numeric_limits<typename TestType::Unsigned>::max();

        TestType state;
        for (int i = 1; i < TestType::REGISTER_COUNT; ++i) {
            state.set_register_unsigned(i, U_MAX);
            REQUIRE(state.get_register(i).u == U_MAX);
        }
    }
}

TEMPLATE_TEST_CASE("Set register signed", "[RegisterState]", RegisterState32,
                   RegisterState64) {
    SECTION("to 1") {
        TestType state;
        for (int i = 1; i < TestType::REGISTER_COUNT; ++i) {
            state.set_register_signed(i, 1);
            REQUIRE(state.get_register(i).s == 1);
        }
    }

    SECTION("to -1") {
        TestType state;
        for (int i = 1; i < TestType::REGISTER_COUNT; ++i) {
            state.set_register_signed(i, -1);
            REQUIRE(state.get_register(i).s == -1);
        }
    }

    SECTION("to MAX") {
        constexpr typename TestType::Signed S_MAX =
            std::numeric_limits<typename TestType::Signed>::max();

        TestType state;
        for (int i = 1; i < TestType::REGISTER_COUNT; ++i) {
            state.set_register_signed(i, S_MAX);
            REQUIRE(state.get_register(i).s == S_MAX);
        }
    }

    SECTION("to MIN") {

        constexpr typename TestType::Signed S_MIN =
            std::numeric_limits<typename TestType::Signed>::min();

        TestType state;
        for (int i = 1; i < TestType::REGISTER_COUNT; ++i) {
            state.set_register_signed(i, S_MIN);
            REQUIRE(state.get_register(i).s == S_MIN);
        }
    }
}

TEMPLATE_TEST_CASE("Register $0 is always zero", "[RegisterState]",
                   RegisterState32, RegisterState64) {
    constexpr typename TestType::Unsigned U_MAX =
        std::numeric_limits<typename TestType::Unsigned>::max();

    constexpr typename TestType::Unsigned S_MAX =
        std::numeric_limits<typename TestType::Signed>::max();

    TestType state;
    state.set_register_unsigned(0, 1);
    REQUIRE(state.get_register(0).u == 0);

    state.set_register_unsigned(0, U_MAX);
    REQUIRE(state.get_register(0).u == 0);

    state.set_register_signed(0, S_MAX);
    REQUIRE(state.get_register(0).u == 0);

    state.set_register_signed(0, -5);
    REQUIRE(state.get_register(0).u == 0);
}
