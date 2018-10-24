// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License

#pragma once

#include <stddef.h>  // for size_t
#include <stdint.h>  // for uint8_t
#include <string.h>

#include "../Configuration.hpp"
#include "../Data/Slot.hpp"
#include "../Polyfills/attributes.hpp"
#include "StringSlot.hpp"

namespace ARDUINOJSON_NAMESPACE {

class SlotCache {
 public:
  SlotCache() : _head(0) {}

  Slot *pop() {
    Slot *slot = _head;
    if (slot) _head = slot->next;
    return slot;
  }

  void push(Slot *slot) {
    slot->next = _head;
    _head = slot;
  }

  size_t size() const {
    size_t sum = 0;
    for (Slot *s = _head; s; s = s->next) sum += sizeof(Slot);
    return sum;
  }

 private:
  Slot *_head;
};  // namespace ARDUINOJSON_NAMESPACE

// Handle the memory management (done in derived classes) and calls the
// parser. This abstract class is implemented by StaticMemoryPool which
// implements a fixed memory allocation.
class MemoryPool {
 public:
  // Allocates n bytes in the MemoryPool.
  // Return a pointer to the allocated memory or NULL if allocation fails.
  virtual StringSlot *allocString(size_t size) = 0;
  virtual void append(StringSlot *, char c) = 0;
  virtual void freeString(StringSlot *) {}

  virtual Slot *allocVariant() = 0;
  virtual void freeVariant(Slot *) = 0;

  virtual size_t size() const = 0;

 protected:
  // CAUTION: NO VIRTUAL DESTRUCTOR!
  // If we add a virtual constructor the Arduino compiler will add malloc()
  // and free() to the binary, adding 706 useless bytes.
  ~MemoryPool() {}

  // Preserve aligment if necessary
  static FORCE_INLINE size_t round_size_up(size_t bytes) {
#if ARDUINOJSON_ENABLE_ALIGNMENT
    const size_t x = sizeof(void *) - 1;
    return (bytes + x) & ~x;
#else
    return bytes;
#endif
  }
};
}  // namespace ARDUINOJSON_NAMESPACE
