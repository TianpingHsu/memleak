
#pragma once
#include <mutex>
#include <thread>

#include <map>
#include "StaticArrayAllocator.h"
#include <cstdio>
#include <iostream>
#include <execinfo.h>

extern "C" {
    void* __wrap_malloc(size_t);
    void* __wrap_calloc(size_t, size_t);
    void* __wrap_realloc(void*, size_t);
    void __wrap_free(void*);

    void* __real_malloc(size_t);
    void* __real_calloc(size_t, size_t);
    void* __real_realloc(void*, size_t);
    void __real_free(void*);

    // c++filt: _Znwm ==> operator new(unsigned long)
    void* __wrap__Znwm(std::size_t size);
    // c++filt _ZdlPv ==> operator delete(void*)
    void __wrap__ZdlPv(void* ptr);
    // ===> operator new[](std::size_t)
    void* __wrap__Znam(std::size_t);
    // ===> operator delete[](void*)
    void __wrap__ZdaPv(void*);

    void* __real__Znwm(std::size_t size);
    void __real__ZdlPv(void* ptr);
    void* __real__Znam(std::size_t);
    void __real__ZdaPv(void*);
}

struct Node {
    public:
        const static int s_depth = 40;
        void* buf[s_depth] = {0};
        int depth = 0;
        std::thread::id tid;
        void show() {
        }
};

template <class T>
class Singleton {
    public:
        static T* getInstance() {
            static T instance;
            return &instance;
        }

    protected:
        Singleton() = default;
        Singleton(const Singleton&) = default;
        Singleton& operator=(const Singleton&) = default;
        ~Singleton() = default;
};

class Man : public Singleton<Man>
{
    public:
        void show() {
            printf("size: %zu\n", info.size());
            for (const auto & it : info) {
                printf("%p\t", it.first);
                for (int i = 0; i < it.second.depth; i++) {
                    printf("%p ", it.second.buf[i]);
                }
                printf("\n");
            }
        }

        void bt(void *p) {
            Node node;
            node.depth = backtrace(node.buf, node.s_depth);
            node.tid = std::this_thread::get_id();
            info[p] = node;
        }

        void kick(void *p) {
            //printf("kick: %p", p);
            info.erase(p);
        }
    private:
        typedef POC::StaticArrayAllocator<std::map<void*, Node>::value_type> MyAllocator;
        std::map<void*, Node, std::less<void*>, MyAllocator> info;
};

