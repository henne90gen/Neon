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

string *createString(char *data, long size, long maxSize) {
    auto s = (string *)malloc(sizeof(string));
    s->size = size;
    s->maxSize = maxSize;
    s->buf = (char *)malloc(s->maxSize);
    if (data != nullptr) {
        memcpy(s->buf, data, s->size);
        memset(s->buf + s->size, 0, s->maxSize - s->size);
    } else {
        memset(s->buf, 0, s->maxSize);
    }
    return s;
}

void deleteString(string *s) {
    char *buf = s->buf;
    if (buf == nullptr) {
        return;
    }
    free(buf);
    s->buf = nullptr;
}

void resizeString(string *s, long newMaxSize) {
    // TODO implement exponential growth here?
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

void assignString(string *dest, string *src) {
    if (dest->maxSize < src->size) {
        resizeString(dest, src->size);
    }
    memcpy(dest->buf, src->buf, src->size);
    dest->size = src->size;
}
}
