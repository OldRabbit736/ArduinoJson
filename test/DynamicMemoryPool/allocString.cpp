// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License

#include <ArduinoJson/Memory/DynamicMemoryPool.hpp>
#include <catch.hpp>
#include <sstream>

using namespace ARDUINOJSON_NAMESPACE;

static bool isAligned(void* ptr) {
  const size_t mask = sizeof(void*) - 1;
  size_t addr = reinterpret_cast<size_t>(ptr);
  return (addr & mask) == 0;
}

TEST_CASE("DynamicMemoryPool::allocFrozenString()") {
  SECTION("Returns different pointers") {
    DynamicMemoryPool memoryPool;
    void* p1 = memoryPool.allocFrozenString(1);
    void* p2 = memoryPool.allocFrozenString(2);
    REQUIRE(p1 != p2);
  }

  SECTION("Alignment") {
    // make room for two but not three
    DynamicMemoryPool tinyBuf(2 * sizeof(VariantSlot));

    REQUIRE(
        isAligned(tinyBuf.allocVariant()));  // this one is aligned by design
    REQUIRE(
        isAligned(tinyBuf.allocVariant()));  // this one fits in the first block
    REQUIRE(
        isAligned(tinyBuf.allocVariant()));  // this one requires a new block
  }
}
