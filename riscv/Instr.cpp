#include "Instr.h"
#include "Cpu.h"
#include "TraceCtl.h"

extern TraceCtl trace;

INSTR_DEF(LUI);
INSTR_DEF(AUIPC);
INSTR_DEF(JAL);
INSTR_DEF(JALR);

INSTR_DEF(BEQ);
INSTR_DEF(BNE);
INSTR_DEF(BLT);
INSTR_DEF(BGE);
INSTR_DEF(BLTU);
INSTR_DEF(BGEU);

INSTR_DEF(LB);
INSTR_DEF(LH);
INSTR_DEF(LW);
INSTR_DEF(LBU);
INSTR_DEF(LHU);

INSTR_DEF(SB);
INSTR_DEF(SH);
INSTR_DEF(SW);

INSTR_DEF(ADDI);
INSTR_DEF(SLTI);
INSTR_DEF(SLTIU);
INSTR_DEF(XORI);
INSTR_DEF(ORI);
INSTR_DEF(ANDI);
INSTR_DEF(SLLI);
INSTR_DEF(SRLI);
INSTR_DEF(SRAI);

INSTR_DEF(ADD);
INSTR_DEF(SUB);
INSTR_DEF(SLL);
INSTR_DEF(SLT);
INSTR_DEF(SLTU);
INSTR_DEF(XOR);

INSTR_DEF(SRL);
INSTR_DEF(SRA);
INSTR_DEF(OR);
INSTR_DEF(AND);

INSTR_DEF(CSRRW);
INSTR_DEF(CSRRS);
INSTR_DEF(CSRRC);
INSTR_DEF(CSRRWI);
INSTR_DEF(CSRRSI);
INSTR_DEF(CSRRCI);

INSTR_DEF(FENCE);
INSTR_DEF(FENCEI);
INSTR_DEF(ECALL);
INSTR_DEF(EBREAK);
INSTR_DEF(E73);

// Base class
void Instr::execute(Cpu *cpu, uint32_t instr) {
    cpu->incr_pc();
}

void Instr::execute_noincr(Cpu *cpu, uint32_t instr) {
}

// ------------------------------------------------------

void Instr_LUI::execute(Cpu *cpu, uint32_t instr) {
    uint32_t imm = instr & 0xfffff000;
    int rd = (instr >> 7) & 0x1f;
    if (trace.idebug()) printf(" instr_LUI x%d <- %08x\n", rd, imm);
    cpu->set_reg(rd, imm);
    Instr::execute(cpu, instr);
};

// add upper immediate to pc
void Instr_AUIPC::execute(Cpu *cpu, uint32_t instr) {
    uint32_t imm = instr & 0xfffff000;
    int rd = (instr >> 7) & 0x1f;
    uint32_t v = cpu->pc + imm;
    if (trace.idebug()) printf(" instr_AUIPC x%d <- %08x\n", rd, v);
    cpu->set_reg(rd, v);
    Instr::execute(cpu, instr);
};

uint32_t sign_extend_8(uint32_t v8) {
    if (v8 & 0x00000080) v8 |= 0xffffff80;
    return v8;
}

uint32_t sign_extend_12(uint32_t v12) {
    if (v12 & 0x00000800) v12 |= 0xfffff800;
    return v12;
}

uint32_t sign_extend_16(uint32_t v16) {
    if (v16 & 0x00008000) v16 |= 0xffff8000;
    return v16;
}

uint32_t sign_extend_20(uint32_t v20) {
    if (v20 & (1 << 20)) v20 |= 0xfff00000;
    return v20;
}

void Instr_JAL::execute(Cpu *cpu, uint32_t instr) {
    uint32_t imm20 = (instr >> 31) & 1;
    uint32_t imm10 = (instr >> 21) & 0x000003ff;
    uint32_t imm11 = (instr >> 20) & 1;
    uint32_t imm19 = (instr >> 12) & 0x000000ff;
    int rd = (instr >> 7) & 0x0000001f;
    uint32_t imm = (imm20 << 20) | (imm19 << 12) | (imm11 << 11) | (imm10 << 1);
    int offset = sign_extend_20(imm);
    uint32_t v = (int)cpu->pc + offset;
    if (trace.idebug()) printf(" instr_JAL pc <- %08x; pc %c %08x\n", v, offset < 0 ? '-' : '+', offset);
    cpu->set_reg(rd, cpu->pc+4);
    cpu->pc = v;
    Instr::execute_noincr(cpu, instr);
};

