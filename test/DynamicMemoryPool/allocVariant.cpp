// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License

#include <ArduinoJson/Memory/DynamicMemoryPool.hpp>
#include <catch.hpp>

using namespace ARDUINOJSON_NAMESPACE;

TEST_CASE("DynamicMemoryPool::allocVariant()") {
  DynamicMemoryPool memoryPool;

  SECTION("Returns different pointer") {
    Slot* s1 = memoryPool.allocVariant();
    Slot* s2 = memoryPool.allocVariant();

    REQUIRE(s1 != s2);
  }

  SECTION("Returns same pointer after freeSlot()") {
    Slot* s1 = memoryPool.allocVariant();
    memoryPool.freeSlot(s1);
    Slot* s2 = memoryPool.allocVariant();

    REQUIRE(s1 == s2);
  }
}
