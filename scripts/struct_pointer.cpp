#include <cstdlib>
#include <cstring>

struct string {
    char *buf;
    int size;
    int maxSize;
};

void initString(string *s, char *data) {
    s->buf = (char *)malloc(s->maxSize);
    memcpy(s->buf, data, s->size);
}

int main() {
    string s = {};
    initString(&s, "Hello world");
}
