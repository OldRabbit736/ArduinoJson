// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License

#include <ArduinoJson/Memory/DynamicMemoryPool.hpp>
#include <catch.hpp>
#include <sstream>

using namespace ARDUINOJSON_NAMESPACE;

TEST_CASE("DynamicMemoryPool::allocFrozenString()") {
  SECTION("Returns different pointers") {
    DynamicMemoryPool memoryPool;
    void* p1 = memoryPool.allocFrozenString(1);
    void* p2 = memoryPool.allocFrozenString(2);
    REQUIRE(p1 != p2);
  }
}
