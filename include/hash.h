#ifndef HASH_FUNCTION_H_
#define HASH_FUNCTION_H_

#ifdef CANONICAL_HASH
#warning "CANONICLA_HASH is used for testing only. Do not use it in production."
#endif

/**
 * Key descriptor
 */
typedef struct key_desc {
  void const* raw;
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
 * Hash function
 */
static inline size_t hash(key_desc_t key, size_t max) {
#ifdef CANONICAL_HASH
  return ((unsigned char*)key.raw)[0];
#else
  size_t hval = 5381;
  for (size_t i = 0; i < key.size; ++i) {
    hval = (hval << 5) + hval + ((unsigned char*)key.raw)[i];
  }
  return hval % max;
#endif
}

#endif
