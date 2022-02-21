#pragma once
#include "mips-emulator/instruction.hpp"
#include "mips-emulator/executor.hpp"
#include "mips-emulator/register_file.hpp"

#include <utility>

namespace mips_emulator {
    template <typename Memory>
    class Emulator {
    public:
        template <typename... Args>
        Emulator(Args&&... args) : memory(std::forward<Args>(args)...) {}

        const RegisterFile& get_register_file() const noexcept {
            return reg_file;
        }
        RegisterFile clone_register_file() const noexcept { return reg_file; }

        [[nodiscard]] bool step() noexcept {
            return Executor::step(reg_file, memory);
        }

    private:
        RegisterFile reg_file;
        Memory memory;
    };
} // namespace mips_emulator
