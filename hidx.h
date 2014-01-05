#ifndef HASH_INDEX_H_
#define HASH_INDEX_H_

#include <stdbool.h>
#include <stddef.h>
#include "bucket.h"

// Forwarded decls
struct hidx_impl;
struct mhidx_impl;

typedef struct key_desc
{
  void const *raw;
  size_t size;
} key_desc_t;

typedef key_desc_t (*hkey_extractor_cb)(void const* val);

/**
 * hidx - hash index 
 */
typedef struct hidx_interface
{
  struct hidx_impl*  (*ctor)   (size_t entry_num, hkey_extractor_cb extractor);
  void          (*dtor)   (struct hidx_impl*);
  bool          (*insert) (struct hidx_impl*, void const *val);
  void          (*remove) (struct hidx_impl*, key_desc_t key);
  size_t        (*count)  (struct hidx_impl const*, key_desc_t key);
  void const *  (*find)   (struct hidx_impl const*, key_desc_t key);
} hidx_interface_t;

typedef struct hidx
{
  hidx_interface_t const *fnptr_;
  struct hidx_impl *inst_;
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
