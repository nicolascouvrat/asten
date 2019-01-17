#ifndef GUARD_BINARY_READER_H
#define GUARD_BINARY_READER_H

#include "Memory.h"
#include <string>

class BinaryReader {
    private:
        std::string file_name;
    public:
        BinaryReader(std::string name);
        std::string get_file_name();
        void load_to_memory(CPUMemory &mem);
};

#endif
