#include <stdio.h>
#include "Memory.h"
#include "TraceCtl.h"

extern TraceCtl trace;

int Memory::write8(uint32_t addr, uint8_t v) {
    if (addr >= m_base && addr < m_base+m_size) {
        uint32_t off = addr - m_base;
        if (trace.mtrace()) printf(" Memory::write8(%08x, %02x)\n", addr, v);
        m_data[off] = v;
        return 0;
    }
    printf("Memory::write8(%08x, %02x) ???\n", addr, v);
    return -1;
}

int Memory::write16(uint32_t addr, uint16_t v) {
    if (addr >= m_base && addr < m_base+m_size) {
        uint32_t off = addr - m_base;
        if (trace.mtrace()) printf(" Memory::write16(%08x, %04x)\n", addr, v);
        m_data[off+0] = (v >>  0) & 0xff;
        m_data[off+1] = (v >>  8) & 0xff;
        return 0;
    }
    printf("Memory::write16(%08x, %02x) ???\n", addr, v);
    return -1;
}

int Memory::write32(uint32_t addr, uint32_t v) {
    if (addr >= m_base && addr < m_base+m_size) {
        uint32_t off = addr - m_base;
        if (trace.mtrace()) printf(" Memory::write32(%08x, %08x)\n", addr, v);
        m_data[off+0] = (v >>  0) & 0xff;
        m_data[off+1] = (v >>  8) & 0xff;
        m_data[off+2] = (v >> 16) & 0xff;
        m_data[off+3] = (v >> 24) & 0xff;
        return 0;
    }
    printf("Memory::write32(%08x, %02x) ???\n", addr, v);
    return -1;
}

uint8_t Memory::read8(uint32_t addr) {
    if (addr >= m_base && addr < m_base+m_size) {
        uint32_t off = addr - m_base;
        uint8_t v = m_data[off];
        if (trace.mtrace()) printf(" Memory::read8(%08x) -> %08x\n", addr, v);
        return v;
    }
    return -1;
}

uint16_t Memory::read16(uint32_t addr) {
    if (addr >= m_base && addr < m_base+m_size) {
        uint32_t off = addr - m_base;
        return (m_data[off+1] << 8) | (m_data[off+0] << 0);
    }
    return -1;
}

uint32_t Memory::read32(uint32_t addr) {
    if (addr >= m_base && addr < m_base+m_size) {
        uint32_t off = addr - m_base;
        uint32_t v = (m_data[off+3] << 24) | (m_data[off+2] << 16) | (m_data[off+1] << 8) | (m_data[off+0] << 0);
        if (trace.mtrace()) printf(" Memory::read32(%08x) -> %08x\n", addr, v);
        return v;
    }

    printf(" Memory::read32(%08x) ??? max %08x\n", addr, m_base + m_size);
    return -1;
}

void Memory::write_abs_bytes(uint32_t addr, uint8_t *data, int data_len) {
    if (trace.mtrace()) printf("Memory::write_abs_bytes(%08x, len=%d)\n", addr, data_len);
    for (int n = 0; n < data_len; n++) write8(addr+n, data[n]);
}


/* Local Variables:      */
/* mode: c++             */
/* indent-tabs-mode: nil */
/* tab-width: 4          */
/* c-basic-offset: 4     */
/* End:                  */

