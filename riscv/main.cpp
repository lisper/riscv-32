#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "Cpu.h"
#include "Hex.h"
#include "TraceCtl.h"

TraceCtl trace;

int TraceCtl::m_debug_flags = 0;
int TraceCtl::m_trace_flags = 0;

extern char *optarg;

void do_help(const char *name) {
    fprintf(stderr, "usage: %s\n", name);
}

void add_file(const char *filename) {
}

void load_hex() {
        
}

void run(Cpu *cpu, uint32_t start) {
    cpu->init(start);
#if 1
    while (1) {
        cpu->execute();
    }
#endif
}

int parse_trace_arg(char *arg) {
    if (strcmp(arg, "all") == 0) {
        trace.set_trace_flags(-1);
        trace.set_debug_flags(-1);
        return 0;
    }

    return 0;
}

int main(int argc, char *argv[]) {
    Hex hex;

    bool done = false;
    for (;;) {
        switch(getopt(argc, argv, "hf:b:t")) {
        case '?':
        case 'h':
        default :
            do_help(argv[0]);
            return -1;

        case 'f':
            hex.add_hex_file(optarg);
            break;

        case 'b':
            hex.add_binary_file(optarg);
            break;

        case 't':
            parse_trace_arg(optarg);
            break;
            
        case -1:
            done = true;
            break;
        }

        if (done) break;
    }

    Cpu cpu;

    hex.set_memory(cpu.get_memory());
    hex.load_files();
    uint32_t sa = hex.get_start_addr();
    
    run(&cpu, sa);

    return 0;
}

/* Local Variables:      */
/* mode: c++             */
/* indent-tabs-mode: nil */
/* tab-width: 4          */
/* c-basic-offset: 4     */
/* End:                  */
