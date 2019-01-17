#include "BinaryReader.h"
#include <fstream>
#include <iterator>
#include <algorithm>
// TODO: remove
#include <iostream>

using std::ifstream;    using std::istreambuf_iterator;

BinaryReader::BinaryReader(std::string name): file_name(name) {}

std::string BinaryReader::get_file_name() { return file_name; }
void BinaryReader::load_to_memory(CPUMemory &mem) {
    ifstream in(file_name, std::ios::binary);
    istreambuf_iterator<char> it(in);

    std::copy(
        istreambuf_iterator<char>(in),
        istreambuf_iterator<char>(),
        mem.get_prg_ram_start()
    );
}
