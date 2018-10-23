// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License

#pragma once

#include "../Polyfills/mpl/max.hpp"
#include "../Strings/StringInMemoryPool.hpp"
#include "MemoryPool.hpp"
#include "StringBuilder.hpp"

namespace ARDUINOJSON_NAMESPACE {

class StaticMemoryPoolBase : public MemoryPool {
 public:
  // Gets the capacity of the memoryPool in bytes
  size_t capacity() const {
    return _capacity;
  }

  virtual size_t size() const {
    return allocated_bytes() - _cache.size();
  }

  virtual Slot* allocVariant() {
    Slot* s = _cache.pop();
    if (s) return s;
    alignNextAlloc();
    if (!canAlloc(sizeof(Slot))) return 0;
    return s ? s : reinterpret_cast<Slot*>(doAlloc(sizeof(Slot)));
  }

  virtual void freeVariant(Slot* slot) {
    _cache.push(slot);
  }

  virtual StringSlot* allocString(size_t len) {
    const size_t bytes = len + sizeof(StringSlot);
    alignNextAlloc();
    if (!canAlloc(bytes)) return NULL;
    char* block = doAlloc(bytes);
    if (!block) return 0;

    StringSlot* s = reinterpret_cast<StringSlot*>(block);
    if (!s) return 0;

    s->value = block + sizeof(StringSlot);
    s->size = len;
    return s;
  }

  virtual void append(StringSlot* slot, char c) {
    if (!slot) return;
    if (canAlloc(1)) {
      doAlloc(1);
      slot->value[slot->size++] = c;
    } else {
      slot->value = 0;
    }
  }

  // Resets the memoryPool.
  // USE WITH CAUTION: this invalidates all previously allocated data
  void clear() {
    _size = 0;
  }

  StringBuilder startString() {
    return StringBuilder(this);
  }

 protected:
  StaticMemoryPoolBase(char* memoryPool, size_t capa)
      : _buffer(memoryPool), _capacity(capa), _size(0) {}

  ~StaticMemoryPoolBase() {}

  // Gets the current usage of the memoryPool in bytes
  virtual size_t allocated_bytes() const {
    return _size;
  }

 private:
  void alignNextAlloc() {
    _size = round_size_up(_size);
  }

  bool canAlloc(size_t bytes) const {
    return _size + bytes <= _capacity;
  }

  char* doAlloc(size_t bytes) {
    char* p = &_buffer[_size];
    _size += bytes;
    return p;
  }

  template <typename T>
  T* alloc(size_t n) {
    const size_t bytes = n * sizeof(T);
    alignNextAlloc();
    if (!canAlloc(bytes)) return NULL;
    return reinterpret_cast<T*>(doAlloc(bytes));
  }

  char* _buffer;
  size_t _capacity;
  size_t _size;
  SlotCache _cache;
};

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnon-virtual-dtor"
#elif defined(__GNUC__)
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#endif
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#endif

// Implements a MemoryPool with fixed memory allocation.
// The template paramenter CAPACITY specifies the capacity of the memoryPool in
// bytes.
template <size_t CAPACITY>
class StaticMemoryPool : public StaticMemoryPoolBase {
  static const size_t ACTUAL_CAPACITY = Max<1, CAPACITY>::value;

 public:
  explicit StaticMemoryPool()
      : StaticMemoryPoolBase(_buffer, ACTUAL_CAPACITY) {}

 private:
  char _buffer[ACTUAL_CAPACITY];
};
}  // namespace ARDUINOJSON_NAMESPACE

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
#endif
