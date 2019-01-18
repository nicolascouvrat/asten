#include "Logger.h"


using std::string;


std::map<string, Logger> LoggerStore::logger_map = std::map<string, Logger>();

Logger LoggerStore::getLogger(string name) {
    std::map<string, Logger>::iterator it = logger_map.find(name);
    if (it != logger_map.end())
        return it->second;
    else
        return Logger(name, std::cout);
}

Logger::Logger(string _name, std::ostream& os): name(_name), output_stream(os) { }

std::ostream& Logger::dump_log_infos() {
    return output_stream << name << ": ";
}
std::ostream& operator<< (Logger& l, string s) {
    return l.dump_log_infos() << s;
}

std::ostream& operator<< (Logger& l, const std::vector<HexChar>& hcs) {
    std::ostream& out = l.dump_log_infos();
    std::vector<HexChar>::const_iterator it = hcs.begin();
    while (it != hcs.end()) {
        out << *it++ << " ";
    }
    return out;
}

std::ostream& operator<< (Logger& l, const HexChar& hc) {
    return l.dump_log_infos() << hc;
}



