// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License

#include <ArduinoJson/Memory/StaticMemoryPool.hpp>
#include <catch.hpp>

using namespace ARDUINOJSON_NAMESPACE;

TEST_CASE("StaticMemoryPool::size()") {
  StaticMemoryPool<64> memoryPool;

  SECTION("Capacity equals template parameter") {
    REQUIRE(64 == memoryPool.capacity());
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

  SECTION("Doesn't grow when memoryPool is full") {
    memoryPool.allocString(64);
    memoryPool.allocString(1);
    REQUIRE(64 == memoryPool.size());
  }

  SECTION("Does't grow when memoryPool is too small for alloc") {
    memoryPool.allocString(65);
    REQUIRE(0 == memoryPool.size());
  }

  SECTION("Goes back to zero after clear()") {
    memoryPool.allocString(1);
    memoryPool.clear();
    REQUIRE(0 == memoryPool.size());
  }
}
