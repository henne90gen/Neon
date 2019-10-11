#pragma once

#define NOT_IMPLEMENTED                                                                                                \
    std::cout << __FILE__ << ":" << __FUNCTION__ << ": Not implemented yet." << std::endl;                             \
    exit(1);

#define STARTS_WITH(word, search) word.find(search) == 0
#define TOKEN(type, content) std::optional<Token>({type, content})
