#include "BinaryReader.h"
#include <fstream>
#include <iterator>
#include <algorithm>
#include "Logger.h"

using std::ifstream;    using std::istreambuf_iterator;


#define HEADER_SIZE 16

namespace {
    Logger log = Logger::get_logger("CartridgeReader").set_level(DEBUG);
}


BinaryReader::BinaryReader(std::string name): file_name(name) {}

std::vector<uint8_t> BinaryReader::read_bytes() {
    ifstream in(file_name, std::ios::binary);
    istreambuf_iterator<char> it(in);
    uint8_t header[HEADER_SIZE];
    for (int i = 0; i < HEADER_SIZE; i++)
        header[i] = *it++;

    std::vector<uint8_t> buffer;

    std::copy(
        istreambuf_iterator<char>(in),
        istreambuf_iterator<char>(),
        std::back_inserter(buffer)
    );
    return buffer;
}
