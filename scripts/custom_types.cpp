#include <cstdlib>

struct MyType {
    int i;
    char *p;
    int c;
};

void myFunc(MyType *t) { t->i = 100000000; }

int main() {
    MyType *t = (MyType *)malloc(sizeof(MyType));
    myFunc(t);
    return 0;
}
