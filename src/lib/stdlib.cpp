#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef DEBUG
#define LOG(x) x
#else
#define LOG(x)
#endif

extern "C" {

struct string {
    char *buf;
    long size;
    long maxSize;
};

void pi(long x) { printf("%ld\n", x); }

void pf(double x) { printf("%f\n", x); }

void pb(bool x) { printf("%s\n", x ? "true" : "false"); }

void ps(string *s) {
    if (s == nullptr) {
        printf("received null string\n");
        return;
    }

    if (s->size == 0 || s->buf == nullptr) {
        printf("empty string!\n");
        return;
    }

    char *buf = (char *)malloc(s->size + 1);
    memcpy(buf, s->buf, s->size);
    buf[s->size] = '\0';
    LOG(printf("Printing string at    %p {size=%ld, maxSize=%ld, buf=%s}\n", s, s->size, s->maxSize, buf));
    printf("%s\n", buf);
}

long ftoi(double x) { return (long)x; }

double itof(long x) { return (double)x; }

void initString(string *s, char *data) {
    s->buf = (char *)malloc(s->maxSize);
    memcpy(s->buf, data, s->size);
    LOG(printf("Initialized string at %p {size=%ld, maxSize=%ld, buf=%s}\n", s, s->size, s->maxSize, s->buf));
}
}