void Instr_JALR::execute(Cpu *cpu, uint32_t instr) {
    uint32_t imm = (instr >> 20) & 0x00000fff;
    int rs1      = (instr >> 15) & 0x0000001f;
    int rd       = (instr >>  7) & 0x0000001f;
    uint32_t rrs1 = cpu->get_reg(rs1);
    uint32_t offset = sign_extend_12(imm);
    uint32_t v = (int)rrs1 + offset;
    v &= ~1;
    if (trace.idebug()) printf(" instr_JALR PC <- %08x; x%d(%08x) %c %08x \n", v, rs1, rrs1, offset < 0 ? '-' : '+', offset);
    cpu->set_reg(rd, cpu->pc+4);
    cpu->pc = v;
    Instr::execute_noincr(cpu, instr);
};

void Instr_BEQ::execute(Cpu *cpu, uint32_t instr) {
    int imm12    = (instr >> 31) & 1;
    int imm10    = (instr >> 25) & 0x0000003f;
    int rs2      = (instr >> 20) & 0x0000001f;
    int rs1      = (instr >> 15) & 0x0000001f;
    int imm4     = (instr >>  8) & 0x0000000f;
    int imm11    = (instr >>  7) & 1;
    uint32_t rrs1 = cpu->get_reg(rs1);
    uint32_t rrs2 = cpu->get_reg(rs2);
    uint32_t imm  = (imm12 << 12) | (imm11 << 11) | (imm10 << 5) | (imm4 << 1);
    int simm = sign_extend_12(imm);
    uint32_t addr = (int)cpu->pc + simm;
    int jump = rrs1 == rrs2 ? 1 : 0;
    if (trace.idebug()) printf(" instr_BEQ %08x %d; x%d(%08x) == x%d(%08x)\n", addr, jump, rs1, rrs1, rs2, rrs2);
    if (jump) {
        cpu->pc = addr;
        Instr::execute_noincr(cpu, instr);
    } else {
        Instr::execute(cpu, instr);
    }
};

void Instr_BNE::execute(Cpu *cpu, uint32_t instr) {
    int imm12    = (instr >> 31) & 1;
    int imm10    = (instr >> 25) & 0x0000003f;
    int rs2      = (instr >> 20) & 0x0000001f;
    int rs1      = (instr >> 15) & 0x0000001f;
    int imm4     = (instr >>  8) & 0x0000000f;
    int imm11    = (instr >>  7) & 1;
    uint32_t rrs1 = cpu->get_reg(rs1);
    uint32_t rrs2 = cpu->get_reg(rs2);
    uint32_t imm  = (imm12 << 12) | (imm11 << 11) | (imm10 << 5) | (imm4 << 1);
    int simm = sign_extend_12(imm);
    uint32_t addr = (int)cpu->pc + simm;
    int jump = rrs1 != rrs2 ? 1 : 0;
    if (trace.idebug()) printf(" instr_BNE %08x %d; x%d(%08x) == x%d(%08x)\n", addr, jump, rs1, rrs1, rs2, rrs2);
    if (jump) {
        cpu->pc = addr;
        Instr::execute_noincr(cpu, instr);
    } else {
        Instr::execute(cpu, instr);
    }
};

void Instr_BLT::execute(Cpu *cpu, uint32_t instr) {
    int imm12    = (instr >> 31) & 1;
    int imm10    = (instr >> 25) & 0x0000003f;
    int rs2      = (instr >> 20) & 0x0000001f;
    int rs1      = (instr >> 15) & 0x0000001f;
    int imm4     = (instr >>  8) & 0x0000000f;
    int imm11    = (instr >>  7) & 1;
    uint32_t rrs1 = cpu->get_reg(rs1);
    uint32_t rrs2 = cpu->get_reg(rs2);
    uint32_t imm  = (imm12 << 12) | (imm11 << 11) | (imm10 << 5) | (imm4 << 1);
    int simm = sign_extend_12(imm);
    uint32_t addr = (int)cpu->pc + simm;
    int jump = (int)rrs1 < (int)rrs2 ? 1 : 0;
    if (trace.idebug()) printf(" instr_BLT %08x %d; x%d(%08x) < x%d(%08x)\n", addr, jump, rs1, rrs1, rs2, rrs2);
    if (jump) {
        cpu->pc = addr;
        Instr::execute_noincr(cpu, instr);
    } else {
        Instr::execute(cpu, instr);
    }
};

