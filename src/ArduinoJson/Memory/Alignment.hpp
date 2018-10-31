// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License

#pragma once

#include <stddef.h>  // size_t

namespace ARDUINOJSON_NAMESPACE {

inline size_t addPadding(size_t bytes) {
  const size_t mask = sizeof(void *) - 1;
  return (bytes + mask) & ~mask;
}

template <size_t bytes>
struct AddPadding {
  static const size_t mask = sizeof(void *) - 1;
  static const size_t value = (bytes + mask) & ~mask;
};

}  // namespace ARDUINOJSON_NAMESPACE
