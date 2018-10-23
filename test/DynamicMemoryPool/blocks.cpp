// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>
#include <sstream>

using namespace ARDUINOJSON_NAMESPACE;

std::stringstream allocatorLog;

struct SpyingAllocator : DefaultAllocator {
  void* allocate(size_t n) {
    allocatorLog << "A" << (n - DynamicMemoryPool::EmptyBlockSize);
    return DefaultAllocator::allocate(n);
  }
  void deallocate(void* p) {
    allocatorLog << "F";
    return DefaultAllocator::deallocate(p);
  }
};

TEST_CASE("DynamicMemoryPool blocks") {
  SECTION("Doubles allocation size when full") {
    allocatorLog.str("");
    {
      DynamicMemoryPoolBase<SpyingAllocator> memoryPool(sizeof(Slot));
      memoryPool.allocVariant();
      memoryPool.allocVariant();
    }
    std::stringstream expected;
    expected << "A" << sizeof(Slot)      // block 1
             << "A" << 2 * sizeof(Slot)  // block 2, twice bigger
             << "FF";

    REQUIRE(allocatorLog.str() == expected.str());
  }

  SECTION("Resets allocation size after clear()") {
    allocatorLog.str("");
    {
      DynamicMemoryPoolBase<SpyingAllocator> memoryPool(sizeof(Slot));
      memoryPool.allocVariant();
      memoryPool.allocVariant();
      memoryPool.clear();
      memoryPool.allocVariant();
    }
    std::stringstream expected;
    expected << "A" << sizeof(Slot)      // block 1
             << "A" << 2 * sizeof(Slot)  // block 2, twice bigger
             << "FF"                     // clear
             << "A" << sizeof(Slot)      // block 1
             << "F";
    REQUIRE(allocatorLog.str() == expected.str());
  }

  SECTION("Alloc big block for large string") {
    allocatorLog.str("");
    {
      DynamicMemoryPoolBase<SpyingAllocator> memoryPool(1);
      memoryPool.allocString(42);
    }
    std::stringstream expected;
    expected << "A" << sizeof(StringSlot)  // block 1
             << "A" << 42                  // block 2
             << "FF";
    REQUIRE(allocatorLog.str() == expected.str());
  }
}
