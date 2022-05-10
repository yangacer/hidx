
// Copyright 2022 YANG, YUN-TSE (yangacer).
// Distributed under the Boost Software License, Version 1.0.

#pragma once
#include "hidx/compat.h"
typedef struct bucket_impl {
  void const** storage;
  uint8_t fib_idx;
  uint32_t size;
} bucket_impl_t;


bucket_impl_t* bucket_ctor();
void bucket_dtor(bucket_impl_t* inst);
bucket_impl_t* bucket_init(bucket_impl_t* inst);
void bucket_deinit(bucket_impl_t* inst);
bool bucket_append(bucket_impl_t* inst, void const* val);
void bucket_remove(bucket_impl_t* inst, size_t offset);
void bucket_remove_keep_order(bucket_impl_t* inst, size_t offset);
void const* bucket_at(bucket_impl_t* inst, size_t offset);
void const* bucket_find(bucket_impl_t* inst,
                        key_desc_t key,
                        hkey_extractor_cb extrator);
bool bucket_find_index(bucket_impl_t*,
                       size_t* index,
                       key_desc_t key,
                       hkey_extractor_cb extractor);
size_t bucket_size(bucket_impl_t const* inst);

