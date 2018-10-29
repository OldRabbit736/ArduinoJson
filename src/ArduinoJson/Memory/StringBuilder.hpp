// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License

#pragma once

#include "../Strings/StringInMemoryPool.hpp"
#include "MemoryPool.hpp"

namespace ARDUINOJSON_NAMESPACE {

class StringBuilder {
 public:
  typedef StringInMemoryPool StringType;

  explicit StringBuilder(MemoryPool* parent) : _parent(parent) {
    _slot = _parent->allocString(0);
  }

  void append(char c) {
    _slot = _parent->append(_slot, c);
  }

  StringType complete() {
    _slot = _parent->append(_slot, 0);
    return _slot;
  }

 private:
  MemoryPool* _parent;
  StringSlot* _slot;
};

}  // namespace ARDUINOJSON_NAMESPACE
