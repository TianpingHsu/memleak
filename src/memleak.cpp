
#include "StaticArrayAllocator.h"
#include <map>
#include <execinfo.h>
#include "Man.h"

#define BACKTRACE(p) do {\
    Man::getInstance()->bt(p); \
} while(0)

extern "C" void* __wrap_malloc(size_t size) {
    void* p = __real_malloc(size);
    BACKTRACE(p);
    return p;
}

extern "C" void* __wrap_calloc(size_t nitems, size_t size) {
    void* p = __real_calloc(nitems, size);
    BACKTRACE(p);
    return p;
}

extern "C" void* __wrap_realloc(void* ptr, size_t size) {
    void* p = __real_realloc(ptr, size);
    BACKTRACE(p);
    return p;
}

extern "C" void __wrap_free(void *ptr) {
    Man::getInstance()->kick(ptr);
    __real_free(ptr);
}

extern "C" void* __wrap__Znwm(unsigned long size)
{
    void* p = __real__Znwm(size);
    BACKTRACE(p);
	return p;
}

extern "C" void __wrap__ZdlPv(void* ptr)
{
    Man::getInstance()->kick(ptr);
	__real__ZdlPv(ptr);
}

