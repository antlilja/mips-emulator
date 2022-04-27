#include "mips-emulator/instruction.hpp"
#include "mips-emulator/register_name.hpp"

#include <catch2/catch.hpp>

using namespace mips_emulator;

using Type = Instruction::Type;
using Func = Instruction::Func;
using IOp = Instruction::ITypeOpcode;
using JOp = Instruction::JTypeOpcode;

bool instr_type_matches(Instruction instr, Type type) {
    const auto instr_type = instr.get_type();
    return !instr_type.is_error() && instr_type.get_value() == type;
}

TEST_CASE("R-Type instruction", "[Instruction]") {

    SECTION("get_type") {
        Func funcs[] = {
            Func::e_add,   Func::e_addu,  Func::e_sub,   Func::e_subu,
            Func::e_sop30, Func::e_sop31, Func::e_sop32, Func::e_sop33,
            Func::e_and,   Func::e_nor,   Func::e_or,    Func::e_xor,
            Func::e_jr,    Func::e_jalr,  Func::e_slt,   Func::e_sltu,
            Func::e_sll,   Func::e_sllv,  Func::e_sra,   Func::e_srav,
            Func::e_srl,   Func::e_srlv,
        };

        for (auto const f : funcs) {

            const auto instr = Instruction(
                f, RegisterName::e_0, RegisterName::e_0, RegisterName::e_0);

            REQUIRE(instr_type_matches(instr, Instruction::Type::e_rtype));
        }
    }

    SECTION("add - zero registers") {
        const Instruction instr(Func::e_add, RegisterName::e_0,
                                RegisterName::e_0, RegisterName::e_0);

        REQUIRE(instr_type_matches(instr, Instruction::Type::e_rtype));
        REQUIRE(instr.raw == 0x20);
    }

    SECTION("add - non zero registers") {
        const Instruction instr(Func::e_add, RegisterName::e_t0,
                                RegisterName::e_t5, RegisterName::e_a0);

        REQUIRE(instr_type_matches(instr, Instruction::Type::e_rtype));
        REQUIRE(instr.raw == 0x01a44020);
    }

    SECTION("sll") {
        const Instruction instr(Func::e_sll, RegisterName::e_t0,
                                RegisterName::e_0, RegisterName::e_t1, 4);

        REQUIRE(instr.raw == 0x00094100);
    }

    SECTION("sllv") {
        const Instruction instr(Func::e_sllv, RegisterName::e_t0,
                                RegisterName::e_t2, RegisterName::e_t1);

        REQUIRE(instr.raw == 0x01494004);
    }

    SECTION("sra") {
        const Instruction instr(Func::e_sra, RegisterName::e_t0,
                                RegisterName::e_0, RegisterName::e_t1, 4);

        REQUIRE(instr.raw == 0x00094103);
    }

    SECTION("srav") {
        const Instruction instr(Func::e_srav, RegisterName::e_t0,
                                RegisterName::e_t2, RegisterName::e_t1);

        REQUIRE(instr.raw == 0x01494007);
    }

    SECTION("srl") {
        const Instruction instr(Func::e_srl, RegisterName::e_t0,
                                RegisterName::e_0, RegisterName::e_t1, 4);

        REQUIRE(instr.raw == 0x00094102);
    }

    SECTION("srlv") {
        const Instruction instr(Func::e_srlv, RegisterName::e_t0,
                                RegisterName::e_t2, RegisterName::e_t1);

        REQUIRE(instr.raw == 0x01494006);
    }
}

