#ifndef HASH_INDEX_H_
#define HASH_INDEX_H_

#include <stdbool.h>
#include <stddef.h>
#include "bucket.h"
#include "hash.h"

// Forwarded decls
struct hidx_impl;
struct mhidx_impl;
typedef struct hidx_impl hidx_impl_t;
typedef struct mhidx_impl mhidx_impl_t;


/**
 * hidx - hash index 
 */
typedef struct hidx_interface
{
    hidx_impl_t*  (*ctor)     (size_t entry_num, hkey_extractor_cb extractor);
    void          (*dtor)     (hidx_impl_t*);
    bool          (*insert)   (hidx_impl_t*, void const *val);
    void          (*remove)   (hidx_impl_t*, key_desc_t key);
    size_t        (*count)    (hidx_impl_t const*, key_desc_t key);
    size_t        (*size)     (hidx_impl_t const*);
    void const *  (*find)     (hidx_impl_t const*, key_desc_t key);
} hidx_interface_t;

typedef struct hidx
{
    hidx_interface_t const *fnptr_;
    hidx_impl_t *inst_;
} hidx_ref;

hidx_ref create_hidx(size_t entry_num, hkey_extractor_cb extractor);
void destroy_hidx(hidx_ref *ref);

/**
 * mhidx - multi-hash index
 */
typedef struct mhidx_interface
{
    mhidx_impl_t* (*ctor)   (size_t entry_num, hkey_extractor_cb extractor);
    void          (*dtor)   (mhidx_impl_t*);
    bool          (*insert) (mhidx_impl_t*, void const *val);
    void          (*remove) (mhidx_impl_t*, key_desc_t key);
    size_t        (*count)  (mhidx_impl_t const*, key_desc_t key);
    size_t        (*size)   (mhidx_impl_t const*);
    bucket_ref    (*find)   (mhidx_impl_t const*, key_desc_t key);
} mhidx_interface_t;

typedef struct mhidx
{
    mhidx_interface_t const *fnptr_;
    mhidx_impl_t *inst_;
} mhidx_ref;

mhidx_ref create_mhidx(size_t entry_num, hkey_extractor_cb extractor);
void destroy_mhidx(mhidx_ref *ref);

#endif
