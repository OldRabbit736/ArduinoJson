// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License

#pragma once

#include "../Strings/StringInMemoryPool.hpp"
#include "MemoryPool.hpp"
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
  struct Block;
  struct EmptyBlock {
    Block* next;
    size_t capacity;
    size_t size;
  };
  struct Block : EmptyBlock {
    char data[1];
  };

 public:
  enum { EmptyBlockSize = sizeof(EmptyBlock) };

  DynamicMemoryPoolBase(size_t initialSize = ARDUINOJSON_DEFAULT_POOL_SIZE)
      : _head(NULL), _nextBlockCapacity(initialSize) {}

  ~DynamicMemoryPoolBase() {
    clear();
  }

  void reserve(size_t capacity) {
    _nextBlockCapacity = capacity;
  }

  virtual size_t size() const {
    return allocated_bytes() - _cache.size();
  }

  template <typename T>
  T* alloc() {
    return reinterpret_cast<T*>(canAllocInHead(sizeof(T))
                                    ? allocInHead(sizeof(T))
                                    : allocInNewBlock(sizeof(T)));
  }

  virtual Slot* allocVariant() {
    Slot* s = _cache.pop();
    return s ? s : alloc<Slot>();
  }

  virtual void freeVariant(Slot* slot) {
    _cache.push(slot);
  }

  // Allocates the specified amount of bytes in the memoryPool
  virtual StringSlot* allocString(size_t len) {
    alignNextAlloc();
    StringSlot* slot = alloc<StringSlot>();
    if (slot) {
      slot->value =
          canAllocInHead(len) ? allocInHead(len) : allocInNewBlock(len);
      slot->size = len;
      // slot->next = 0;
    }
    return slot;
  }

  virtual void append(StringSlot* slot, char c) {
    if (!slot || !slot->value) return;
    if (canAllocInHead(1)) {
      allocInHead(1);
    } else {
      char* newValue = allocInNewBlock(1);
      if (newValue) memcpy(newValue, slot->value, slot->size);
      slot->value = newValue;
    }
    slot->value[slot->size++] = c;
  }

  // Resets the memoryPool.
  // USE WITH CAUTION: this invalidates all previously allocated data
  void clear() {
    Block* currentBlock = _head;
    while (currentBlock != NULL) {
      _nextBlockCapacity = currentBlock->capacity;
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
  // Gets the number of bytes occupied in the memoryPool
  size_t allocated_bytes() const {
    size_t total = 0;
    for (const Block* b = _head; b; b = b->next)
      total += round_size_up(b->size);
    return total;
  }

  void alignNextAlloc() {
    if (_head) _head->size = this->round_size_up(_head->size);
  }

  bool canAllocInHead(size_t bytes) const {
    return _head != NULL && _head->size + bytes <= _head->capacity;
  }

  char* allocInHead(size_t bytes) {
    char* p = _head->data + _head->size;
    _head->size += bytes;
    return p;
  }

  char* allocInNewBlock(size_t bytes) {
    size_t capacity = _nextBlockCapacity;
    if (bytes > capacity) capacity = bytes;
    if (!addNewBlock(capacity)) return NULL;
    _nextBlockCapacity *= 2;
    return allocInHead(bytes);
  }

  bool addNewBlock(size_t capacity) {
    size_t bytes = EmptyBlockSize + capacity;
    Block* block = static_cast<Block*>(_allocator.allocate(bytes));
    if (block == NULL) return false;
    block->capacity = capacity;
    block->size = 0;
    block->next = _head;
    _head = block;
    return true;
  }

  TAllocator _allocator;
  Block* _head;
  size_t _nextBlockCapacity;
  SlotCache _cache;
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
