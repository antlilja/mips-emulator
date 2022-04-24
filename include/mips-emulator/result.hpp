#pragma once

namespace mips_emulator {
    template <typename Value, typename Error>
    class Result {
    public:
        [[nodiscard]] bool is_error() const { return is_err; }

        [[nodiscard]] Value get_value() const { return value; }
        [[nodiscard]] Error get_error() const { return error; }

        Result(Value val) {
            is_err = false;
            value = val;
        }

        Result(Error err) {
            is_err = true;
            error = err;
        }

    private:
        bool is_err;
        union {
            Value value;
            Error error;
        };
    };

    template <typename Value>
    class Result<Value, void> {
    public:
        [[nodiscard]] bool is_error() const { return is_err; }

        [[nodiscard]] Value get_value() const { return value; }

        Result() { is_err = true; }

        Result(Value val) {
            is_err = false;
            value = val;
        }

    private:
        bool is_err;
        Value value;
    };

    template <typename Error>
    class Result<void, Error> {
    public:
        [[nodiscard]] bool is_error() const { return is_err; }

        [[nodiscard]] Error get_error() const { return error; }

        Result() { is_err = false; }

        Result(Error err) {
            is_err = true;
            error = err;
        }

    private:
        bool is_err;
        Error error;
    };

} // namespace mips_emulator
