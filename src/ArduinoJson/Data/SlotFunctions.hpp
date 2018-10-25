// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License

#pragma once

#include "../Memory/MemoryPool.hpp"
#include "../Strings/StringTypes.hpp"
#include "JsonVariantData.hpp"

namespace ARDUINOJSON_NAMESPACE {

template <typename TKey>
inline bool slotSetKey(VariantSlot* slot, TKey key, MemoryPool* pool) {
  const char* dup = key.save(pool);
  if (!dup) return false;
  slot->key = dup;
  slot->value.keyIsStatic = false;
  return true;
}

inline bool slotSetKey(VariantSlot* slot, ZeroTerminatedRamStringConst key,
                       MemoryPool* pool) {
  slot->key = key.save(pool);
  slot->value.keyIsStatic = true;
  return true;
}

inline bool slotSetKey(VariantSlot* slot, StringInMemoryPool key,
                       MemoryPool* pool) {
  slot->key = key.save(pool);
  slot->value.keyIsStatic = false;
  return true;
}

inline const VariantSlot* slotAdvance(const VariantSlot* slot,
                                      size_t distance) {
  while (distance && slot) {
    slot = slot->next;
    distance--;
  }
  return slot;
}

inline VariantSlot* slotAdvance(VariantSlot* slot, size_t distance) {
  while (distance && slot) {
    slot = slot->next;
    distance--;
  }
  return slot;
}

inline size_t slotSize(const VariantSlot* slot) {
  size_t n = 0;
  while (slot) {
    n++;
    slot = slot->next;
  }
  return n;
}

inline void slotFree(VariantSlot* slot, MemoryPool* pool) {
  const JsonVariantData& v = slot->value;
  if (v.type == JSON_ARRAY || v.type == JSON_OBJECT) {
    for (VariantSlot* s = v.content.asObject.head; s; s = s->next) {
      slotFree(s, pool);
    }
  }

  pool->freeVariant(slot);
}
}  // namespace ARDUINOJSON_NAMESPACE
