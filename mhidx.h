#ifndef MULTI_HASH_INDEX_H_
#define MULTI_HASH_INDEX_H_

#include <stdbool.h>
#include <stddef.h>
#include "bucket.h"
#include "hash.h"

struct mhidx_impl;
typedef struct mhidx_impl mhidx_impl_t;

/**
 * mhidx - multi-hash index
 */
typedef struct mhidx_interface
{
    mhidx_impl_t* (*ctor)   (size_t entry_num, hkey_extractor_cb extractor);
    void          (*dtor)   (mhidx_impl_t*);
    bool          (*insert) (mhidx_impl_t*, void const *val);
    void          (*remove) (mhidx_impl_t*, key_desc_t key);
    size_t        (*count)  (mhidx_impl_t*, key_desc_t key);
    bucket_t      (*find)   (mhidx_impl_t*, key_desc_t key);
} mhidx_interface_t;

typedef struct mhidx
{
    mhidx_interface_t *fnptr_;
    mstruct hidx_impl *inst_;
} mhidx_ref;

#endif
