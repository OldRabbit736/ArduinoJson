// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License

#include <ArduinoJson/Memory/StaticMemoryPool.hpp>
#include <catch.hpp>

using namespace ARDUINOJSON_NAMESPACE;

static const size_t poolCapacity = 64;
static const size_t longestString = poolCapacity - sizeof(StringSlot);

TEST_CASE("StaticMemoryPool::allocFrozenString()") {
  StaticMemoryPool<poolCapacity> pool;

  SECTION("Returns different addresses") {
    void *a = pool.allocFrozenString(0);
    void *b = pool.allocFrozenString(0);
    REQUIRE(a != b);
  }

  SECTION("Returns NULL when full") {
    pool.allocFrozenString(longestString);
    void *p = pool.allocFrozenString(1);
    REQUIRE(0 == p);
  }

  SECTION("Returns NULL when pool is too small") {
    void *p = pool.allocFrozenString(longestString + 1);
    REQUIRE(0 == p);
  }

  SECTION("Returns aligned pointers") {
    REQUIRE(isAligned(pool.allocFrozenString(1)));
    REQUIRE(isAligned(pool.allocFrozenString(1)));
  }

  SECTION("Returns same address after clear()") {
    StringSlot *a = pool.allocFrozenString(1);
    pool.clear();
    StringSlot *b = pool.allocFrozenString(1);

    REQUIRE(a == b);
  }

  SECTION("Returns same address after freeString()") {
    StringSlot *a = pool.allocFrozenString(1);
    pool.freeString(a);
    StringSlot *b = pool.allocFrozenString(1);

    REQUIRE(a == b);
  }
}
