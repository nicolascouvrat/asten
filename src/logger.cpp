#include "logger.h"


std::map<std::string, Logger> Logger::loggerMap = std::map<std::string, Logger>();

Logger Logger::getLogger(std::string name, std::string fileName) {
  std::map<std::string, Logger>::iterator it = loggerMap.find(name);
  if (it != loggerMap.end())
    return it->second;
  else
    return (loggerMap.emplace(std::make_pair(name, Logger(name, fileName))).first)->second;
}

Logger::Logger(const Logger& log):
  name(log.name),
  outputFileName(log.outputFileName),
  level(log.level),
  out(buf)
{
  if (outputFileName != "") {
    of.open(outputFileName);
    buf = of.rdbuf();
  }
  else
    buf = std::cout.rdbuf();
  out.rdbuf(buf);
}

Logger::Logger(std::string _name, std::string fileName):
  name(_name),
  outputFileName(fileName),
  level(INFO),
  out(buf)
{
  if (outputFileName != "") {
    of.open(outputFileName);
    buf = of.rdbuf();
  }
  else
    buf = std::cout.rdbuf();
  out.rdbuf(buf);
}

void Logger::outputHeader(LogLevel l) {
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
    outputEnabled = false;
  else {
    outputEnabled = true;
    if (headerEnabled)
      outputHeader(l);
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

Logger& Logger::setLevel(LogLevel l) {
  level = l;
  return *this;
}

Logger& Logger::toggleHeader() {
  headerEnabled = !headerEnabled;
  return *this;
}
