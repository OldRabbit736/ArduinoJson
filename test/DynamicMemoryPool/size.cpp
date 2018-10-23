// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License

#include <ArduinoJson/Memory/DynamicMemoryPool.hpp>
#include <catch.hpp>

using namespace ARDUINOJSON_NAMESPACE;

TEST_CASE("DynamicMemoryPool::size()") {
  DynamicMemoryPool memoryPool;

  SECTION("Initial size is 0") {
    REQUIRE(0 == memoryPool.size());
  }

  SECTION("Increases after allocString()") {
    memoryPool.allocString(1);
    REQUIRE(1U <= memoryPool.size());
    memoryPool.allocString(1);
    REQUIRE(2U <= memoryPool.size());
  }

  SECTION("Goes back to 0 after clear()") {
    memoryPool.allocString(1);
    memoryPool.allocVariant();
    memoryPool.clear();
    REQUIRE(0 == memoryPool.size());
  }

  SECTION("Increases after allocVariant()") {
    memoryPool.allocVariant();
    REQUIRE(sizeof(Slot) == memoryPool.size());

    memoryPool.allocVariant();
    REQUIRE(2 * sizeof(Slot) == memoryPool.size());
  }

  SECTION("Decreases after freeSlot()") {
    Slot* s1 = memoryPool.allocVariant();
    Slot* s2 = memoryPool.allocVariant();

    memoryPool.freeSlot(s1);
    REQUIRE(sizeof(Slot) == memoryPool.size());

    memoryPool.freeSlot(s2);
    REQUIRE(0 == memoryPool.size());
  }
}
