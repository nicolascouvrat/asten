#ifndef GUARD_LOGGER_H
#define GUARD_LOGGER_H

#include <string>
#include <iostream>
#include <fstream>
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
    Logger(const Logger&);
    static Logger getLogger(std::string, std::string outputFile = "");
    Logger& setLevel(LogLevel);
    Logger& toggleHeader();
    // operator
    template<class T>
    Logger& operator<< (const T& msg) {
      if (outputEnabled)
        out << msg;
      return *this;
    }
  private:
    Logger(std::string, std::string);
    std::string name;
    std::string outputFileName;
    LogLevel level;
    std::streambuf *buf;
    std::ofstream of;
    std::ostream out;
    bool outputEnabled = true;
    bool headerEnabled = true;
    static std::map<std::string, Logger> loggerMap;
    void outputHeader(LogLevel);
    Logger& log(LogLevel);
};

#endif
