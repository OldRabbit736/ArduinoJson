// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License

#pragma once

#include <stddef.h>  // for size_t
#include "../Configuration.hpp"

#define MASK (sizeof(void *) - 1)
#define ADD_PADDING(N) (((N) + MASK) & ~MASK)

#define JSON_STRING_SIZE(SIZE) \
  (sizeof(ARDUINOJSON_NAMESPACE::StringSlot) + ADD_PADDING(SIZE))

namespace ARDUINOJSON_NAMESPACE {

struct StringSlot {
  char *value;
  size_t size;
  // struct StringSlot *next;
};
}  // namespace ARDUINOJSON_NAMESPACE
