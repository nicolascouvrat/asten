#include "Logger.h"


std::map<std::string, Logger> Logger::logger_map = std::map<std::string, Logger>();

Logger Logger::get_logger(std::string name, std::string file_name) {
    std::map<std::string, Logger>::iterator it = logger_map.find(name);
    if (it != logger_map.end())
        return it->second;
    else
        return (logger_map.emplace(std::make_pair(name, Logger(name, file_name))).first)->second;
}

Logger::Logger(const Logger& log):
    name(log.name),
    level(log.level),
    out(buf),
    output_file_name(log.output_file_name)
{
    if (output_file_name != "") {
        of.open(output_file_name);
        buf = of.rdbuf();
    }
    else
        buf = std::cout.rdbuf();
    out.rdbuf(buf);
}

Logger::Logger(std::string _name, std::string file_name):
    name(_name),
    level(INFO),
    out(buf),
    output_file_name(file_name)
{
    if (output_file_name != "") {
        of.open(output_file_name);
        buf = of.rdbuf();
    }
    else
        buf = std::cout.rdbuf();
    out.rdbuf(buf);
}

void Logger::output_header(LogLevel l) {
    std::string label;
    switch(l) {
        case DEBUG: label = "DEBUG"; break;
        case INFO: label = "INFO"; break;
        case WARN: label = "WARN"; break;
        case ERROR: label = "ERROR"; break;
    }
    out << "[" + label + "]" << " " << name << ": ";
}

Logger& Logger::log(LogLevel l) {
    if (level > l)
        output_enabled = false;
    else {
        output_enabled = true;
        if (header_enabled)
            output_header(l);
    }
    return *this;
}

Logger& Logger::debug() {
    return log(DEBUG);
}

Logger& Logger::info() {
    return log(INFO);
}

Logger& Logger::warn() {
    return log(WARN);
}

Logger& Logger::error() {
    return log(ERROR);
}

Logger& Logger::set_level(LogLevel l) {
    level = l;
    return *this;
}

Logger& Logger::toggle_header() {
    header_enabled = !header_enabled;
    return *this;
}