void Instr_BGE::execute(Cpu *cpu, uint32_t instr) {
    int imm12    = (instr >> 31) & 1;
    int imm10    = (instr >> 25) & 0x0000003f;
    int rs2      = (instr >> 20) & 0x0000001f;
    int rs1      = (instr >> 15) & 0x0000001f;
    int imm4     = (instr >>  8) & 0x0000000f;
    int imm11    = (instr >>  7) & 1;
    uint32_t rrs1 = cpu->get_reg(rs1);
    uint32_t rrs2 = cpu->get_reg(rs2);
    uint32_t imm  = (imm12 << 12) | (imm11 << 11) | (imm10 << 5) | (imm4 << 1);
    int simm = sign_extend_12(imm);
    uint32_t addr = (int)cpu->pc + simm;
    int jump = (int)rrs1 >= (int)rrs2 ? 1 : 0;
    if (trace.idebug()) printf(" instr_BGE %08x %d; (%d)%08x >= (%d)%08x\n", addr, jump, rs1, rrs1, rs2, rrs2);
    if (jump) {
        cpu->pc = addr;
        Instr::execute_noincr(cpu, instr);
    } else {
        Instr::execute(cpu, instr);
    }
};

void Instr_BLTU::execute(Cpu *cpu, uint32_t instr) {
    int imm12    = (instr >> 31) & 1;
    int imm10    = (instr >> 25) & 0x0000003f;
    int rs2      = (instr >> 20) & 0x0000001f;
    int rs1      = (instr >> 15) & 0x0000001f;
    int imm4     = (instr >>  8) & 0x0000000f;
    int imm11    = (instr >>  7) & 1;
    uint32_t rrs1 = cpu->get_reg(rs1);
    uint32_t rrs2 = cpu->get_reg(rs2);
    uint32_t imm  = (imm12 << 12) | (imm11 << 11) | (imm10 << 5) | (imm4 << 1);
    int simm = sign_extend_12(imm);
    uint32_t addr = (int)cpu->pc + simm;
    int jump = rrs1 < rrs2 ? 1 : 0;
    if (trace.idebug()) printf(" instr_BLTU %08x %d; x%d(%08x) < x%d(%08x)\n", addr, jump, rs1, rrs1, rs2, rrs2);
    if (jump) {
        cpu->pc = addr;
        Instr::execute_noincr(cpu, instr);
    } else {
        Instr::execute(cpu, instr);
    }
};

void Instr_BGEU::execute(Cpu *cpu, uint32_t instr) {
    int imm12    = (instr >> 31) & 1;
    int imm10    = (instr >> 25) & 0x0000003f;
    int rs2      = (instr >> 20) & 0x0000001f;
    int rs1      = (instr >> 15) & 0x0000001f;
    int imm4     = (instr >>  8) & 0x0000000f;
    int imm11    = (instr >>  7) & 1;
    uint32_t rrs1 = cpu->get_reg(rs1);
    uint32_t rrs2 = cpu->get_reg(rs2);
    uint32_t imm  = (imm12 << 12) | (imm11 << 11) | (imm10 << 5) | (imm4 << 1);
    int simm = sign_extend_12(imm);
    uint32_t addr = (int)cpu->pc + simm;
    int jump = rrs1 >= rrs2 ? 1 : 0;
    if (trace.idebug()) printf(" instr_BGEU %08x %d; x%d(%08x) < x%d(%08x)\n", addr, jump, rs1, rrs1, rs2, rrs2);
    if (jump) {
        cpu->pc = addr;
        Instr::execute_noincr(cpu, instr);
    } else {
        Instr::execute(cpu, instr);
    }
};

void Instr_LB::execute(Cpu *cpu, uint32_t instr) {
    int imm  = (instr >> 20) & 0x00000fff;
    int rs1  = (instr >> 15) & 0x0000001f;
    int rd   = (instr >>  7) & 0x0000001f;
    uint32_t rrs1 = cpu->get_reg(rs1);
    uint32_t simm = sign_extend_12(imm);
    uint32_t addr = (int)rrs1 + simm;
    uint32_t v = sign_extend_8(cpu->read8(addr));
    if (trace.idebug()) printf(" instr_LB %d <- %08x; %08x + %08x\n", rd, v, rrs1, simm);
    cpu->set_reg(rd, v);
    Instr::execute(cpu, instr);
};

