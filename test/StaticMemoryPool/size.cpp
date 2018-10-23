// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License

#include <ArduinoJson/Memory/StaticMemoryPool.hpp>
#include <catch.hpp>

using namespace ARDUINOJSON_NAMESPACE;

static const size_t poolCapacity = 64;
static const size_t longestString = poolCapacity - sizeof(StringSlot);

TEST_CASE("StaticMemoryPool::size()") {
  StaticMemoryPool<poolCapacity> memoryPool;

  SECTION("Capacity equals template parameter") {
    REQUIRE(poolCapacity == memoryPool.capacity());
  }

  SECTION("Initial size is 0") {
    REQUIRE(0 == memoryPool.size());
  }

  SECTION("Increases after allocString()") {
    memoryPool.allocString(1);
    REQUIRE(1U <= memoryPool.size());
    memoryPool.allocString(1);
    REQUIRE(2U <= memoryPool.size());
  }

  // SECTION("Decreases after freeString()") {
  //   StringSlot* s1 = memoryPool.allocString(1);
  //   StringSlot* s2 = memoryPool.allocString(1);

  //   memoryPool.freeString(s1);
  //   REQUIRE(1U <= memoryPool.size());
  //   memoryPool.freeString(s2);
  //   REQUIRE(2U <= memoryPool.size());
  // }

  SECTION("Doesn't grow when memoryPool is full") {
    memoryPool.allocString(longestString);
    memoryPool.allocString(1);
    REQUIRE(64 == memoryPool.size());
  }

  SECTION("Does't grow when memoryPool is too small for alloc") {
    memoryPool.allocString(longestString+1);
    REQUIRE(0 == memoryPool.size());
  }

  SECTION("Goes back to zero after clear()") {
    memoryPool.allocString(1);
    memoryPool.allocVariant();
    memoryPool.clear();
    REQUIRE(0 == memoryPool.size());
  }
}
