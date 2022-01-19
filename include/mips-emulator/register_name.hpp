#pragma once
#include <cstdint>

namespace mips_emulator {
    enum class RegisterName : uint8_t {
        e_0 = 0,
        e_at = 1,
        e_v0 = 2,
        e_v1 = 3,
        e_a0 = 4,
        e_a1 = 5,
        e_a2 = 6,
        e_a3 = 7,
        e_t0 = 8,
        e_t1 = 9,
        e_t2 = 10,
        e_t3 = 11,
        e_t4 = 12,
        e_t5 = 13,
        e_t6 = 14,
        e_t7 = 15,
        e_s0 = 16,
        e_s1 = 17,
        e_s2 = 18,
        e_s3 = 19,
        e_s4 = 20,
        e_s5 = 21,
        e_s6 = 22,
        e_s7 = 23,
        e_t8 = 24,
        e_t9 = 25,
        e_k0 = 26,
        e_k1 = 27,
        e_gp = 28,
        e_sp = 29,
        e_fp = 30,
        e_ra = 31,
    };
}
