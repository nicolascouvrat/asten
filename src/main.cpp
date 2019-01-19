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
    c.get_memory().debug_dump(0x6000, 8);
    c.execute(0);
    c.debug_dump();
}
