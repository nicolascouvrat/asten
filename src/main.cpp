#include "Cpu.h"
#include "BinaryReader.h"

#include <iostream>
#include <string>
#include <iterator>


using std::string;  using std::cout;    using std::endl;
using std::ostream; using std::iterator;

int main(void) {
    BinaryReader r("test.bin");
    CPU c;
    // should have the values $a9 $01 $8d $02
    c.get_memory().load(0x6000, r.read_bytes());
    c.get_memory().debug_dump(std::cout, 0x6000, 4);
}
