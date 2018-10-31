// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License

#include <ArduinoJson/Memory/StaticMemoryPool.hpp>
#include <catch.hpp>

using namespace ARDUINOJSON_NAMESPACE;

static const size_t poolCapacity = 64;

TEST_CASE("StaticMemoryPool::clear()") {
  StaticMemoryPool<poolCapacity> memoryPool;

  SECTION("Discards allocated variants") {
    memoryPool.allocVariant();

    memoryPool.clear();
    REQUIRE(memoryPool.size() == 0);
  }

  SECTION("Discards allocated strings") {
    memoryPool.allocFrozenString(10);

    memoryPool.clear();

    REQUIRE(memoryPool.size() == 0);
  }

  SECTION("Purges variant cache") {
    memoryPool.freeVariant(memoryPool.allocVariant());

    memoryPool.clear();

    REQUIRE(memoryPool.size() == 0);
  }

  SECTION("Purges string cache") {
    memoryPool.freeString(memoryPool.allocFrozenString(10));

    memoryPool.clear();

    REQUIRE(memoryPool.size() == 0);
  }
}
