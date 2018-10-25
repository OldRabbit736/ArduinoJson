// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License

#pragma once

#include "../Polyfills/mpl/max.hpp"
#include "../Strings/StringInMemoryPool.hpp"
#include "MemoryPool.hpp"
#include "StringBuilder.hpp"

namespace ARDUINOJSON_NAMESPACE {

// _begin                                _end
// v                                        v
// +-------------+--------------+-----------+
// | strings...  |   (free)     |  ...slots |
// +-------------+--------------+-----------+
//               ^              ^
//             _left          _right

class StaticMemoryPoolBase : public MemoryPool {
 public:
  // Gets the capacity of the memoryPool in bytes
  size_t capacity() const {
    return size_t(_end - _begin);
  }

  virtual size_t size() const {
    return allocated_bytes() - _freeVariants.size();
  }

  virtual Slot* allocVariant() {
    Slot* s = _freeVariants.pop();
    if (s) return s;
    return s ? s : allocRight<Slot>();
  }

  virtual void freeVariant(Slot* slot) {
    _freeVariants.push(slot);
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
    _left = _begin;
    _right = _end;
  }

  StringBuilder startString() {
    return StringBuilder(this);
  }

 protected:
  StaticMemoryPoolBase(char* buffer, size_t capa)
      : _begin(buffer),
        _left(buffer),
        _right(buffer + capa),
        _end(buffer + capa) {}

  ~StaticMemoryPoolBase() {}

  // Gets the current usage of the memoryPool in bytes
  virtual size_t allocated_bytes() const {
    return round_size_up(size_t(_left - _begin + _end - _right));
  }

 private:
  void alignNextAlloc() {
    _left = reinterpret_cast<char*>(round_size_up(size_t(_left)));
    // TODO: remove (no need to align strings)
  }

  bool canAlloc(size_t bytes) const {
    return _left + bytes <= _end;
  }

  char* doAlloc(size_t bytes) {
    char* p = _left;
    _left += bytes;
    return p;
  }

  template <typename T>
  T* allocRight() {
    if (_right - sizeof(T) < _left) return 0;
    _right -= sizeof(T);
    return reinterpret_cast<T*>(_right);
  }

  char *_begin, *_left, *_right, *_end;
  SlotCache<Slot> _freeVariants;
  SlotCache<StringSlot> _freeStrings;
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
