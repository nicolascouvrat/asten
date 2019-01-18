#include "BinaryReader.h"
#include <fstream>
#include <iterator>
#include <algorithm>
// TODO: remove
#include <iostream>

using std::ifstream;    using std::istreambuf_iterator;

BinaryReader::BinaryReader(std::string name): file_name(name) {}

std::string BinaryReader::get_file_name() { return file_name; }
std::vector<uint8_t> BinaryReader::read_bytes() {
    ifstream in(file_name, std::ios::binary);
    istreambuf_iterator<char> it(in);
    std::vector<uint8_t> buffer;

    std::copy(
        istreambuf_iterator<char>(in),
        istreambuf_iterator<char>(),
        std::back_inserter(buffer)
    );
    return buffer;
}
