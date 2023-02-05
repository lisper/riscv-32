#pragma once

#include <stdint.h>
#include "Memory.h"
#include "Instr.h"

class Cpu {
public:
	Cpu() = default;
    
    void init(uint32_t start_pc);
    void execute();
    Memory *get_memory() { return &m_memory; }
        
    uint32_t x[32] = {};     // registers
    uint32_t pc;             // program counter

    // CSRs
    uint32_t ustatus;      // 000
    uint32_t uie;          // 004
    uint32_t utvec;        // 005
    uint32_t uscratch;     // 040
    uint32_t uepc;         // 041
    uint32_t ucause;       // 042
    uint32_t utval;        // 043
    uint32_t uip;          // 044


    static constexpr char *reg_nicknames[32] = {
        (char *)"zero",
        (char *)"ra",
        (char *)"sp",
        (char *)"gp",
        (char *)"tp",
        (char *)"t0",
        (char *)"t1",
        (char *)"t2",
        (char *)"fp",
        (char *)"s1",
        (char *)"a0",
        (char *)"a1",
        (char *)"a2",
        (char *)"a3",
        (char *)"a4",
        (char *)"a5",
        (char *)"a6",
        (char *)"a7",
        (char *)"s2",
        (char *)"s3",
        (char *)"s4",
        (char *)"s5",
        (char *)"s6",
        (char *)"s7",
        (char *)"s8",
        (char *)"s9",
        (char *)"s10",
        (char *)"s11",
        (char *)"t3",
        (char *)"t4",
        (char *)"t5",
        (char *)"t6"
    };

    uint32_t get_reg(int reg);
    void set_reg(int reg, uint32_t v);

    uint32_t read32(uint32_t addr);
    uint16_t read16(uint32_t addr);
    uint8_t read8(uint32_t addr);
    
    void write32(uint32_t addr, uint32_t v);
    void write16(uint32_t addr, uint16_t v);
    void write8(uint32_t addr, uint8_t v);

    friend class Instr;
    
private:
    void add_decode(int opcode, Instr *func);
    void add_decode(int opcode, int funct3, Instr *func);
    void add_decode(int opcode, int funct3, int funct7, Instr *func);
    void add_decode(int opcode, int funct3, int funct7, int rs1, Instr *func);
    void add_decode(int opcode, int funct3, int funct7, int rs1, int rd, Instr *func);
    void setup_decode();

    void fake_putc_dump();
    void fake_putc(char ch);
    
    uint32_t fetch();
    void incr_pc() { pc += 4; }
    Memory m_memory;

    Instr *m_decode_table[1024*256] = {};

    char m_uart_buf[256];
    int m_uart_buf_len = 0;

};


/* Local Variables:      */
/* mode: c++             */
/* indent-tabs-mode: nil */
/* tab-width: 4          */
/* c-basic-offset: 4     */
/* End:                  */