void Instr_LH::execute(Cpu *cpu, uint32_t instr) {
    int imm  = (instr >> 20) & 0x00000fff;
    int rs1  = (instr >> 15) & 0x0000001f;
    int rd   = (instr >>  7) & 0x0000001f;
    uint32_t rrs1 = cpu->get_reg(rs1);
    uint32_t simm = sign_extend_12(imm);
    uint32_t addr = (int)rrs1 + simm;
    uint32_t v = sign_extend_16(cpu->read16(addr));
    if (trace.idebug()) printf(" instr_LH %d <- %08x; %08x + %08x\n", rd, v, rrs1, simm);
    cpu->set_reg(rd, v);
    Instr::execute(cpu, instr);
};

void Instr_LW::execute(Cpu *cpu, uint32_t instr) {
    int imm  = (instr >> 20) & 0x00000fff;
    int rs1  = (instr >> 15) & 0x0000001f;
    int rd   = (instr >>  7) & 0x0000001f;
    uint32_t rrs1 = cpu->get_reg(rs1);
    int simm = sign_extend_12(imm);
    uint32_t addr = (int)rrs1 + simm;
    uint32_t v = cpu->read32(addr);
    if (trace.idebug()) printf(" instr_LW %d <- %08x; [%08x] %08x + %08x\n", rd, v, addr, rrs1, simm);
    cpu->set_reg(rd, v);
    Instr::execute(cpu, instr);
};

void Instr_LBU::execute(Cpu *cpu, uint32_t instr) {
    int imm  = (instr >> 20) & 0x00000fff;
    int rs1  = (instr >> 15) & 0x0000001f;
    int rd   = (instr >>  7) & 0x0000001f;
    uint32_t rrs1 = cpu->get_reg(rs1);
    int simm = sign_extend_12(imm);
    uint32_t addr = (int)rrs1 + simm;
    uint32_t v = cpu->read8(addr);
    if (trace.idebug()) printf(" instr_LBU %d <- %08x; x%d(%08x + %08x\n", rd, v, rs1, rrs1, simm);
    cpu->set_reg(rd, v);
    Instr::execute(cpu, instr);
};

void Instr_LHU::execute(Cpu *cpu, uint32_t instr) {
    int imm  = (instr >> 20) & 0x00000fff;
    int rs1  = (instr >> 15) & 0x0000001f;
    int rd   = (instr >>  7) & 0x0000001f;
    uint32_t rrs1 = cpu->get_reg(rs1);
    uint32_t simm = sign_extend_12(imm);
    uint32_t addr = (int)rrs1 + simm;
    uint32_t v = cpu->read16(addr);
    if (trace.idebug()) printf(" instr_LHU %d <- %08x; %08x + %08x\n", rd, v, rrs1, simm);
    cpu->set_reg(rd, v);
    Instr::execute(cpu, instr);
};

void Instr_SB::execute(Cpu *cpu, uint32_t instr) {
    int immed5   = (instr >> 25) & 0x0000007f;
    int rs2      = (instr >> 20) & 0x0000001f;
    int rs1      = (instr >> 15) & 0x0000001f;
    int immed4   = (instr >>  7) & 0x0000001f;
    uint32_t rrs1 = cpu->get_reg(rs1);
    uint32_t rrs2 = cpu->get_reg(rs2);
    uint32_t imm  = (immed5 << 5) | immed4;
    uint32_t simm = sign_extend_12(imm);
    uint32_t addr = (int)rrs1 + simm;
    if (trace.idebug()) printf(" instr_SB %08x <- %08x; %08x + %08x\n", addr, rrs2, rrs1, simm);
    cpu->write8(addr, rrs2);
    Instr::execute(cpu, instr);
};

