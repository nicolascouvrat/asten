#ifndef GUARD_LOGGER_H
#define GUARD_LOGGER_H

#include <string>
#include <iostream>
#include <map>


enum LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR
};


class Logger {
    public:
        Logger& debug();
        Logger& info();
        Logger& warn();
        Logger& error();
        static Logger get_logger(std::string);
        Logger& set_level(LogLevel);
        Logger& toggle_header();
        // operator
        template<class T>
        Logger& operator<< (const T& msg) {
            if (output_enabled)
                out << msg;
            return *this;
        }
    private:
        Logger(std::string);
        std::string name;
        LogLevel level;
        std::ostream& out;
        bool output_enabled = true;
        bool header_enabled = true;
        static std::map<std::string, Logger> logger_map;
        void output_header(LogLevel);
        Logger& log(LogLevel);
};

#endif
