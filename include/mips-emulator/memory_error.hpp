#pragma once
#include <cstdint>

namespace mips_emulator {
    enum class MemoryError : uint8_t {
        unaligned_access,
        out_of_bounds_access,
    };
}
