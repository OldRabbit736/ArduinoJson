// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License

#include <ArduinoJson/Memory/StaticMemoryPool.hpp>
#include <catch.hpp>

using namespace ARDUINOJSON_NAMESPACE;

static const size_t poolCapacity = 64;
static const size_t longestString = poolCapacity - sizeof(StringSlot);
static const size_t maxVariants = poolCapacity / sizeof(VariantSlot);

TEST_CASE("StaticMemoryPool::size()") {
  StaticMemoryPool<poolCapacity> memoryPool;

  SECTION("Capacity equals template parameter") {
    REQUIRE(poolCapacity == memoryPool.capacity());
  }

  SECTION("Initial size is 0") {
    REQUIRE(0 == memoryPool.size());
  }

  SECTION("Increases after allocFrozenString()") {
    memoryPool.allocFrozenString(0);
    REQUIRE(memoryPool.size() == JSON_STRING_SIZE(0));
    memoryPool.allocFrozenString(0);
    REQUIRE(memoryPool.size() == 2 * JSON_STRING_SIZE(0));
  }

  // SECTION("Decreases after freeString()") {
  //   StringSlot* s1 = memoryPool.allocFrozenString(0);
  //   StringSlot* s2 = memoryPool.allocFrozenString(1);

  //   memoryPool.freeString(s1);
  //   REQUIRE(memoryPool.size() == JSON_STRING_SIZE(0));
  //   memoryPool.freeString(s2);
  //   REQUIRE(memoryPool.size() == 0);
  // }

  SECTION("Doesn't grow when memoryPool is full") {
    for (size_t i = 0; i < maxVariants; i++) memoryPool.allocVariant();
    REQUIRE(poolCapacity == memoryPool.size());
    memoryPool.allocVariant();
    REQUIRE(poolCapacity == memoryPool.size());
  }

  SECTION("Goes back to zero after clear()") {
    memoryPool.allocVariant();
    memoryPool.clear();
    REQUIRE(0 == memoryPool.size());
  }
}
