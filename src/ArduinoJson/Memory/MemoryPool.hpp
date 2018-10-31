// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License

#pragma once

#include <stddef.h>  // for size_t
#include <stdint.h>  // for uint8_t
#include <string.h>

#include "../Configuration.hpp"
#include "../Polyfills/attributes.hpp"
#include "StringSlot.hpp"
#include "VariantSlot.hpp"

namespace ARDUINOJSON_NAMESPACE {

template <typename TSlot>
class SlotCache {
 public:
  SlotCache() : _head(0) {}

  TSlot *pop() {
    TSlot *slot = _head;
    if (slot) _head = slot->next;
    return slot;
  }

  void push(TSlot *slot) {
    slot->next = _head;
    _head = slot;
  }

  void remove(TSlot *slot) {
    if (_head == slot) {
      _head = slot->next;
      return;
    }

    for (TSlot *s = _head; s; s = s->next) {
      if (s->next == slot) {
        s->next = slot->next;
        return;
      }
    }
  }

  template <typename Functor>
  void forEach(const Functor &f) {
    for (TSlot *s = _head; s; s = s->next) {
      f(s);
    }
  }

  size_t size() const {
    size_t sum = 0;
    for (TSlot *s = _head; s; s = s->next) sum += sizeof(TSlot);
    return sum;
  }

  void clear() {
    _head = 0;
  }

 private:
  TSlot *_head;
};

// Handle the memory management (done in derived classes) and calls the
// parser. This abstract class is implemented by StaticMemoryPool which
// implements a fixed memory allocation.
class MemoryPool {
 public:
  // Allocates n bytes in the MemoryPool.
  // Return a pointer to the allocated memory or NULL if allocation fails.
  virtual StringSlot *allocExpandableString() = 0;
  virtual StringSlot *allocFrozenString(size_t) = 0;
  virtual StringSlot *expandString(StringSlot *) = 0;
  virtual void freezeString(StringSlot *) = 0;
  virtual void freeString(StringSlot *) {}

  virtual VariantSlot *allocVariant() = 0;
  virtual void freeVariant(VariantSlot *) = 0;

  virtual size_t size() const = 0;

 protected:
  // CAUTION: NO VIRTUAL DESTRUCTOR!
  // If we add a virtual constructor the Arduino compiler will add malloc()
  // and free() to the binary, adding 706 useless bytes.
  ~MemoryPool() {}
};
}  // namespace ARDUINOJSON_NAMESPACE
