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

    inline void debug(const std::string &msg) const { log(LogLevel::DEBUG_, msg); }
    inline void info(const std::string &msg) const { log(LogLevel::INFO, msg); }
    inline void warn(const std::string &msg) const { log(LogLevel::WARNING, msg); }
    inline void error(const std::string &msg) const { log(LogLevel::ERROR, msg); }

    LogLevel getLogLevel() const { return logLevel; }
    void setLogLevel(LogLevel level) { this->logLevel = level; }

  private:
    LogLevel logLevel = LogLevel::INFO;

    inline void log(const LogLevel level, const std::string msg) const {
        if (logLevel > level) {
            return;
        }

        auto now = std::chrono::system_clock::now();
        auto nowTimeT = std::chrono::system_clock::to_time_t(now);

        std::string levelStr;
        switch (logLevel) {
        case DEBUG_:
            levelStr = "DEBUG";
            break;
        case INFO:
            levelStr = "INFO";
            break;
        case WARNING:
            levelStr = "WARN";
            break;
        case ERROR:
            levelStr = "ERROR";
            break;
        case DISABLED:
            break;
        }
        std::cout << "[" << std::put_time(std::localtime(&nowTimeT), "%Y-%m-%d %X") << "] - [" << levelStr << "] "
                  << msg << "\n";
    }
};
