#ifndef GUARD_BINARY_READER_H
#define GUARD_BINARY_READER_H

#include "Memory.h"
#include <string>
#include <cstdint>

class BinaryReader {
    private:
        std::string file_name;
    public:
        BinaryReader(std::string name);
        std::vector<uint8_t> read_bytes();
        void load_to_memory(CPUMemory &mem);
};

#endif
