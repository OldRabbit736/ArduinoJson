// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License

#pragma once

#include <new>
#include "../Strings/StringInMemoryPool.hpp"
#include "MemoryPool.hpp"
#include "StaticMemoryPool.hpp"
#include "StringBuilder.hpp"

#include <stdlib.h>  // malloc, free

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnon-virtual-dtor"
#elif defined(__GNUC__)
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#endif
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#endif

namespace ARDUINOJSON_NAMESPACE {
class DefaultAllocator {
 public:
  void* allocate(size_t size) {
    return malloc(size);
  }
  void deallocate(void* pointer) {
    free(pointer);
  }
};

template <typename TAllocator>
class DynamicMemoryPoolBase : public MemoryPool {
  struct Block : StaticMemoryPoolBase {
    Block(char* buffer, size_t capacity, Block* nxt)
        : StaticMemoryPoolBase(buffer, capacity), next(nxt) {}
    Block* next;
  };

 public:
  enum { EmptyBlockSize = sizeof(Block) };

  DynamicMemoryPoolBase(size_t initialSize = ARDUINOJSON_DEFAULT_POOL_SIZE)
      : _head(NULL), _nextBlockCapacity(initialSize) {}

  ~DynamicMemoryPoolBase() {
    clear();
  }

  void reserve(size_t capacity) {
    _nextBlockCapacity = capacity;
  }

  virtual size_t size() const {
    size_t sum = 0;
    for (Block* b = _head; b; b = b->next) {
      sum += b->size();
    }
    return sum;
  }

  virtual Slot* allocVariant() {
    for (Block* b = _head; b; b = b->next) {
      Slot* s = b->allocVariant();
      if (s) return s;
    }

    if (!addNewBlock(sizeof(Slot))) return 0;

    return _head->allocVariant();
  }

  virtual void freeVariant(Slot* slot) {
    for (Block* b = _head; b; b = b->next) {
      if (b->owns(slot)) {
        b->freeVariant(slot);
        break;
      }
    }
  }

  // Allocates the specified amount of bytes in the memoryPool
  virtual StringSlot* allocString(size_t len) {
    for (Block* b = _head; b; b = b->next) {
      StringSlot* s = b->allocString(len);
      if (s) return s;
    }

    if (!addNewBlock(sizeof(StringSlot) + round_size_up(len))) return 0;

    return _head->allocString(len);
  }

  virtual void append(StringSlot* slot, char c) {
    // TODO: this is slow as hell!!!!!!!!!!!!
    for (Block* b = _head; b; b = b->next) {
      if (b->owns(slot)) {
        b->append(slot, c);
        break;
      }
    }
  }

  // Resets the memoryPool.
  // USE WITH CAUTION: this invalidates all previously allocated data
  void clear() {
    Block* currentBlock = _head;
    while (currentBlock != NULL) {
      _nextBlockCapacity = currentBlock->capacity();
      Block* nextBlock = currentBlock->next;
      _allocator.deallocate(currentBlock);
      currentBlock = nextBlock;
    }
    _head = 0;
  }

  StringBuilder startString() {
    return StringBuilder(this);
  }

 private:
  bool addNewBlock(size_t minCapacity) {
    size_t capacity = _nextBlockCapacity;
    if (minCapacity > capacity) capacity = minCapacity;
    size_t bytes = sizeof(Block) + capacity;
    char* p = reinterpret_cast<char*>(_allocator.allocate(bytes));
    Block* block = new (p) Block(p, capacity, _head);
    if (block == NULL) return false;
    _nextBlockCapacity *= 2;
    _head = block;
    return true;
  }

  TAllocator _allocator;
  Block* _head;
  size_t _nextBlockCapacity;
  SlotCache<Slot> _cache;
};

// Implements a MemoryPool with dynamic memory allocation.
// You are strongly encouraged to consider using StaticMemoryPool which is much
// more suitable for embedded systems.
typedef DynamicMemoryPoolBase<DefaultAllocator> DynamicMemoryPool;
}  // namespace ARDUINOJSON_NAMESPACE

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
#endif
