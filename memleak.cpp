
#include "StaticArrayAllocator.h"
#include <map>
#include <execinfo.h>

struct Node {
    public:
        const static int s_depth = 40;
        void* buf[s_depth] = {0};
        int depth = 0;
};

typedef POC::StaticArrayAllocator<std::map<void*, Node>::value_type> MyAllocator;
std::map<void*, Node, std::less<void*>, MyAllocator> info;

#define BACKTRACE(p) do {\
    Node node; \
    node.depth = backtrace(node.buf, node.s_depth); \
    info[p] = node; \
} while(0)

void* __wrap_malloc(size_t size) {
    void* p = __real_malloc(size);
    BACKTRACE(p);
    return p;
}

void* __wrap_calloc(size_t nitems, size_t size) {
    void* p = __real_calloc(nitems, size);
    BACKTRACE(p);
    return p;
}

void* __wrap_realloc(void* ptr, size_t size) {
    void* p = __real_realloc(ptr, size);
    BACKTRACE(p);
    return p;
}

void free(void *ptr) {
    info.erase(ptr);    
    __real_free(ptr);
}
