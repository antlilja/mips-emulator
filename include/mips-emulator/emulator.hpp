#pragma once
#include "mips-emulator/instruction.hpp"
#include "mips-emulator/executor.hpp"

#include <utility>

namespace mips_emulator {
    template <typename RegisterFile, typename Memory>
    class Emulator {
    public:
        using Register = typename RegisterFile::Register;
        using Address = typename RegisterFile::Unsigned;

        template <typename... Args>
        Emulator(Args&&... args) : memory(std::forward<Args>(args)...) {}

        const RegisterFile& get_register_file() const noexcept {
            return reg_file;
        }
        RegisterFile clone_register_file() const noexcept { return reg_file; }

        [[nodiscard]] bool step() noexcept {
            return Executor::step(reg_file, memory, pc);
        }

    private:
        RegisterFile reg_file;
        Memory memory;

        Address pc;
    };
} // namespace mips_emulator
