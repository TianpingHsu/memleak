
#include "Man.h"
#include <chrono>

struct Foo {
    public:
    char name[16] = "Foo";
    Foo() = default;
};

void leak_1() {
    using namespace std::chrono;
    for (int i = 0; i < 128; i++) {
        printf("%s [%d] %p\n", __FUNCTION__, i, malloc(sizeof(Foo)));
        std::this_thread::sleep_for(10ms);
        printf("%s [%d] %p\n", __FUNCTION__, i, new Foo);
        //if (i % 2 == 0) Man::getInstance()->show();
    }
}

void leak_2() {
    using namespace std::chrono;
    for (int i = 0; i < 128; i++) {
        printf("%s [%d] %p\n", __FUNCTION__, i, new Foo);
        printf("%s [%d] %p\n", __FUNCTION__, i, malloc(sizeof(Foo)));
        std::this_thread::sleep_for(15ms);
        //if (i % 2 == 0) Man::getInstance()->show();
    }
}

int main() {
    std::thread t1(leak_1);
    std::thread t2(leak_2);

    t1.join();
    t2.join();
    printf("leak done!\n");
    Man::getInstance()->show();
    return 0;
}

