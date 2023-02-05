#include <stdio.h>
#include <stdlib.h>
#include "Cpu.h"
#include "TraceCtl.h"

extern TraceCtl trace;

void Cpu::init(uint32_t start) {
    setup_decode();
    pc = start;
}

void Cpu::add_decode(int opcode, Instr *func) {
    for (int funct3 = 0; funct3 < 8; funct3++) {
        add_decode(opcode, funct3, func);
    }
}

void Cpu::add_decode(int opcode, int funct3, Instr *func) {
    for (int funct7 = 0; funct7 < 256; funct7++) {
        add_decode(opcode, funct3, funct7, func);
    }
}

void Cpu::add_decode(int opcode, int funct3, int funct7, Instr *func) {
    int index = (funct7 << 10) | (funct3 << 7) | opcode;
    m_decode_table[index] = func;
    //printf("m_decode_table[0x%0x] = %p\n", index, func);
}

void Cpu::add_decode(int opcode, int funct3, int funct7, int rs1, Instr *func) {
}

void Cpu::add_decode(int opcode, int funct3, int funct7, int rs1, int rd, Instr *func) {
}

void Cpu::setup_decode() {
    add_decode(0x37, &instr_LUI);
    add_decode(0x17, &instr_AUIPC);
    add_decode(0x6f, &instr_JAL);
    add_decode(0x67, &instr_JALR);

    add_decode(0x63, 0, &instr_BEQ);
    add_decode(0x63, 1, &instr_BNE);
    add_decode(0x63, 4, &instr_BLT);
    add_decode(0x63, 5, &instr_BGE);
    add_decode(0x63, 6, &instr_BLTU);
    add_decode(0x63, 7, &instr_BGEU);

    add_decode(0x03, 0, &instr_LB);
    add_decode(0x03, 1, &instr_LH);
    add_decode(0x03, 2, &instr_LW);
    add_decode(0x03, 4, &instr_LBU);
    add_decode(0x03, 5, &instr_LHU);

    add_decode(0x23, 0, &instr_SB);
    add_decode(0x23, 1, &instr_SH);
    add_decode(0x23, 2, &instr_SW);

    add_decode(0x13, 0, &instr_ADDI);
    add_decode(0x13, 2, &instr_SLTI);
    add_decode(0x13, 3, &instr_SLTIU);
    add_decode(0x13, 4, &instr_XORI);
    add_decode(0x13, 6, &instr_ORI);
    add_decode(0x13, 7, &instr_ANDI);
    add_decode(0x13, 1, 0x00, &instr_SLLI);
    add_decode(0x13, 5, 0x00, &instr_SRLI);
    add_decode(0x13, 5, 0x20, &instr_SRAI);

    add_decode(0x33, 0, 0x00, &instr_ADD);
    add_decode(0x33, 0, 0x20, &instr_SUB);
    add_decode(0x33, 1, 0x00, &instr_SLL);
    add_decode(0x33, 2, 0x00, &instr_SLT);
    add_decode(0x33, 3, 0x00, &instr_SLTU);
    add_decode(0x33, 4, 0x00, &instr_XOR);

    add_decode(0x33, 5, 0x00, &instr_SRL);
    add_decode(0x33, 5, 0x20, &instr_SRA);
    add_decode(0x33, 6, 0x00, &instr_OR);
    add_decode(0x33, 7, 0x00, &instr_AND);

    add_decode(0x0f, 0, &instr_FENCE);
    add_decode(0x0f, 1, &instr_FENCEI);

    add_decode(0x73, 0, &instr_E73);
//    add_decode(0x0f, 0, 0x00, 0x00, 0x00, &instr_FENCE);
//    add_decode(0x0f, 1, 0x00, 0x00, 0x00, &instr_FENCE_I);
//    add_decode(0x73, 0, 0x00, 0x00, 0x00, &instr_ECALL);
//    add_decode(0x73, 0, 0x00, 0x00, 0x00, &instr_EBREAK);
    
    add_decode(0x73, 1, &instr_CSRRW);
    add_decode(0x73, 2, &instr_CSRRS);
    add_decode(0x73, 3, &instr_CSRRC);
    add_decode(0x73, 5, &instr_CSRRWI);
    add_decode(0x73, 6, &instr_CSRRSI);
    add_decode(0x73, 7, &instr_CSRRCI);
    
}

uint32_t Cpu::fetch() {
    return m_memory.read32(pc);
}

uint32_t Cpu::get_reg(int reg) {
    return x[reg];
}

void Cpu::set_reg(int reg, uint32_t v) {
    if (reg == 0) return;
    if (trace.idebug()) printf(" x%d (%s) <- %08x\n", reg, reg_nicknames[reg], v);
    x[reg] = v;
}

uint32_t Cpu::read32(uint32_t addr) {
    return m_memory.read32(addr);
}

uint16_t Cpu::read16(uint32_t addr) {
    return m_memory.read16(addr);
}

uint8_t Cpu::read8(uint32_t addr) {
    return m_memory.read8(addr);
}

void Cpu::write32(uint32_t addr, uint32_t v) {
    m_memory.write32(addr, v);
}

void Cpu::write16(uint32_t addr, uint16_t v) {
    m_memory.write16(addr, v);
}
    
void Cpu::write8(uint32_t addr, uint8_t v) {
    m_memory.write8(addr, v);
}    

void Cpu::fake_putc_dump() {
    m_uart_buf[m_uart_buf_len++] = 0;
    printf("uart: %s\n", m_uart_buf);
    m_uart_buf_len = 0;
}

void Cpu::fake_putc(char ch) {
    //printf("uart: fake %c\n", ch);
    if (m_uart_buf_len < 256) {
        if (ch == '\n') {
            fake_putc_dump();
            return;
        }
        if (ch != '\r') {
            m_uart_buf[m_uart_buf_len++] = ch;
        }
    }
}

void Cpu::execute() {
    uint32_t instr = fetch();

    if (trace.ptrace()) printf("pc: %08x, fetch %08x\n", pc, instr);
    
    unsigned funct7 = (instr >>25) & 0x0000007f;
    unsigned rs2    = (instr >>20) & 0x0000001f;
    unsigned rs1    = (instr >>15) & 0x0000001f;
    unsigned funct3 = (instr >>12) & 0x00000007;
    unsigned rd     = (instr >> 7) & 0x0000001f;
    unsigned opcode = (instr >> 0) & 0x0000007f;

    if (instr == 0x00002573) {
        fake_putc(x[10]);
        pc += 4;
        return;
    }
        
    // 10 bits.  0..0x3ff, 0..1023
    int index = (funct7 << 10) | (funct3 << 7) | opcode;
    Instr *func = m_decode_table[index];
    if (!func) {
        printf("panic - no opcode func, index %d (0x%x)\n", index, index);
        exit(1);
    }

    uint32_t old_pc = pc;
    
    func->execute(this, instr);

    if (pc == old_pc) {
        printf("loop?\n");
        exit(1);
    }
}

/* Local Variables:      */
/* mode: c++             */
/* indent-tabs-mode: nil */
/* tab-width: 4          */
/* c-basic-offset: 4     */
/* End:                  */
