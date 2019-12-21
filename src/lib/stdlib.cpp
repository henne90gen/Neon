#include <cstdio>

extern "C" {
void pi(long x) { printf("%ld\n", x); }

void pf(double x) { printf("%f\n", x); }

void pb(bool x) { printf("%s\n", x ? "true" : "false"); }

long ftoi(double x) { return (int)x; }

double itof(long x) { return (double)x; }
}
