#include "Logger.h"

Logger::Logger() {
#ifdef WIN32
    // NOTE printing an empty line to enable colors for this terminal session
    system("echo.");
#endif
}

void Logger::log(const Logger::LogLevel level, const std::string& msg) const {
    if (logLevel > level) {
        return;
    }

    auto now = std::chrono::system_clock::now();
    auto nowTimeT = std::chrono::system_clock::to_time_t(now);

    std::string levelStr;
    switch (level) {
    case DEBUG_:
        levelStr = "DEBG";
        break;
    case INFO:
        levelStr = "INFO";
        break;
    case WARNING:
        levelStr = "WARN";
        break;
    case ERROR:
        levelStr = "ERRO";
        break;
    case DISABLED:
        break;
    }
    std::string colorStr;
    if (colorEnabled) {
        switch (level) {
        case DEBUG_:
            colorStr = "\u001b[36m";
            break;
        case INFO:
            colorStr = "\u001b[92m";
            break;
        case WARNING:
            colorStr = "\u001b[93m";
            break;
        case ERROR:
            colorStr = "\u001b[91m";
            break;
        case DISABLED:
            break;
        }
    }

    std::cout << colorStr << "[" << std::put_time(std::localtime(&nowTimeT), "%Y-%m-%d %X") << "] - [" << levelStr
              << "] " << msg;
    if (colorEnabled) {
        std::cout << "\u001b[0m";
    }
    std::cout << "\n";
}
