#pragma once

#include <stdio.h>
#include <string>
#include <vector>

#include "Memory.h"

class Hex {
public:
	Hex() = default;

	void set_memory(Memory *mem) { m_memory = mem; }
	void add_hex_file(const char *fn);
	void add_binary_file(const char *fn);
	int load_files();
	uint32_t get_start_addr() const { return m_start_addr; }
	
	static constexpr int kMaxData = 64;
	
private:
	int from_ascii_hex(char ch);
	int get_hex_byte(const char *p);
	int parse_hex_file_line(const char *l);
	int read_entire_hex_file(FILE *f);
	int read_hex_file(std::string fn);
	int read_bin_file(std::string fn, uint32_t loc);
	int load_fd(int fd, uint32_t len, uint32_t loc);
	
	std::vector<std::string> m_hex_files;
	std::vector<std::string> m_bin_files;
	uint8_t m_data[kMaxData];
	int m_data_len = 0;
	uint32_t m_addr = 0;
	uint32_t m_base_addr = 0;
	uint32_t m_start_addr = 0;
	bool m_debug = false;
	
	Memory *m_memory;
};


