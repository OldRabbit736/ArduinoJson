// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License

#include <ArduinoJson/Memory/StaticMemoryPool.hpp>
#include <catch.hpp>

using namespace ARDUINOJSON_NAMESPACE;

TEST_CASE("StaticMemoryPool::size()") {
  SECTION("Capacity equals template parameter") {
    const size_t capacity = 64;
    StaticMemoryPool<capacity> memoryPool;
    REQUIRE(capacity == memoryPool.capacity());
  }

  SECTION("Initial size is 0") {
    StaticMemoryPool<32> memoryPool;
    REQUIRE(0 == memoryPool.size());
  }

  SECTION("Increases after allocFrozenString()") {
    StaticMemoryPool<128> memoryPool;
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
    const size_t variantCount = 4;
    const size_t capacity = variantCount * sizeof(VariantSlot);
    StaticMemoryPool<capacity> memoryPool;

    for (size_t i = 0; i < variantCount; i++) memoryPool.allocVariant();
    REQUIRE(capacity == memoryPool.size());

    memoryPool.allocVariant();

    REQUIRE(capacity == memoryPool.size());
  }
}