void Instr_SH::execute(Cpu *cpu, uint32_t instr) {
    int immed5   = (instr >> 25) & 0x0000007f;
    int rs2      = (instr >> 20) & 0x0000001f;
    int rs1      = (instr >> 15) & 0x0000001f;
    int immed4   = (instr >>  7) & 0x0000001f;
    uint32_t rrs1 = cpu->get_reg(rs1);
    uint32_t rrs2 = cpu->get_reg(rs2);
    uint32_t imm  = (immed5 << 5) | immed4;
    uint32_t simm = sign_extend_12(imm);
    uint32_t addr = (int)rrs1 + simm;
    if (trace.idebug()) printf(" instr_SH %08x <- %08x; %08x + %08x\n", addr, rrs2, rrs1, simm);
    cpu->write16(addr, rrs2);
    Instr::execute(cpu, instr);
};

void Instr_SW::execute(Cpu *cpu, uint32_t instr) {
    int immed5   = (instr >> 25) & 0x0000007f;
    int rs2      = (instr >> 20) & 0x0000001f;
    int rs1      = (instr >> 15) & 0x0000001f;
    int immed4   = (instr >>  7) & 0x0000001f;
    uint32_t rrs1 = cpu->get_reg(rs1);
    uint32_t rrs2 = cpu->get_reg(rs2);
    uint32_t imm  = (immed5 << 5) | immed4;
    uint32_t simm = sign_extend_12(imm);
    uint32_t addr = (int)rrs1 + simm;
    if (trace.idebug()) printf(" instr_SW %08x <- %08x; %08x + %08x\n", addr, rrs2, rrs1, simm);
    cpu->write32(addr, rrs2);
    Instr::execute(cpu, instr);
};

void Instr_ADDI::execute(Cpu *cpu, uint32_t instr) {
    uint32_t imm = (instr >> 20) & 0x00000fff;
    int rs1      = (instr >> 15) & 0x0000001f;
    int rd       = (instr >>  7) & 0x0000001f;
    uint32_t rrs1 = cpu->get_reg(rs1);
    uint32_t simm = sign_extend_12(imm);
    uint32_t v = rrs1 + simm;
    if (trace.idebug()) printf(" instr_ADDI x%d <- %08x; %d(%08x) + %08x\n", rd, v, rs1, rrs1, simm);
    cpu->set_reg(rd, v);
    Instr::execute(cpu, instr);
};

void Instr_SLTI::execute(Cpu *cpu, uint32_t instr) {
    uint32_t imm = (instr >> 20) & 0x00000fff;
    int rs1      = (instr >> 15) & 0x0000001f;
    int rd       = (instr >>  7) & 0x0000001f;
    uint32_t rrs1 = cpu->get_reg(rs1);
    uint32_t simm = sign_extend_12(imm);
    uint32_t v = (int)rrs1 < (int)simm ? 1 : 0;
    if (trace.idebug()) printf(" instr_SLTI %d <- %d; %08x < %08x\n", rd, v, rs1, simm);
    cpu->set_reg(rd, v);
    Instr::execute(cpu, instr);
};

void Instr_SLTIU::execute(Cpu *cpu, uint32_t instr) {
    uint32_t imm = (instr >> 20) & 0x00000fff;
    int rs1      = (instr >> 15) & 0x0000001f;
    int rd       = (instr >>  7) & 0x0000001f;
    uint32_t rrs1 = cpu->get_reg(rs1);
    uint32_t simm = sign_extend_12(imm);
    uint32_t v = rrs1 < simm ? 1 : 0;
    if (trace.idebug()) printf(" instr_SLTIU %d <- %d; %08x < %08x\n", rd, v, rs1, simm);
    cpu->set_reg(rd, v);
    Instr::execute(cpu, instr);
};

void Instr_XORI::execute(Cpu *cpu, uint32_t instr) {
    uint32_t imm = (instr >> 20) & 0x00000fff;
    int rs1      = (instr >> 15) & 0x0000001f;
    int rd       = (instr >>  7) & 0x0000001f;
    uint32_t rrs1 = cpu->get_reg(rs1);
    uint32_t simm = sign_extend_12(imm);
    uint32_t v = rrs1 ^ simm;
    if (trace.idebug()) printf(" instr_XORI %d <- %08x; %d ^ %08x\n", rd, v, rs1, simm);
    cpu->set_reg(rd, v);
    Instr::execute(cpu, instr);
};

