
#pragma once

#include <memory>
#include <cstdio>

namespace POC {

    static uint8_t s_DATA[0x01 << 20];  // definitely not thread-safe
    template <class T> class StaticArrayAllocator {
        /** 
         * the `Chunk` struct comes from: 
         * http://dmitrysoshnikov.com/compilers/writing-a-pool-allocator/
         */
        struct Chunk {  // this struct is pretty tricky
            /**
             * When a chunk is free, the `next` contains the
             * address of the next chunk in a list.
             *
             * When it's allocated, this space is used by
             * the user.
             */
            Chunk* next;
        };

        private:
        Chunk *firstFreeChunk = nullptr;
        std::size_t usedChunks = 0;

        public:
        StaticArrayAllocator() = default;
        StaticArrayAllocator(StaticArrayAllocator && ) = delete;  // move ctor
        StaticArrayAllocator(const StaticArrayAllocator&) = delete;  // copy ctor
        StaticArrayAllocator operator=(const StaticArrayAllocator&&) = delete; // assignment operator
        StaticArrayAllocator operator=(const StaticArrayAllocator&) = delete; // assignment operator

        ~StaticArrayAllocator() {
            // do nothing
        }

        T *_allocate() {
            /**
             * `firstFreeChunk` will always be nullptr, unless `deallocate` is called
             */
            if (firstFreeChunk) {
                Chunk* p = firstFreeChunk;
                firstFreeChunk= firstFreeChunk->next;
                return reinterpret_cast<T*>(p);
            }

            /**
             * usedChunks is initialized to be totalChunks,
             * so when first allocation happens, a block will be newed.
             */
            if (usedChunks >= sizeof(s_DATA)/sizeof(T)) {
                /**
                 * single list will be constructed automatically
                 */
                return nullptr;  // no empty space left
            }

            T* p = reinterpret_cast<T*>(&s_DATA[(usedChunks++) * sizeof(T)]);
            //printf("%p, s_DATA[%d]: %p\n", p, (usedChunks - 1) * sizeof(T), &s_DATA[(usedChunks - 1) * sizeof(T)]);
            return p;
        }

        void _deallocate(T* p) {
            /**
             * this is a little tricky,
             * free chunk list is automatically constructed.
             */
            Chunk *pchunk = reinterpret_cast<Chunk*>(p);
            pchunk->next = firstFreeChunk;
            firstFreeChunk = pchunk;
            //printf("free: %p, used: %ul, total: %ul\n", p, usedChunks, sizeof(s_DATA)/sizeof(T));
        }

        public:
        typedef T           value_type;
        typedef T*          pointer;
        typedef const T*    const_pointer;
        typedef T&          reference;
        typedef const T&    const_reference;
        typedef std::size_t size_type;
        // https://en.cppreference.com/w/cpp/types/ptrdiff_t
        typedef ptrdiff_t   difference_type;

        // rebind allocator of type U
        // https://stackoverflow.com/questions/14148756/what-does-template-rebind-do
        template <class U>
            struct rebind {
                //typedef StaticArrayAllocator<U> other;
                typedef StaticArrayAllocator<U> other;
            };

        /**
         * hint used for locality. ref.[Austern], p189
         * https://en.cppreference.com/w/cpp/memory/allocator/allocate
         * The pointer `hint` may be used to provide locality of reference: 
         *      the allocator, if supported by the implementation, 
         *      will attempt to allocate the new memory block as close as possible to hint.
         */
        pointer allocate(size_type n, const void* hint = 0) {
            if (n != 1 || hint) throw std::bad_alloc(); 
            return _allocate();
        }

        void deallocate(pointer p, size_type n) {
            _deallocate(p);
        }

        void construct(pointer p, const_reference val) {
            //https://stackoverflow.com/questions/222557/what-uses-are-there-for-placement-new
            //https://stackoverflow.com/questions/58900136/custom-allocator-including-placement-new-case
            new (p) T(val);  // placement new
        }

        void destroy(pointer p) {
            p->~T();
        }
    };

}

