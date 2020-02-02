#include <cstdlib>
#include <cstring>
#include <stdio.h>

int main() {
    const char *string = "Hello world";
    for (int i = 0; i < 1000000000000; i++) {
        void *buf = malloc(10000);
        memcpy(buf, string, strlen(string));
        printf("%s\n", buf);
        free(buf);
    }
}
