#pragma once

#include <stdint.h>

class Memory {
public:
	Memory() = default;

    int write8(uint32_t addr, uint8_t v);
    int write16(uint32_t addr, uint16_t v);
    int write32(uint32_t addr, uint32_t v);
    
    uint8_t read8(uint32_t addr);
    uint16_t read16(uint32_t addr);
    uint32_t read32(uint32_t addr);

    void write_abs_bytes(uint32_t addr, uint8_t *data, int data_len);

private:
    uint32_t m_base = 0x80000000;
    uint32_t m_size = 1024*1024;
    uint8_t m_data[1024*1024];
};


/* Local Variables:      */
/* mode: c++             */
/* indent-tabs-mode: nil */
/* tab-width: 4          */
/* c-basic-offset: 4     */
/* End:                  */
