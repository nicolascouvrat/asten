#ifndef GUARD_LOGGER_H
#define GUARD_LOGGER_H

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include "Utilities.h"

class Logger {
    public:
        friend std::ostream& operator<< (Logger&, std::string);
        friend std::ostream& operator<< (Logger&, const HexChar&);
        friend std::ostream& operator<< (Logger&, const std::vector<HexChar>&);
        Logger(std::string, std::ostream&);
        std::ostream& dump_log_infos();
    private:
        std::string name;
        std::ostream& output_stream;
};

class LoggerStore {
    public:
        static Logger getLogger(std::string);
    private:
        static std::map<std::string, Logger> logger_map;
};

#endif
