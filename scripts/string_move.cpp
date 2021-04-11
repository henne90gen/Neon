#include <string>

struct MyType {
    std::string name;
};

int main() {

    auto *buf = std::malloc(sizeof(MyType));
    auto *t = new (buf) MyType;

    std::string name = "hello";
    t->name = std::move(name);

    return 0;
}