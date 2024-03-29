
// Copyright 2022 YANG, YUN-TSE (yangacer).
// Distributed under the Boost Software License, Version 1.0.

#ifndef HIDX_COMPATIBILITY_H_
#define HIDX_COMPATIBILITY_H_

#ifdef KLD_MODULE  // BEGIN kernel

#include <sys/libkern.h>
#include <sys/malloc.h>
#include <sys/types.h>
#define assert(...) \
  {}

MALLOC_DECLARE(HIDX_BUCKET_INSTANCE);
MALLOC_DECLARE(HIDX_BUCKET_STORAGE);
MALLOC_DECLARE(HIDX_BUCKET_REF);
MALLOC_DECLARE(HIDX_INSTANCE);
MALLOC_DECLARE(HIDX_ENTRIES);
MALLOC_DECLARE(HIDX_M_INSTANCE);
MALLOC_DECLARE(HIDX_M_ENTRIES);

#define HIDX_MALLOC_(SIZE, M_TYPE) malloc(SIZE, M_TYPE, M_ZERO | M_NOWAIT)
#define HIDX_REALLOC_(ADDR, SIZE, M_TYPE) realloc(ADDR, SIZE, M_TYPE, M_NOWAIT);
#define HIDX_CALLOC_(NUM, TYPE_SIZE, M_TYPE) \
  malloc(NUM* TYPE_SIZE, M_TYPE, M_ZERO | M_NOWAIT)
#define HIDX_FREE_(ADDR, M_TYPE) free(ADDR, M_TYPE);

#else  // END
       // BEGIN non kernel

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define HIDX_MALLOC_(SIZE, M_TYPE) malloc(SIZE)
#define HIDX_REALLOC_(ADDR, SIZE, M_TYPE) realloc(ADDR, SIZE);
#define HIDX_CALLOC_(NUM, TYPE_SIZE, M_TYPE) calloc(NUM, TYPE_SIZE)
#define HIDX_FREE_(ADDR, M_TYPE) free(ADDR)

#endif  // END non kerenl

#endif  // END header guard