TEST_CASE("I-Type instruction", "[Instruction]") {

    SECTION("get_type") {
        IOp iops[] = {IOp::e_beq,  IOp::e_bne,   IOp::e_bgtz, IOp::e_blez,
                      IOp::e_addi, IOp::e_addiu, IOp::e_slti, IOp::e_sltiu,
                      IOp::e_andi, IOp::e_ori,   IOp::e_xori, IOp::e_lb,
                      IOp::e_lbu,  IOp::e_lw,    IOp::e_sb,   IOp::e_sw};

        for (auto const i : iops) {

            const auto instr_zero =
                Instruction(i, RegisterName::e_0, RegisterName::e_0, 0);

            REQUIRE(instr_type_matches(instr_zero, Instruction::Type::e_itype));

            const auto instr_non_zero =
                Instruction(i, RegisterName::e_t0, RegisterName::e_t5, 0xffff);

            REQUIRE(
                instr_type_matches(instr_non_zero, Instruction::Type::e_itype));
        }
    }

    SECTION("addi - zero registers and zero imm") {
        const Instruction instr(IOp::e_addi, RegisterName::e_0,
                                RegisterName::e_0, 0);

        REQUIRE(instr.raw == 0x20000000);
    }

    SECTION("addi - non zero registers and imm") {
        const Instruction instr(IOp::e_addi, RegisterName::e_t0,
                                RegisterName::e_t5, 0xffff);

        REQUIRE(instr.raw == 0x21a8ffff);
    }
}

TEST_CASE("J-Type instruction", "[Instruction]") {

    SECTION("get_type") {
        JOp jops[] = {JOp::e_j, JOp::e_jal};

        for (auto const j : jops) {
            const auto instr = Instruction(j, 0);

            REQUIRE(instr_type_matches(instr, Instruction::Type::e_jtype));
        }
    }
}

// FPU Instruction Tests
TEST_CASE("FPU R Type", "[Instruction]") {
    using Type = Instruction::Type;
    using R = Instruction::FPURTypeOp;
    using Func = Instruction::FPUFunc;

    SECTION("get_type") {
        R instr[] = {R::e_fmt_s, R::e_fmt_d,       R::e_fmt_w,
                     R::e_fmt_l, R::e_cmp_condn_s, R::e_cmp_condn_d};
        for (auto const v : instr) {
            const auto inst = Instruction(v, 0, 2, 3, Func::e_floor_l);
            REQUIRE(instr_type_matches(inst, Type::e_fpu_rtype));
        }
    }

    SECTION("ceil.w.d $f1, $f2") {
        Instruction r(R::e_fmt_d, 0, 2, 1, Func::e_ceil_w);
        REQUIRE(r.raw == 0x4620104eU);
    }

    SECTION("abs.s $f24, $f26") {
        Instruction r(R::e_fmt_s, 0, 26, 24, Func::e_abs);
        REQUIRE(r.raw == 0x4600d605U);
    }

    SECTION("add.d $f2, $f1, $f20") {
        Instruction r(R::e_fmt_d, 20, 1, 2, Func::e_add);
        REQUIRE(r.raw == 0x46340880);
    }
}

TEST_CASE("FPU T Type", "[Instruction]") {
    using Type = Instruction::Type;
    using T = Instruction::FPUTTypeOp;

    SECTION("get_type") {
        T instr[] = {T::e_cf, T::e_ct, T::e_mf, T::e_mfh, T::e_mt, T::e_mth};
        for (auto const v : instr) {
            const auto inst = Instruction(v, RegisterName::e_k0, 0);
            REQUIRE(instr_type_matches(inst, Type::e_fpu_ttype));
        }
    }

    SECTION("mfc1 $t1, $f8") {
        Instruction t(T::e_mf, RegisterName::e_t1, 8);
        REQUIRE(t.raw == 0x44094000U);
    }

    SECTION("mtc1 $t0, $f31") {
        Instruction t(T::e_mt, RegisterName::e_t0, 31);
        REQUIRE(t.raw == 0x4488f800U);
    }
}

TEST_CASE("FPU B Type", "[Instruction]") {
    using Type = Instruction::Type;
    using B = Instruction::FPUBTypeOp;

    SECTION("get_type") {
        B instr[] = {B::e_bc1eqz, B::e_bc1nez};
        for (auto const v : instr) {
            const auto inst = Instruction(v, 31, 25);
            REQUIRE(instr_type_matches(inst, Type::e_fpu_btype));
        }
    }
}

