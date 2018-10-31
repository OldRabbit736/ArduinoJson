// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License

#pragma once

#include "../Polyfills/mpl/max.hpp"
#include "../Strings/StringInMemoryPool.hpp"
#include "Alignment.hpp"
#include "MemoryPool.hpp"

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

  virtual VariantSlot* allocVariant() {
    VariantSlot* s = _freeVariants.pop();
    if (s) return s;
    return s ? s : allocRight<VariantSlot>();
  }

  virtual void freeVariant(VariantSlot* slot) {
    _freeVariants.push(slot);
  }

  virtual StringSlot* allocFrozenString(size_t n) {
    StringSlot* s = allocRight<StringSlot>();
    if (!s) return 0;

    if (!canAlloc(n)) return 0;

    s->value = _left;
    s->size = n;
    _left += n;

    return s;
  }

  virtual StringSlot* allocExpandableString() {
    StringSlot* s = allocRight<StringSlot>();
    if (!s) return 0;

    s->value = _left;
    s->size = size_t(_right - _left);
    return s;
  }

  virtual StringSlot* expandString(StringSlot*) {
    return 0;
  }

  virtual void freezeString(StringSlot* slot) {
    _left = slot->value + slot->size;
  }

  // Resets the memoryPool.
  // USE WITH CAUTION: this invalidates all previously allocated data
  void clear() {
    _left = _begin;
    _right = _end;
    _freeVariants.clear();
  }

  bool canAlloc(size_t bytes) const {
    return _left + bytes <= _right;
  }

  bool owns(void* p) const {
    return _begin <= p && p < _end;
  }

  template <typename T>
  T* allocRight() {
    return reinterpret_cast<T*>(allocRight(sizeof(T)));
  }

  char* allocRight(size_t bytes) {
    if (!canAlloc(bytes)) return 0;
    _right -= bytes;
    return _right;
  }

  // Workaround for missing placement new
  void* operator new(size_t, void* p) {
    return p;
  }

 protected:
  StaticMemoryPoolBase(char* buffer, size_t capa)
      : _begin(buffer),
        _left(buffer),
        _right(buffer + capa),
        _end(buffer + capa) {}

  ~StaticMemoryPoolBase() {}

  // Gets the current usage of the memoryPool in bytes
  size_t allocated_bytes() const {
    return size_t(_left - _begin + _end - _right);
  }

 private:
  char *_begin, *_left, *_right, *_end;
  SlotCache<VariantSlot> _freeVariants;
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
  static const size_t ACTUAL_CAPACITY =
      AddPadding<Max<1, CAPACITY>::value>::value;

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
