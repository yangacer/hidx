
// Copyright 2022 YANG, YUN-TSE (yangacer).
// Distributed under the Boost Software License, Version 1.0.

#include "hidx/bucket.h"
#include "hidx/compat.h"

#ifdef KLD_MODULE
#include <sys/kernel.h>
#include <sys/param.h>

MALLOC_DEFINE(HIDX_BUCKET_INSTANCE, "hidx_bucket", "hidx bucket impl pointer");
MALLOC_DEFINE(HIDX_BUCKET_STORAGE,
              "hidx_bucket_storage",
              "hidx bucket storage");
MALLOC_DEFINE(HIDX_BUCKET_REF, "hidx_bucket_ref", "hidx bucket reference");

#endif

struct bucket_impl {
  void const** storage;
  uint8_t fib_idx;
  uint32_t size;
};

/**
 * Prototypes
 **/

static bucket_impl_t* bucket_ctor();
static void bucket_dtor(bucket_impl_t* inst);
static bool bucket_append(bucket_impl_t* inst, void const* val);
static void bucket_remove(bucket_impl_t* inst, size_t offset);
static void bucket_remove_keep_order(bucket_impl_t* inst, size_t offset);
static void const* bucket_at(bucket_impl_t* inst, size_t offset);
static size_t bucket_size(bucket_impl_t const* inst);

static bucket_interface_t bucket_fnptr_ = {
    .ctor = &bucket_ctor,
    .dtor = &bucket_dtor,
    .append = &bucket_append,
    .remove = &bucket_remove,
    .remove_keep_order = &bucket_remove_keep_order,
    .at = &bucket_at,
    .size = &bucket_size};

/**
 * Implementations
 */

#define FIBTABLESIZE 44
static uint32_t fib_table_[FIBTABLESIZE] = {
    2,          3,         5,         8,         13,        21,
    34,         55,        89,        144,       233,       377,
    610,        987,       1597,      2584,      4181,      6765,
    10946,      17711,     28657,     46368,     75025,     121393,
    196418,     317811,    514229,    832040,    1346269,   2178309,
    3524578,    5702887,   9227465,   14930352,  24157817,  39088169,
    63245986,   102334155, 165580141, 267914296, 433494437, 701408733,
    1134903170, 1836311903};

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
  (*inst) = (bucket_impl_t){
      .storage =
          HIDX_CALLOC_(fib_table_[0], sizeof(void const*), HIDX_BUCKET_STORAGE),
      .fib_idx = 0,
      .size = 0,
  };
  if (0 == inst->storage) {
    HIDX_FREE_(inst, HIDX_BUCKET_INSTANCE);
    return 0;
  }
  return inst;
}

void bucket_dtor(bucket_impl_t* inst) {
  HIDX_FREE_(inst->storage, HIDX_BUCKET_STORAGE);
  HIDX_FREE_(inst, HIDX_BUCKET_INSTANCE);
}

bool bucket_append(bucket_impl_t* inst, void const* val) {
  assert(inst->size <= fib_table_[inst->fib_idx]);

  if (inst->size == fib_table_[inst->fib_idx]) {
    // expand
    if (inst->fib_idx + 1 == FIBTABLESIZE)
      return false;
    void const** orig = inst->storage;
    inst->storage = HIDX_REALLOC_(
        inst->storage, sizeof(void const*) * fib_table_[inst->fib_idx + 1],
        HIDX_BUCKET_STORAGE);
    if (0 == inst->storage) {
      inst->storage = orig;
      return false;
    }
    inst->fib_idx += 1;
  }
  inst->storage[inst->size] = val;
  inst->size += 1;
  return true;
}

void bucket_remove(bucket_impl_t* inst, size_t offset) {
  if (offset >= inst->size)
    return;
  inst->size -= 1;
  inst->storage[offset] = inst->storage[inst->size];
  inst->storage[inst->size] = 0;
  return;
}

void bucket_remove_keep_order(bucket_impl_t* inst, size_t offset) {
  if (offset >= inst->size)
    return;
  inst->size -= 1;
  for (size_t i = offset; i < inst->size; ++i) {
    inst->storage[i] = inst->storage[i + 1];
  }
  inst->storage[inst->size] = 0;
}

void const* bucket_at(bucket_impl_t* inst, size_t offset) {
  if (offset >= inst->size)
    return 0;
  return inst->storage[offset];
}

size_t bucket_size(bucket_impl_t const* inst) {
  return inst->size;
}
