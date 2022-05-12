
// Copyright 2022 YANG, YUN-TSE (yangacer).
// Distributed under the Boost Software License, Version 1.0.

#include "bucket_impl.h"

#ifdef KLD_MODULE
#include <sys/kernel.h>
#include <sys/param.h>

MALLOC_DEFINE(HIDX_BUCKET_INSTANCE, "hidx_bucket", "hidx bucket impl pointer");
MALLOC_DEFINE(HIDX_BUCKET_STORAGE,
              "hidx_bucket_storage",
              "hidx bucket storage");
MALLOC_DEFINE(HIDX_BUCKET_REF, "hidx_bucket_ref", "hidx bucket reference");

#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
static bucket_interface_t bucket_fnptr_ = {
    .ctor = &bucket_ctor,
    .dtor = &bucket_dtor,
    .append = &bucket_append,
    .remove = &bucket_remove,
    .remove_keep_order = &bucket_remove_keep_order,
    .at = &bucket_at,
    .find = &bucket_find,
    .find_index = &bucket_find_index,
    .size = &bucket_size};
#pragma GCC diagnostic pop

/**
 * Implementations
 */

static uint32_t fib_table_[] = {
    16,        24,         48,         80,         128,       208,
    336,       544,        880,        1424,       2304,      3728,
    6032,      9760,       15792,      25552,      41344,     66896,
    108240,    175136,     283376,     458512,     741888,    1200400,
    1942288,   3142688,    5084976,    8227664,    13312640,  21540304,
    34852944,  56393248,   91246192,   147639440,  238885632, 386525072,
    625410704, 1011935776, 1637346480, 2649282256, 4286628736};

bucket_ref create_bucket() {
  return (bucket_ref){.fnptr_ = &bucket_fnptr_, .inst_ = bucket_fnptr_.ctor()};
}

void destroy_bucket(bucket_ref* ref) {
  ref->fnptr_->dtor(ref->inst_);
  ref->inst_ = 0;
}

bucket_impl_t* bucket_ctor() {
  bucket_impl_t* inst =
      HIDX_MALLOC_(sizeof(bucket_impl_t), HIDX_BUCKET_INSTANCE);
  if (0 == inst)
    return 0;
  if(bucket_init(inst))
    return inst;
  HIDX_FREE_(inst, HIDX_BUCKET_INSTANCE);
  return 0;
}

void bucket_dtor(bucket_impl_t* inst) {
  bucket_deinit(inst);
  HIDX_FREE_(inst, HIDX_BUCKET_INSTANCE);
}

bucket_impl_t* bucket_init(bucket_impl_t* inst) {
  (*inst) = (bucket_impl_t){
      .storage =
          HIDX_CALLOC_(fib_table_[0], sizeof(key_val_t), HIDX_BUCKET_STORAGE),
      .fib_idx = 0,
      .size = 0,
  };
  return (0 == inst->storage) ? 0 : inst;
}

void bucket_deinit(bucket_impl_t* inst) {
  HIDX_FREE_(inst->storage, HIDX_BUCKET_STORAGE);
}

bool bucket_append(bucket_impl_t* inst, key_val_t kv) {
  assert(inst->size <= fib_table_[inst->fib_idx]);

  if (inst->size == fib_table_[inst->fib_idx]) {
    // expand
    if (inst->fib_idx + 1 == sizeof(fib_table_) / sizeof(uint32_t))
      return false;
    key_val_t* orig = inst->storage;
    inst->storage = HIDX_REALLOC_(
        inst->storage, sizeof(key_val_t) * fib_table_[inst->fib_idx + 1],
        HIDX_BUCKET_STORAGE);
    if (0 == inst->storage) {
      inst->storage = orig;
      return false;
    }
    inst->fib_idx += 1;
  }
  inst->storage[inst->size] = kv;
  inst->size += 1;
  return true;
}

void bucket_remove(bucket_impl_t* inst, size_t offset) {
  if (offset >= inst->size)
    return;
  inst->size -= 1;
  inst->storage[offset] = inst->storage[inst->size];
  memset(inst->storage + inst->size, 0, sizeof(key_val_t));
  return;
}

void bucket_remove_keep_order(bucket_impl_t* inst, size_t offset) {
  if (offset >= inst->size)
    return;
  inst->size -= 1;
  for (size_t i = offset; i < inst->size; ++i) {
    inst->storage[i] = inst->storage[i + 1];
  }
  memset(inst->storage + inst->size, 0, sizeof(key_val_t));
}

void const* bucket_at(bucket_impl_t* inst, size_t offset) {
  if (offset >= inst->size)
    return 0;
  return inst->storage[offset].val;
}

void const* bucket_find(bucket_impl_t* inst, key_desc_t key) {
  if (!inst->size)
    return 0;
  size_t index;
  if (bucket_find_index(inst, &index, key))
    return inst->storage[index].val;
  return 0;
}

bool bucket_find_index(bucket_impl_t* inst, size_t* index, key_desc_t key) {
  if (!inst->size)
    return false;
  size_t i = 0;
  do {
    const key_val_t* kv = inst->storage + i;
    if (kv->key_size == key.size && 0 == memcmp(kv->key, key.raw, key.size)) {
      *index = i;
      return true;
    }
  } while (++i < inst->size);
  return false;
}

size_t bucket_size(bucket_impl_t const* inst) {
  return inst->size;
}
