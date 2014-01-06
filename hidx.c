#include "hidx.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef CANONICAL_HASH
#warning "CANONICLA_HASH is used for testing only. Do not use it in production."
#endif

typedef struct hidx_entry_header
{
  void const **collisions;
  size_t fib_idx;
} hidx_entry_header_t;

struct hidx_impl 
{
  size_t size;
  hidx_entry_header_t *entry;
  hkey_extractor_cb extractor;
};

/**
 * Prototypes
 * */
static hidx_impl_t *hidx_ctor(size_t entry_num, hkey_extractor_cb extractor);
static void hidx_dtor(hidx_impl_t* inst);
static bool hidx_insert(hidx_impl_t* inst, void const *val);
static void hidx_remove(hidx_impl_t* inst, key_desc_t key);
static size_t hidx_count(hidx_impl_t const* inst, key_desc_t key);
static void const *hidx_find(hidx_impl_t const* inst, key_desc_t key);

static hidx_interface_t hidx_fnptr_ = {
  .ctor = &hidx_ctor,
  .dtor = &hidx_dtor,
  .insert = &hidx_insert,
  .remove = &hidx_remove,
  .count = &hidx_count,
  .find = &hidx_find
};

/**
 * Implementations
 */

hidx_ref create_hidx(size_t entry_num, hkey_extractor_cb extractor)
{
  return (hidx_ref) {
      .fnptr_ = &hidx_fnptr_,
      .inst_ = hidx_fnptr_.ctor(entry_num, extractor)
    };  
}

void destroy_hidx(hidx_ref *ref)
{
  ref->fnptr_->dtor(ref->inst_);
  ref->inst_ = 0;
}

#define ZEROOUT_(ADDR, SIZE) \
  do {\
    for(size_t i=0; i<SIZE; ++i) \
      (ADDR)[i] = (typeof(ADDR[i])) {0}; \
  } while(0);

static size_t hash(key_desc_t key, size_t max)
{
#ifdef CANONICAL_HASH
  return ((unsigned char*)key.raw)[0];
#else
  size_t hval = 5381;
  for(size_t i = 0; i < key.size; ++i) {
      hval = (hval << 5) + hval + ((unsigned char *)key.raw)[i];
  }
  return hval % max;
#endif
}

static size_t fib_table[10] = { 2, 3, 5, 8, 13, 21, 44, 65, 109, 174 };

#if !defined(NDEBUG) && !defined(KERNEL)
#define ASSERT_COND_NULLPTR_CANNOT_PRECEED_NON_NULL_ONES \
    for (size_t i = 0; i < fib_table[header->fib_idx]; ++i) { \
      if ( header->collisions[i] == 0 ) { \
        for (size_t j = i + 1; j < fib_table[header->fib_idx]; ++j) { \
          assert(header->collisions[j] == 0 && \
            "Nullptr cannot preceed non null ones"); \
        } \
      } \
    }
#else
#define ASSERT_COND_NULLPTR_CANNOT_PRECEED_NON_NULL_ONES {}
#endif

static hidx_impl_t *hidx_ctor(size_t entry_num, hkey_extractor_cb extractor)
{
  hidx_impl_t *inst = malloc(sizeof(hidx_impl_t));
  if (0 == inst) 
    return 0;

  (*inst) = (hidx_impl_t) {
    .size = entry_num,
      .entry = malloc(entry_num * sizeof(hidx_entry_header_t)),
      .extractor = extractor
  };
  if (0 == inst->entry) 
    return 0;

  ZEROOUT_(inst->entry, inst->size);
  return inst;
}

static void hidx_dtor(hidx_impl_t* inst)
{
  for (size_t i = 0; i < inst->size; ++i) {
      if(inst->entry[i].collisions)
        free(inst->entry[i].collisions);
  }
  free(inst->entry);
  free(inst);
}

static bool hidx_insert(hidx_impl_t* inst, void const *val)
{
  bool result = false;
  key_desc_t newkey = inst->extractor(val);
  size_t offset = hash(newkey, inst->size);
  hidx_entry_header_t *header = inst->entry + offset;
  // initial array for storing collisions
  if (0 == header->collisions) {
    header->fib_idx = 0;
    header->collisions = malloc(sizeof(void const*) * fib_table[0]);
    if (0 == header->collisions)
      return false;
    ZEROOUT_(header->collisions, sizeof(void const*) * fib_table[0]);
  }

  ASSERT_COND_NULLPTR_CANNOT_PRECEED_NON_NULL_ONES;

  // search for collisions and empty slot
  for (size_t i = 0; i < fib_table[header->fib_idx]; ++i) {
    void const **slot = &(header->collisions[i]);
    if (0 == *slot) {
      *slot = val;
      result = true;
      break;
    } else {
      key_desc_t curkey = inst->extractor(*slot);
      if ( curkey.size == newkey.size && 
           0 == memcmp(curkey.raw, newkey.raw, curkey.size ))
      {
        // duplicated key
        return false;
      } 
    }
  }
  // no duplicated keys but collisions storage is too small
  if (false == result) {
    // collisions storage is too large, consider using bigger hidx
    if ( header->fib_idx >= 9 )
      return false;
    // reallocate and copy collisions
    void const **orig = header->collisions;
    size_t cursize = fib_table[header->fib_idx];

    header->collisions = malloc(
      sizeof(void const*) * fib_table[header->fib_idx + 1]);
    if ( header->collisions ) {
      memcpy(header->collisions, orig, sizeof(void const*) * cursize);
      free(orig);
      header->collisions[cursize] = val;
      header->fib_idx += 1;
      result = true;
    } else {
      header->collisions = orig;  
    }
  }
  return result;
}

static void hidx_remove(hidx_impl_t* inst, key_desc_t key)
{
  size_t offset = hash(key, inst->size);
  hidx_entry_header_t *header = inst->entry + offset;
  if (0 == header->collisions) 
    return;
  for (size_t i = 0; i < fib_table[header->fib_idx]; ++i) {
    void const **slot = &(header->collisions[i]);
    if (0 == *slot) {
      return;  
    } else {
      key_desc_t curkey = inst->extractor(*slot);
      if ( curkey.size == key.size && 
           0 == memcmp(curkey.raw, key.raw, curkey.size ))
      { // found target
        *slot = 0;
        // swap removed one with the last non-null one
        size_t cursize = fib_table[header->fib_idx];
        for (size_t j = cursize; j-- > i + 1;) {
          if (0 != header->collisions[j]) {
            *slot = header->collisions[j];
            header->collisions[j] = 0;
            break;
          }
        } 
      }
    }
  }
  ASSERT_COND_NULLPTR_CANNOT_PRECEED_NON_NULL_ONES;
}

static size_t hidx_count(hidx_impl_t const* inst, key_desc_t key)
{
  return hidx_find(inst, key) == 0 ? 0 : 1;
}

static void const *hidx_find(hidx_impl_t const* inst, key_desc_t key)
{
  size_t offset = hash(key, inst->size);
  hidx_entry_header_t *header = inst->entry + offset;
  if (0 == header->collisions) 
    return 0;
  for (size_t i = 0; i < fib_table[header->fib_idx]; ++i) {
    void const **slot = &(header->collisions[i]);
    if (0 == *slot) {
      return 0;  
    } else {
      key_desc_t curkey = inst->extractor(*slot);
      if ( curkey.size == key.size && 
           0 == memcmp(curkey.raw, key.raw, curkey.size ))
      { // found target
        return *slot;
      }
    }
  }
  return 0;
}

