#include "Memory.h"

unsigned char *CPUMemory::get_prg_ram_start() {
    return std::begin(prg_ram);
}

void CPUMemory::debug_dump(std::ostream& out, int start, int range, int bytes_per_line) {
    unsigned char *it = prg_ram + start;
    for (int i = 1; i <= range; i++) {
        out << hex(*it++) << " ";
        if (i % bytes_per_line == 0) {
            out << std::endl;
        }
    }

}