TEST_CASE("Special3 Type", "[Instruction]") {
    using Type = Instruction::Type;
    using Func = Instruction::Special3Func;
    using BSHFLFunc = Instruction::Special3BSHFLFunc;

    SECTION("get_type ext") {
        // EXT, rt, rs, msbd(rd), lsb(extra), func
        const auto inst = Instruction(Func::e_ext, 0, 1, RegisterName::e_t0,
                                      RegisterName::e_t1);
        REQUIRE(instr_type_matches(inst, Type::e_special3_type_ext));
    }

    // Note, the size parameter is stored as size-1 in the machine instruction
    SECTION("ext $t0 $t1 0 1") {
        Instruction inst(Func::e_ext, 0, 0, RegisterName::e_t1,
                         RegisterName::e_t0);
        REQUIRE(inst.raw == 0x7D280000);
    }

    SECTION("get_type ins") {
        // INS, rt, rs, msb(rd), lsb(extra), func
        const auto inst = Instruction(Func::e_ins, 0, 0, RegisterName::e_t0,
                                      RegisterName::e_t1);
        REQUIRE(instr_type_matches(inst, Type::e_special3_type_ins));
    }

    // Note, the size parameter is stored as size-1 in the machine instruction
    SECTION("ins $t0 $t1 0 1") {
        Instruction inst(Func::e_ins, 0, 0, RegisterName::e_t1,
                         RegisterName::e_t0);
        REQUIRE(inst.raw == 0x7D280004);
    }

    SECTION("get_type bshfl") {
        BSHFLFunc instr[] = {BSHFLFunc::e_bitswap, BSHFLFunc::e_wsbh,
                             BSHFLFunc::e_seb, BSHFLFunc::e_seh};

        for (auto const v : instr) {
            const auto inst = Instruction(Func::e_bshfl, v, RegisterName::e_t0,
                                          RegisterName::e_t1);
            REQUIRE(instr_type_matches(inst, Type::e_special3_type_bshfl));
        }
    }

    SECTION("bitswap $t0 $t1") {
        Instruction t(Func::e_bshfl, BSHFLFunc::e_bitswap, RegisterName::e_t0,
                      RegisterName::e_t1);
        REQUIRE(t.raw == 0x7C094020);
    }

    SECTION("wsbh $t0 $t1") {
        Instruction t(Func::e_bshfl, BSHFLFunc::e_wsbh, RegisterName::e_t0,
                      RegisterName::e_t1);
        REQUIRE(t.raw == 0x7c0940a0);
    }

    SECTION("seb $t0 $t1") {
        Instruction t(Func::e_bshfl, BSHFLFunc::e_seb, RegisterName::e_t0,
                      RegisterName::e_t1);
        REQUIRE(t.raw == 0x7c094420);
    }

    SECTION("seh $t0 $t1") {
        Instruction t(Func::e_bshfl, BSHFLFunc::e_seh, RegisterName::e_t0,
                      RegisterName::e_t1);
        REQUIRE(t.raw == 0x7c094620);
    }

    SECTION("get_type align") {
        Instruction t(Func::e_bshfl, BSHFLFunc::e_align_1, RegisterName::e_t0,
                      RegisterName::e_t2, RegisterName::e_t1);
        REQUIRE(instr_type_matches(t, Type::e_special3_type_bshfl));
    }

    SECTION("align $t0 $t1 $t2 1") {
        Instruction t(Func::e_bshfl, BSHFLFunc::e_align_1, RegisterName::e_t0,
                      RegisterName::e_t1, RegisterName::e_t2);
        REQUIRE(t.raw == 0x7d2a4260);
    }
}

TEST_CASE("Regimm I Type", "[Instruction]") {
    using Type = Instruction::Type;
    using IOp = Instruction::RegimmITypeOp;

    SECTION("get_type") {
        IOp instr[] = {IOp::e_bgez, IOp::e_bltz};

        for (auto const v : instr) {
            const auto inst = Instruction(v, RegisterName::e_t0, 3);
            REQUIRE(instr_type_matches(inst, Type::e_regimm_itype));
        }
    }

    SECTION("bgez - zero register and positive immediate ") {
        Instruction t(IOp::e_bgez, RegisterName::e_0, 12);
        REQUIRE(t.raw == 0x401000c);
    }

    SECTION("bltz - positive register and immediate") {
        Instruction t(IOp::e_bltz, RegisterName::e_t1, 8);
        REQUIRE(t.raw == 0x5200008);
    }
}
