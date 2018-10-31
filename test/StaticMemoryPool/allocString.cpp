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
    StringSlot *a = pool.allocFrozenString(1);
    StringSlot *b = pool.allocFrozenString(1);
    REQUIRE(a != b);
    REQUIRE(a->value != b->value);
  }

  SECTION("Returns a StringSlot of the right size") {
    StringSlot *s = pool.allocFrozenString(42);
    REQUIRE(s->size == 42);
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
    REQUIRE(a->value == b->value);
  }

  SECTION("Returns same address after freeString()") {
    StringSlot *a = pool.allocFrozenString(1);
    pool.freeString(a);
    StringSlot *b = pool.allocFrozenString(1);

    REQUIRE(a == b);
    REQUIRE(a->value == b->value);
  }

  SECTION("Can use full capacity when fresh") {
    StringSlot *a = pool.allocFrozenString(longestString);

    REQUIRE(a != NULL);
  }

  SECTION("Can use full capacity after clear") {
    pool.allocFrozenString(longestString);
    pool.clear();

    StringSlot *a = pool.allocFrozenString(longestString);

    REQUIRE(a != NULL);
  }
}

TEST_CASE("StaticMemoryPool::freeString()") {
  StaticMemoryPool<poolCapacity> pool;

  SECTION("Restores full capacity") {
    StringSlot *a = pool.allocFrozenString(longestString);
    pool.freeString(a);

    StringSlot *b = pool.allocFrozenString(longestString);

    REQUIRE(b != NULL);
    REQUIRE(b->size == longestString);
  }

  SECTION("Move strings") {
    StringSlot *a = pool.allocFrozenString(6);
    strcpy(a->value, "hello");

    StringSlot *b = pool.allocFrozenString(7);
    strcpy(b->value, "world!");
    pool.freeString(a);

    REQUIRE(b->size == 7);
    REQUIRE(b->value == std::string("world!"));
    REQUIRE(a->value == b->value);
  }
}