void Instr_ORI::execute(Cpu *cpu, uint32_t instr) {
    uint32_t imm = (instr >> 20) & 0x00000fff;
    int rs1      = (instr >> 15) & 0x0000001f;
    int rd       = (instr >>  7) & 0x0000001f;
    uint32_t rrs1 = cpu->get_reg(rs1);
    uint32_t simm = sign_extend_12(imm);
    uint32_t v = rrs1 | simm;
    if (trace.idebug()) printf(" instr_ORI %d <- %08x; %d | %08x\n", rd, v, rs1, simm);
    cpu->set_reg(rd, v);
    Instr::execute(cpu, instr);
};

void Instr_ANDI::execute(Cpu *cpu, uint32_t instr) {
    uint32_t imm = (instr >> 20) & 0x00000fff;
    int rs1      = (instr >> 15) & 0x0000001f;
    int rd       = (instr >>  7) & 0x0000001f;
    uint32_t rrs1 = cpu->get_reg(rs1);
    uint32_t simm = sign_extend_12(imm);
    uint32_t v = rrs1 & simm;
    if (trace.idebug()) printf(" instr_ANDI %d <- %08x; %d & %08x\n", rd, v, rs1, simm);
    cpu->set_reg(rd, v);
    Instr::execute(cpu, instr);
};

void Instr_SLLI::execute(Cpu *cpu, uint32_t instr) {
    uint32_t imm4  = (instr >> 20) & 0x0000001f;    
    int rs1        = (instr >> 15) & 0x0000001f;
    int rd         = (instr >>  7) & 0x0000001f;
    uint32_t rrs1 = cpu->get_reg(rs1);
    uint32_t v = rrs1 << imm4;
    if (trace.idebug()) printf(" instr_SLLI %d <- %08x; x%d(%08x) << %d\n", rd, v, rs1, rrs1, imm4);
    cpu->set_reg(rd, v);
    Instr::execute(cpu, instr);
};

void Instr_SRLI::execute(Cpu *cpu, uint32_t instr) {
    uint32_t imm4  = (instr >> 20) & 0x0000001f;    
    int rs1        = (instr >> 15) & 0x0000001f;
    int rd         = (instr >>  7) & 0x0000001f;
    uint32_t rrs1 = cpu->get_reg(rs1);
    uint32_t v = rrs1 >> imm4;
    if (trace.idebug()) printf(" instr_SRLI %d <- %08x; x%d(%08x) >> %d\n", rd, v, rs1, rrs1, imm4);
    cpu->set_reg(rd, v);
    Instr::execute(cpu, instr);
};

void Instr_SRAI::execute(Cpu *cpu, uint32_t instr) {
    uint32_t imm4  = (instr >> 20) & 0x0000001f;    
    int rs1        = (instr >> 15) & 0x0000001f;
    int rd         = (instr >>  7) & 0x0000001f;
    uint32_t rrs1 = cpu->get_reg(rs1);
    //uint32_t v = rrs1 >> imm4;
    uint32_t v = rrs1;
    for (int s = 0; s < imm4; s++) {
        v >>= 1;
        if (v & 0x40000000) v |= 0x80000000;
    }
    if (trace.idebug()) printf(" instr_SRAI %d <- %08x; x%d(%08x) >> %d\n", rd, v, rs1, rrs1, imm4);
    cpu->set_reg(rd, v);
    Instr::execute(cpu, instr);
};

void Instr_ADD::execute(Cpu *cpu, uint32_t instr) {
    int rs2      = (instr >> 20) & 0x0000001f;
    int rs1      = (instr >> 15) & 0x0000001f;
    int rd       = (instr >>  7) & 0x0000001f;
    uint32_t rrs1 = cpu->get_reg(rs1);
    uint32_t rrs2 = cpu->get_reg(rs2);
    uint32_t v = rrs1 + rrs2;
    if (trace.idebug()) printf(" instr_ADD %d <- %08x; %d(%08x) + %d(%08x)\n", rd, v, rs1, rrs1, rs2, rrs2);
    cpu->set_reg(rd, v);
    Instr::execute(cpu, instr);
};

