
// Copyright 2022 YANG, YUN-TSE (yangacer).
// Distributed under the Boost Software License, Version 1.0.

#pragma once
#include "hidx/bucket.h"

typedef struct bucket_impl {
  key_val_t* storage;
  size_t size;
  uint8_t fib_idx;
} bucket_impl_t;

bucket_impl_t* bucket_ctor();
void bucket_dtor(bucket_impl_t* inst);
bucket_impl_t* bucket_init(bucket_impl_t* inst);
void bucket_deinit(bucket_impl_t* inst);
bool bucket_append(bucket_impl_t* inst, key_val_t val);
void bucket_remove(bucket_impl_t* inst, size_t offset);
void bucket_remove_keep_order(bucket_impl_t* inst, size_t offset);
void const* bucket_at(bucket_impl_t* inst, size_t offset);
void const* bucket_find(bucket_impl_t* inst, key_desc_t key);
bool bucket_find_index(bucket_impl_t*, size_t* index, key_desc_t key);
size_t bucket_size(bucket_impl_t const* inst);

