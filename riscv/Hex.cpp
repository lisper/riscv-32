#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "Hex.h"
#include "TraceCtl.h"

extern TraceCtl trace;

int Hex::from_ascii_hex(char ch) {
    if ('a' <= ch && ch <= 'f') return 10 + (ch - 'a');
    if ('A' <= ch && ch <= 'F') return 10 + (ch - 'A');
    if ('0' <= ch && ch <= '9') return ch - '0';
    return -1;
}

int Hex::get_hex_byte(const char *p) {
    int b1 = from_ascii_hex(p[0]);
    int b2 = from_ascii_hex(p[1]);
    if (b1 == -1 || b2 == -1) return -1;
    return (b1 << 4) | b2;
}

int Hex::parse_hex_file_line(const char *l) {
    if (l[0] != ':') return -1;
    int len = get_hex_byte(&l[1]);
    if (len < 0) return -1;
    int addrh = get_hex_byte(&l[3]);
    int addrl = get_hex_byte(&l[5]);
    int rt = get_hex_byte(&l[7]);
    if (addrh == -1 || addrl == -1 || rt == -1) return -1;

    if (m_debug) printf("len %d, addr %04x, rtype %d\n", len, (addrh << 8) | addrl, rt);

    int ret = -1;
    switch (rt) {
    case 0: // data
    {
        m_addr = m_base_addr | (addrh << 8) | addrl;

        int checksum = len + addrh + addrl + rt;
        if (len > 0 && len < kMaxData) {
            for (int i = 0; i < len; i++) {
                int b = get_hex_byte(&l[9+(i*2)]);
                if (b == -1) return -1;
                m_data[i] = b;
                checksum += b;
                if (m_debug) printf("%02x ", b);
            }
        }
        if (m_debug) printf("\n");

        int cs = get_hex_byte(&l[9+(len*2)]);
        int check = ~(checksum & 0xff) + 1;
        check &= 0x00ff;
        if (cs != check) {
            if (m_debug) printf("checksum: want %02x, got %02x\n", cs, check);
            return -1;
        }

        m_data_len = len;
        m_memory->write_abs_bytes(m_addr, m_data, m_data_len);
    }
        ret = 0;
        break;

    case 1:  // EOF
        ret = 0;
        break;

    case 2:
    case 3:
        ret = 0;
        break;
        
    case 4: {
        int addrh = get_hex_byte(&l[9]);
        int addrl = get_hex_byte(&l[11]);
        if (addrh == -1 || addrl == -1) return -1;
        m_base_addr = ((addrh << 8) | addrl) << 16;
        if (m_debug) printf("base: %08x\n", m_base_addr);
    }
        ret = 0;
        break;

    case 5: {
        int sa3 = get_hex_byte(&l[9]);
        int sa2 = get_hex_byte(&l[11]);
        int sa1 = get_hex_byte(&l[13]);
        int sa0 = get_hex_byte(&l[15]);
        if (sa3 == -1 || sa2 == -1 || sa1 == -1 || sa0 == -1) return -1;
        m_start_addr = (sa3 << 24) | (sa2 << 16) | (sa1 << 8) | (sa0 << 0);
    }
        ret = 0;
        break;
    }

    return ret;
}

int Hex::read_entire_hex_file(FILE *f) {
    char line[1024];
    int ret = -1;

    while (1) {
        if (!fgets(line, sizeof(line), f))
            break;
        
        int len = strlen(line);
        if (len && line[len-1] == '\n') line[len-1] = 0;
        if (m_debug) printf("line: %s\n", line);
        ret = parse_hex_file_line(line);
        if (ret)
            break;
    }

    return ret;
}

int Hex::read_hex_file(std::string fn) {
    FILE *f;
    int ret = -1;
    
    f = fopen(fn.c_str(), "r");
    if (f) {
        ret = read_entire_hex_file(f);
        fclose(f);
    }

    return ret;
}

void Hex::add_hex_file(const char *fn) {
	m_hex_files.push_back(fn);
}

void Hex::add_binary_file(const char *fn) {
	m_bin_files.push_back(fn);
}

int Hex::load_fd(int fd, uint32_t len, uint32_t loc) {
    uint8_t buffer[512];
    
    while (len > 0) {
        int rs = len > 512 ? 512 : len;
        int ret = read(fd, buffer, rs);

        m_memory->write_abs_bytes(loc, buffer, rs);
    
        loc += rs;
        len -= rs;
    }

    return 0;
}


int Hex::read_bin_file(std::string fn, uint32_t loc) {
    int fd = open(fn.c_str(), O_RDONLY);
    if (fd < 0) {
        perror(fn.c_str());
        return -1;
    }

    struct stat stat_info;
    int ret = fstat(fd, &stat_info);
    if (ret < 0) {
        perror(fn.c_str());
        close(fd);
        return -1;
    }

    int bl = stat_info.st_size;
    ret = load_fd(fd, bl, loc);
    if (ret < 0) {
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

int Hex::load_files() {
	for (std::string s : m_hex_files) {
		printf("hex file: %s\n", s.c_str());
        if (read_hex_file(s)) return -1;
	}

	for (std::string s : m_bin_files) {
		printf("bin file: %s\n", s.c_str());
        if (read_bin_file(s, 0x80000000)) return -1;
    }
    
    return 0;
}

/* Local Variables:      */
/* mode: c++             */
/* indent-tabs-mode: nil */
/* tab-width: 4          */
/* c-basic-offset: 4     */
/* End:                  */
