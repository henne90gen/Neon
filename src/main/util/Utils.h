#pragma once

#include <string>

#define NEON_NOT_IMPLEMENTED                                                                                                \
    std::cout << __FILE__ << ":" << __FUNCTION__ << ": Not implemented yet." << std::endl;                             \
    exit(1);

#define STARTS_WITH(word, search) word.find(search) == 0
#define TOKEN(type, content) std::optional<Token>({type, content})

std::string replace(const std::string &str, const std::string &from, const std::string &to);

const std::string WHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string &s);

std::string rtrim(const std::string &s);

std::string trim(const std::string &s);
