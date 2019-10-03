#include <iostream>

extern "C" {
    int hello();
}

int main() {
    std::cout << hello() << std::endl;
}
