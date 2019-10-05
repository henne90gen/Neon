#include <stdio.h>

extern "C" {
int hello() { return 5; }
void print(int i) { printf("%d\n", i); }
}
