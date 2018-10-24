// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License

#pragma once

#include <stddef.h>  // for size_t
#include "../Configuration.hpp"

#define JSON_STRING_SIZE(SIZE) \
  (ARDUINOJSON_NAMESPACE::StringSizePadded<(SIZE)>::value)

namespace ARDUINOJSON_NAMESPACE {

struct StringSlot {
  char *value;
  size_t size;
  // struct StringSlot *next;
};

template <size_t Size>
struct AddPadding {
  enum mask_type { mask = sizeof(void *) - 1 };
  enum value_type { value = (Size + mask) & ~mask };
};

template <size_t Size>
struct StringSizeUnpadded {
  enum { value = Size + sizeof(StringSlot) };
};

template <size_t Size>
struct StringSizePadded : AddPadding<StringSizeUnpadded<Size>::value> {};
}  // namespace ARDUINOJSON_NAMESPACE
