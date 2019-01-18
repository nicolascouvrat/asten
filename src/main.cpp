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
    r.load_to_memory(c.get_memory());
    c.get_memory().debug_dump(std::cout, 0, 5, 2);
}
