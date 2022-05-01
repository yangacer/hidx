
// Copyright 2022 YANG, YUN-TSE (yangacer).
// Distributed under the Boost Software License, Version 1.0.

#ifndef HASH_INDEX_BUCKET_H_
#define HASH_INDEX_BUCKET_H_

#ifdef KLD_MODULE
#include <sys/types.h>
#else
#include <stdbool.h>
#include <stddef.h>
#endif

/**
 * bucket - bucket for mhidx
 */
typedef struct bucket_interface {
  void* (*ctor)();
  void (*dtor)(void*);
  bool (*append)(void*, void const* val);
  void (*remove)(void*, size_t offset);
  void (*remove_keep_order)(void*, size_t offset);
  void const* (*at)(void*, size_t offset);
  size_t (*size)(void const*);
} bucket_interface_t;

typedef struct bucket {
  bucket_interface_t* fnptr_;
  void* inst_;
} bucket_ref;

bucket_ref create_bucket(void);
void destroy_bucket(bucket_ref* ref);

#endif
