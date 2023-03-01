#include <iostream>
#include <chrono>
#include <random>

#include <set>
#include <map>
#include <list>
#include <forward_list>

#include "StaticArrayAllocator.h"

const size_t numberOfIterations = 1024;
const size_t randomRange = 1024;

template <typename Container>
class PerformanceTest
{
    virtual void testIteration(size_t newSize) = 0;

    protected:

        Container container;

        std::default_random_engine randomNumberGenerator;
        std::uniform_int_distribution<size_t> randomNumberDistribution;

    public:

        PerformanceTest() :
            randomNumberGenerator(0),
            randomNumberDistribution(0, randomRange)
        {
        }

        virtual ~PerformanceTest()
        {
        }

        double run()
        {
            auto from = std::chrono::high_resolution_clock::now();

            for (size_t i = 0; i < numberOfIterations; i++)
                testIteration(randomNumberDistribution(randomNumberGenerator));

            auto to = std::chrono::high_resolution_clock::now();
            return std::chrono::duration_cast<std::chrono::duration<double>>(to - from).count();
        }
};

template <typename Container>
class PushFrontTest : public PerformanceTest<Container>
{
    virtual void testIteration(size_t newSize)
    {
        size_t size = 0;

        while (size < newSize)
            this->container.push_front(size++);

        for (; size > newSize; size--)
            this->container.pop_front();
    }
};

template <typename Container>
class PushBackTest : public PerformanceTest<Container>
{
    virtual void testIteration(size_t newSize)
    {
        size_t size = 0;

        while (size < newSize)
            this->container.push_back(size++);

        for (; size > newSize; size--)
            this->container.pop_back();
    }
};

template <typename Container>
class MapTest : public PerformanceTest<Container>
{
    virtual void testIteration(size_t newSize)
    {
        size_t size = 0;

        for (; size < newSize; size++)
            this->container.insert(typename Container::value_type(size, size));

        while (size > newSize)
            this->container.erase(--size);
    }
};

template <typename Container>
class SetTest : public PerformanceTest<Container>
{
    virtual void testIteration(size_t newSize)
    {
        size_t size = 0;

        while (size < newSize)
            this->container.insert(size++);

        while (size > newSize)
            this->container.erase(--size);
    }
};

template <typename StlStaticArrayAllocatorContainer, typename POCStaticArrayAllocatorContainer>
void printTestStatus(const char *name, StlStaticArrayAllocatorContainer &stlContainer, POCStaticArrayAllocatorContainer &fastContainer)
{
    double stlRunTime = stlContainer.run();
    double POCRunTime = fastContainer.run();

    std::cout << std::fixed;
    std::cout << name << " - Default STL StaticArrayAllocator : " << stlRunTime << " seconds." << std::endl;
    std::cout << name << " - Memory Pool StaticArrayAllocator : " << POCRunTime << " seconds." << std::endl;
    std::cout << name << " - Gain : x" << stlRunTime / POCRunTime << "." << std::endl;
    std::cout << std::endl;
}

int main()
{
    typedef size_t DataType;
    //typedef POC::StaticArrayAllocator<DataType> MemoryPoolStaticArrayAllocator;
    typedef POC::StaticArrayAllocator<std::map<DataType, DataType>::value_type> MapMemoryPoolStaticArrayAllocator;

    std::cout << "StaticArrayAllocator performance measurement example" << std::endl;
    std::cout << "Version: 1.0" << std::endl << std::endl;

    //PushFrontTest<std::forward_list<DataType>> pushFrontForwardListTestStl;
    //PushFrontTest<std::forward_list<DataType, MemoryPoolStaticArrayAllocator>> pushFrontForwardListTestFast;
    //printTestStatus("ForwardList PushFront", pushFrontForwardListTestStl, pushFrontForwardListTestFast);

    //PushFrontTest<std::list<DataType>> pushFrontListTestStl;
    //PushFrontTest<std::list<DataType, MemoryPoolStaticArrayAllocator>> pushFrontListTestFast;
    //printTestStatus("List PushFront", pushFrontListTestStl, pushFrontListTestFast);

    //PushBackTest<std::list<DataType>> pushBackListTestStl;
    //PushBackTest<std::list<DataType, MemoryPoolStaticArrayAllocator>> pushBackListTestFast;
    //printTestStatus("List PushBack", pushBackListTestStl, pushBackListTestFast);

    MapTest<std::map<DataType, DataType, std::less<DataType>>> mapTestStl;
    MapTest<std::map<DataType, DataType, std::less<DataType>, MapMemoryPoolStaticArrayAllocator>> mapTestFast;
    printTestStatus("Map", mapTestStl, mapTestFast);

    //SetTest<std::set<DataType, std::less<DataType>>> setTestStl;
    //SetTest<std::set<DataType, std::less<DataType>, MemoryPoolStaticArrayAllocator>> setTestFast;
    //printTestStatus("Set", setTestStl, setTestFast);

    return 0;
}
