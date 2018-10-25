// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License

#include <ArduinoJson/Memory/DynamicMemoryPool.hpp>
#include <catch.hpp>

using namespace ARDUINOJSON_NAMESPACE;

TEST_CASE("DynamicMemoryPool::startString()") {
  SECTION("WorksWhenBufferIsBigEnough") {
    DynamicMemoryPool memoryPool(JSON_STRING_SIZE(8));

    StringBuilder str = memoryPool.startString();
    str.append('a');
    str.append('b');
    str.append('c');
    str.append('d');
    str.append('e');
    str.append('f');
    str.append('g');

    REQUIRE(memoryPool.blockCount() == 1);
    REQUIRE(str.complete().equals("abcdefg"));
  }

  SECTION("GrowsWhenBufferIsTooSmall") {
    DynamicMemoryPool memoryPool(JSON_STRING_SIZE(8));

    StringBuilder str = memoryPool.startString();
    str.append('a');
    str.append('b');
    str.append('c');
    str.append('d');
    str.append('e');
    str.append('f');
    str.append('g');
    str.append('h');
    str.append('A');  // force a new block
    str.append('B');
    str.append('C');

    REQUIRE(memoryPool.blockCount() == 2);
    REQUIRE(str.complete().equals("abcdefghABC"));
  }

  SECTION("SizeIncreases") {
    DynamicMemoryPool memoryPool(JSON_STRING_SIZE(5));

    StringBuilder str = memoryPool.startString();
    str.append('h');
    str.complete();

    REQUIRE(JSON_STRING_SIZE(2) == memoryPool.size());
  }
}
