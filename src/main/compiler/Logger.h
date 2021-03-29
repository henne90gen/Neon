#pragma once

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

class Logger {
  public:
    enum LogLevel {
        DEBUG_ = 0,
        INFO = 1,
        WARNING = 2,
        ERROR = 3,
        DISABLED = 4,
    };

    Logger();

    inline void debug(const std::string &msg) const { log(LogLevel::DEBUG_, msg); }
    inline void info(const std::string &msg) const { log(LogLevel::INFO, msg); }
    inline void warn(const std::string &msg) const { log(LogLevel::WARNING, msg); }
    inline void error(const std::string &msg) const { log(LogLevel::ERROR, msg); }

    LogLevel getLogLevel() const { return logLevel; }
    void setLogLevel(LogLevel level) { this->logLevel = level; }

    void setColorEnabled(bool enabled) { this->colorEnabled = enabled; }

  private:
    LogLevel logLevel = LogLevel::INFO;
    bool colorEnabled = true;

    void log(const LogLevel level, const std::string msg) const;
};
