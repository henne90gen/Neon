#include <cstdio>
#include <cstdlib>
#include <cstring>

#define DEBUG
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

void logString(string *s) {
    long size = 0;
    long maxSize = 0;
    char *buf = nullptr;
    if (s != nullptr) {
        size = s->size;
        maxSize = s->maxSize;
        buf = s->buf;
    }
    printf("%p {size=%ld, maxSize=%ld, buf=%p} - ", s, size, maxSize, buf);
}

void pi(long x) { printf("%ld\n", x); }

void pf(double x) { printf("%f\n", x); }

void pb(bool x) { printf("%s\n", x ? "true" : "false"); }

void ps(string *s) {
    if (s == nullptr) {
        LOG(logString(s); printf("Did not receive a string to print.\n"));
        return;
    }

    if (s->size == 0 || s->buf == nullptr) {
        LOG(logString(s); printf("Cannot print empty string.\n"));
        return;
    }

    char *buf = (char *)malloc(s->size + 1);
    memcpy(buf, s->buf, s->size);
    buf[s->size] = '\0';
    printf("%s\n", buf);
    free(buf);

    LOG(logString(s); printf("Printed string.\n"));
}

long ftoi(double x) { return (long)x; }

double itof(long x) { return (double)x; }

void createString(string *s, char *data) {
    if (s->buf != nullptr) {
        LOG(logString(s); printf("Found existing buffer.\n"));
    }
    s->buf = (char *)malloc(s->maxSize);
    memcpy(s->buf, data, s->size);
    LOG(logString(s); printf("Initialized string.\n"));
}

void deleteString(string *s) {
    char *buf = s->buf;
    if (buf == nullptr) {
        LOG(logString(s); printf("Nothing to delete, string has no data.\n"));
        return;
    }
    free(buf);
    LOG(logString(s); printf("Freed string.\n"));
    s->buf = nullptr;
}

void resizeString(string *s, long newMaxSize) {
    char *newBuf = (char *)malloc(newMaxSize);
    memcpy(newBuf, s->buf, s->size);
    free(s->buf);
    s->buf = newBuf;
    s->maxSize = newMaxSize;
}

void appendString(string *s0, string *s1, string *s2) {
    long newMaxSize = s1->size + s2->size;
    if (s0->maxSize < newMaxSize) {
        resizeString(s0, newMaxSize);
    }
    memcpy(s0->buf, s1->buf, s1->size);
    memcpy(s0->buf + s1->size, s2->buf, s2->size);
    s0->size = newMaxSize;
}
}
