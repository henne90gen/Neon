#include <cstdint>
#include <cstdio>

extern "C" {
typedef uint64_t uptr;
void __asan_version_mismatch_check_v8() { printf("__asan_version_mismatch_check_v8\n"); }
void __asan_init() { printf("__asan_init\n"); }
void __asan_register_globals() { printf("__asan_register_globals\n"); }
void __asan_unregister_globals() { printf("__asan_unregister_globals\n"); }
void __asan_report_store1(uptr addr) { printf("__asan_report_store1: %ld\n", addr); }
void __asan_report_store8(uptr addr) { printf("__asan_report_store8: %ld\n", addr); }
void __asan_report_load8(uptr addr) { printf("__asan_report_load8: %ld\n", addr); }
void __asan_memcpy(void *to, const void *from, uptr size) { printf("__asan_memcpy: %p %p %ld\n", to, from, size); }
void __asan_memset(void *block, int c, uptr size) { printf("__asan_memset: %p, %d %ld\n", block, c, size); }
void __asan_memmove(void *to, const void *from, uptr size) { printf("__asan_memmove: %p %p %ld\n", to, from, size); }
}
