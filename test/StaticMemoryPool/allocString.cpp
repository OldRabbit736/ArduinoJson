// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License

#include <ArduinoJson/Memory/StaticMemoryPool.hpp>
#include <catch.hpp>

using namespace ARDUINOJSON_NAMESPACE;

static const size_t poolCapacity = 64;
static const size_t longestString = poolCapacity - sizeof(StringSlot);

TEST_CASE("StaticMemoryPool::allocFrozenString()") {
  StaticMemoryPool<poolCapacity> memoryPool;

  SECTION("Returns different addresses") {
    void *p1 = memoryPool.allocFrozenString(0);
    void *p2 = memoryPool.allocFrozenString(0);
    REQUIRE(p1 != p2);
  }

  SECTION("Returns NULL when full") {
    memoryPool.allocFrozenString(longestString);
    void *p = memoryPool.allocFrozenString(1);
    REQUIRE(0 == p);
  }

  SECTION("Returns NULL when memoryPool is too small") {
    void *p = memoryPool.allocFrozenString(longestString + 1);
    REQUIRE(0 == p);
  }

  SECTION("Returns aligned pointers") {
    for (size_t size = 1; size <= sizeof(void *); size++) {
      void *p = memoryPool.allocFrozenString(1);
      REQUIRE(isAligned(p));
    }
  }

  SECTION("Returns same address after clear()") {
    void *p1 = memoryPool.allocFrozenString(1);
    memoryPool.clear();
    void *p2 = memoryPool.allocFrozenString(1);
    REQUIRE(p1 == p2);
  }
}
