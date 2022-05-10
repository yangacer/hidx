
// Copyright 2022 YANG, YUN-TSE (yangacer).
// Distributed under the Boost Software License, Version 1.0.

#ifndef HASH_INDEX_H_
#define HASH_INDEX_H_

#include "hidx/bucket.h"
#include "hidx/hash.h"

/**
 * hidx - hash index
 */
typedef struct hidx_interface {
  void* (*ctor)(size_t entry_num, hkey_extractor_cb extractor);
  void (*dtor)(void*);
  bool (*insert)(void*, void const* val);
  void (*remove)(void*, key_desc_t key);
  size_t (*count)(void const*, key_desc_t key);
  size_t (*size)(void const*);
  void const* (*find)(void const*, key_desc_t key);
} hidx_interface_t;

typedef struct hidx {
  hidx_interface_t const* fnptr_;
  void* inst_;
} hidx_ref;

hidx_ref create_hidx(size_t entry_num, hkey_extractor_cb extractor);
void destroy_hidx(hidx_ref* ref);

/**
 * mhidx - multi-hash index
 */
typedef struct mhidx_interface {
  void* (*ctor)(size_t entry_num, hkey_extractor_cb extractor);
  void (*dtor)(void*);
  bool (*insert)(void*, void const* val);
  void (*remove)(void*, key_desc_t key);
  size_t (*count)(void const*, key_desc_t key);
  size_t (*size)(void const*);
  bucket_ref (*find)(void const*, key_desc_t key);
} mhidx_interface_t;

typedef struct mhidx {
  mhidx_interface_t const* fnptr_;
  void* inst_;
} mhidx_ref;

mhidx_ref create_mhidx(size_t entry_num, hkey_extractor_cb extractor);
void destroy_mhidx(mhidx_ref* ref);

#endif
