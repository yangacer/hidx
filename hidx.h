#ifndef HASH_INDEX_H_
#define HASH_INDEX_H_

#include <stdbool.h>
#include <stddef.h>
#include "bucket.h"

// Forwarded decls
struct hidx_impl;
struct mhidx_impl;
typedef struct hidx_impl hidx_impl_t;

/**
 * Key descriptor
 */
typedef struct key_desc
{
  void const *raw;
  size_t size;
} key_desc_t;

/**
 * Key extractor callback interface.
 * @codebeg
 * struct my_rec {
 *   char const *key;
 *   size_t size;
 *   int other_value;
 * };
 *
 * key_desc_t my_extractor(void const *val)
 * {
 *   my_rec const *r = val;
 *   return (key_desc_t){ .raw = r->key, .size = r->size };
 * }
 * @endcode
 */
typedef key_desc_t (*hkey_extractor_cb)(void const* val);

/**
 * hidx - hash index 
 */
typedef struct hidx_interface
{
  hidx_impl_t*  (*ctor)   (size_t entry_num, hkey_extractor_cb extractor);
  void          (*dtor)   (hidx_impl_t*);
  bool          (*insert) (hidx_impl_t*, void const *val);
  void          (*remove) (hidx_impl_t*, key_desc_t key);
  size_t        (*count)  (hidx_impl_t const*, key_desc_t key);
  void const *  (*find)   (hidx_impl_t const*, key_desc_t key);
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
typedef struct mhidx_interface
{
  struct hidx_impl* (*ctor)   (size_t entry_num, hkey_extractor_cb extractor);
  void          (*dtor)   (struct hidx_impl*);
  bool          (*insert) (struct hidx_impl*, void const *val);
  void          (*remove) (struct hidx_impl*, key_desc_t key);
  size_t        (*count)  (struct hidx_impl*, key_desc_t key);
  bucket_t      (*find)   (struct hidx_impl*, key_desc_t key);
} mhidx_interface_t;

typedef struct mhidx
{
  mhidx_interface_t *fnptr_;
  mstruct hidx_impl *inst_;
} mhidx_ref;

 */
#endif
