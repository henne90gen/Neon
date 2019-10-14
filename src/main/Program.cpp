#include "Program.h"

auto trim(const std::string &s) -> std::string { return rtrim(ltrim(s)); }

auto rtrim(const std::string &s) -> std::string {
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

auto ltrim(const std::string &s) -> std::string {
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}