void Instr_SUB::execute(Cpu *cpu, uint32_t instr) {
    int rs2      = (instr >> 20) & 0x0000001f;
    int rs1      = (instr >> 15) & 0x0000001f;
    int rd       = (instr >>  7) & 0x0000001f;
    uint32_t rrs1 = cpu->get_reg(rs1);
    uint32_t rrs2 = cpu->get_reg(rs2);
    uint32_t v = rrs1 - rrs2;
    if (trace.idebug()) printf(" instr_SUB %d <- %08x; %d(%08x) - %d(%08x)\n", rd, v, rs1, rrs1, rs2, rrs2);
    cpu->set_reg(rd, v);
    Instr::execute(cpu, instr);
};

void Instr_SLL::execute(Cpu *cpu, uint32_t instr) {
    int rs2      = (instr >> 20) & 0x0000001f;
    int rs1      = (instr >> 15) & 0x0000001f;
    int rd       = (instr >>  7) & 0x0000001f;
    uint32_t rrs1 = cpu->get_reg(rs1);
    uint32_t rrs2 = cpu->get_reg(rs2) & 0x0000001f;
    uint32_t v = rrs1 << rrs2;
    if (trace.idebug()) printf(" instr_SLL %d <- %08x; %d(%08x) << %d(%08x)\n", rd, v, rs1, rrs1, rs2, rrs2);
    cpu->set_reg(rd, v);
    Instr::execute(cpu, instr);
};

void Instr_SLT::execute(Cpu *cpu, uint32_t instr) {
    int rs2      = (instr >> 20) & 0x0000001f;
    int rs1      = (instr >> 15) & 0x0000001f;
    int rd       = (instr >>  7) & 0x0000001f;
    uint32_t rrs1 = cpu->get_reg(rs1);
    uint32_t rrs2 = cpu->get_reg(rs2);
    uint32_t v = (int)rrs1 < (int)rrs2 ? 1 : 0;
    if (trace.idebug()) printf(" instr_SLT %d <- %08x; %d(%08x) < %d(%08x)\n", rd, v, rs1, rrs1, rs2, rrs2);
    cpu->set_reg(rd, v);
    Instr::execute(cpu, instr);
};

void Instr_SLTU::execute(Cpu *cpu, uint32_t instr) {
    int rs2      = (instr >> 20) & 0x0000001f;
    int rs1      = (instr >> 15) & 0x0000001f;
    int rd       = (instr >>  7) & 0x0000001f;
    uint32_t rrs1 = cpu->get_reg(rs1);
    uint32_t rrs2 = cpu->get_reg(rs2);
    uint32_t v = rrs1 < rrs2 ? 1 : 0;
    if (trace.idebug()) printf(" instr_SLTU %d <- %08x; %d(%08x) < %d(%08x)\n", rd, v, rs1, rrs1, rs2, rrs2);
    cpu->set_reg(rd, v);
    Instr::execute(cpu, instr);
};

void Instr_XOR::execute(Cpu *cpu, uint32_t instr) {
    int rs2      = (instr >> 20) & 0x0000001f;
    int rs1      = (instr >> 15) & 0x0000001f;
    int rd       = (instr >>  7) & 0x0000001f;
    uint32_t rrs1 = cpu->get_reg(rs1);
    uint32_t rrs2 = cpu->get_reg(rs2);
    uint32_t v = rrs1 ^ rrs2;
    if (trace.idebug()) printf(" instr_XOR %d <- %08x; %d(%08x) ^ %d(%08x)\n", rd, v, rs1, rrs1, rs2, rrs2);
    cpu->set_reg(rd, v);
    Instr::execute(cpu, instr);
};


void Instr_SRL::execute(Cpu *cpu, uint32_t instr) {
    int rs2      = (instr >> 20) & 0x0000001f;
    int rs1      = (instr >> 15) & 0x0000001f;
    int rd       = (instr >>  7) & 0x0000001f;
    uint32_t rrs1 = cpu->get_reg(rs1);
    uint32_t rrs2 = cpu->get_reg(rs2) & 0x0000001f;
    uint32_t v = rrs1 >> rrs2;
    if (trace.idebug()) printf(" instr_SRL %d <- %08x; %d(%08x) >> %d(%08x)\n", rd, v, rs1, rrs1, rs2, rrs2);
    cpu->set_reg(rd, v);
    Instr::execute(cpu, instr);
};

