#pragma once

#include <stdio.h>
#include <stdint.h>

class Cpu;

class Instr {
public:
    Instr() = default;
    virtual void execute(Cpu *cpu, uint32_t instr) = 0;
    virtual void execute_noincr(Cpu *cpu, uint32_t instr) = 0;
};

#define INSTR_CLASS(name) \
class Instr_##name : public Instr { \
public: \
    void execute(Cpu *cpu, uint32_t instr); \
    void execute_noincr(Cpu *cpu, uint32_t instr) {}  \
}; \
extern Instr_##name instr_##name;

#define INSTR_DEF(name) \
Instr_##name instr_##name;

INSTR_CLASS(LUI)
INSTR_CLASS(AUIPC);
INSTR_CLASS(JAL);
INSTR_CLASS(JALR);

INSTR_CLASS(BEQ);
INSTR_CLASS(BNE);
INSTR_CLASS(BLT);
INSTR_CLASS(BGE);
INSTR_CLASS(BLTU);
INSTR_CLASS(BGEU);

INSTR_CLASS(LB);
INSTR_CLASS(LH);
INSTR_CLASS(LW);
INSTR_CLASS(LBU);
INSTR_CLASS(LHU);

INSTR_CLASS(SB);
INSTR_CLASS(SH);
INSTR_CLASS(SW);

INSTR_CLASS(ADDI);
INSTR_CLASS(SLTI);
INSTR_CLASS(SLTIU);
INSTR_CLASS(XORI);
INSTR_CLASS(ORI);
INSTR_CLASS(ANDI);
INSTR_CLASS(SLLI);
INSTR_CLASS(SRLI);
INSTR_CLASS(SRAI);

INSTR_CLASS(ADD);
INSTR_CLASS(SUB);
INSTR_CLASS(SLL);
INSTR_CLASS(SLT);
INSTR_CLASS(SLTU);
INSTR_CLASS(XOR);

INSTR_CLASS(SRL);
INSTR_CLASS(SRA);
INSTR_CLASS(OR);
INSTR_CLASS(AND);

INSTR_CLASS(FENCE);
INSTR_CLASS(FENCEI);

INSTR_CLASS(ECALL);
INSTR_CLASS(EBREAK);
INSTR_CLASS(E73);

INSTR_CLASS(CSRRW);
INSTR_CLASS(CSRRS);
INSTR_CLASS(CSRRC);
INSTR_CLASS(CSRRWI);
INSTR_CLASS(CSRRSI);
INSTR_CLASS(CSRRCI);

    

/* Local Variables:      */
/* mode: c++             */
/* indent-tabs-mode: nil */
/* tab-width: 4          */
/* c-basic-offset: 4     */
/* End:                  */
