#include "Cpu.h"
#include "BinaryReader.h"
#include "Memory.h"

#include <iostream>
#include <string>
#include <iterator>

using std::string;  using std::cout;    using std::endl;
using std::ostream; using std::iterator;


struct HexChar {
    unsigned char c;
    HexChar(unsigned char _c): c(_c) {};
};

ostream& operator<< (ostream& o, const HexChar &hc) {
    return o << std::hex << (int)hc.c;
}

HexChar hex(unsigned char c) { return HexChar(c); } 

void read_prg_ram(CPUMemory m, ostream& out, int range) {
    unsigned char *it = m.get_prg_ram_start();
    for (int i = 0; i < range; i++)
        out << hex(*it++) << endl;
}

int main(void) {
    BinaryReader r("test.bin");
    CPUMemory m;
    r.load_to_memory(m);
    cout << r.get_file_name() << endl;
    read_prg_ram(m, cout, 10);
}
