#include "Utils.h"

std::string replace(const std::string &str, const std::string &from, const std::string &to) {
    std::string result = str;
    size_t pos = 0;
    const size_t increment = to.size();
    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.length(), to);
        pos += increment;
    }
    return result;
}

std::string trim(const std::string &s) { return rtrim(ltrim(s)); }

std::string rtrim(const std::string &s) {
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string ltrim(const std::string &s) {
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}