void Instr_SRA::execute(Cpu *cpu, uint32_t instr) {
    int rs2      = (instr >> 20) & 0x0000001f;
    int rs1      = (instr >> 15) & 0x0000001f;
    int rd       = (instr >>  7) & 0x0000001f;
    uint32_t rrs1 = cpu->get_reg(rs1);
    uint32_t rrs2 = cpu->get_reg(rs2) & 0x0000001f;
    //uint32_t v = rrs1 >> rrs2;
    uint32_t v = rrs1;
    for (int s = 0; s < rrs2; s++) {
        v >>= 1;
        if (v & 0x40000000) v |= 0x80000000;
    }
    if (trace.idebug()) printf(" instr_SRA %d <- %08x; %d(%08x) >> %d(%08x)\n", rd, v, rs1, rrs1, rs2, rrs2);
    cpu->set_reg(rd, v);
    Instr::execute(cpu, instr);
};

void Instr_OR::execute(Cpu *cpu, uint32_t instr) {
    int rs2      = (instr >> 20) & 0x0000001f;
    int rs1      = (instr >> 15) & 0x0000001f;
    int rd       = (instr >>  7) & 0x0000001f;
    uint32_t rrs1 = cpu->get_reg(rs1);
    uint32_t rrs2 = cpu->get_reg(rs2);
    uint32_t v = rrs1 | rrs2;
    if (trace.idebug()) printf(" instr_OR %d <- %08x; %d(%08x) | %d(%08x)\n", rd, v, rs1, rrs1, rs2, rrs2);
    cpu->set_reg(rd, v);
    Instr::execute(cpu, instr);
};

void Instr_AND::execute(Cpu *cpu, uint32_t instr) {
    int rs2      = (instr >> 20) & 0x0000001f;
    int rs1      = (instr >> 15) & 0x0000001f;
    int rd       = (instr >>  7) & 0x0000001f;
    uint32_t rrs1 = cpu->get_reg(rs1);
    uint32_t rrs2 = cpu->get_reg(rs2);
    uint32_t v = rrs1 & rrs2;
    if (trace.idebug()) printf(" instr_OR %d <- %08x; %d(%08x) & %d(%08x)\n", rd, v, rs1, rrs1, rs2, rrs2);
    cpu->set_reg(rd, v);
    Instr::execute(cpu, instr);
};

void Instr_FENCE::execute(Cpu *cpu, uint32_t instr) {
}

void Instr_FENCEI::execute(Cpu *cpu, uint32_t instr) {
}

void Instr_ECALL::execute(Cpu *cpu, uint32_t instr) {
}

void Instr_EBREAK::execute(Cpu *cpu, uint32_t instr) {
}

void Instr_E73::execute(Cpu *cpu, uint32_t instr) {
    uint32_t imm11  = (instr >>20) & 0x00000fff;
    unsigned rs1    = (instr >>15) & 0x0000001f;
    unsigned funct3 = (instr >>12) & 0x00000007;
    int rd          = (instr >> 7) & 0x0000001f;

    if (rd == 0 && funct3 == 0 && rs1 == 0 && imm11 == 0)
        instr_ECALL.execute(cpu, instr);
    else
        if (rd == 0 && funct3 == 0 && rs1 == 0 && imm11 == 0x001)
            instr_EBREAK.execute(cpu, instr);
}

void Instr_CSRRW::execute(Cpu *cpu, uint32_t instr) {
    printf(" instr_CSRRW\n");
    Instr::execute(cpu, instr);
};

void Instr_CSRRS::execute(Cpu *cpu, uint32_t instr) {
    printf(" instr_CSRRS\n");
    Instr::execute(cpu, instr);
};

void Instr_CSRRC::execute(Cpu *cpu, uint32_t instr) {
    printf(" instr_CSRRC\n");
    Instr::execute(cpu, instr);
};

void Instr_CSRRWI::execute(Cpu *cpu, uint32_t instr) {
    printf(" instr_CSRRWI\n");
    Instr::execute(cpu, instr);
};

void Instr_CSRRSI::execute(Cpu *cpu, uint32_t instr) {
    printf(" instr_CSRRSI\n");
    Instr::execute(cpu, instr);
};

void Instr_CSRRCI::execute(Cpu *cpu, uint32_t instr) {
    printf(" instr_CSRRCI\n");
    Instr::execute(cpu, instr);
};

/* Local Variables:      */
/* mode: c++             */
/* indent-tabs-mode: nil */
/* tab-width: 4          */
/* c-basic-offset: 4     */
/* End:                  */
