#pragma once

namespace mips_emulator {
    template <typename Value, typename Error>
    class Result {
    public:
        [[nodiscard]] bool is_error() const { return is_err; }

        [[nodiscard]] Value get_value() const { return value; }
        [[nodiscard]] Error get_error() const { return error; }

        Result(Value value) {
            is_err = false;
            value = value;
        }

        Result(Error error) {
            is_err = true;
            error = error;
        }

    private:
        bool is_err;
        union {
            Value value;
            Error error;
        };
    };

    template <typename Error>
    class Result<void, Error> {
    public:
        [[nodiscard]] bool is_error() const { return is_err; }

        [[nodiscard]] Error get_error() const { return error; }

        Result() { is_err = false; }

        Result(Error error) {
            is_err = true;
            error = error;
        }

    private:
        bool is_err;
        Error error;
    };

} // namespace mips_emulator
