#ifndef HASH_INDEX_BUCKET_H_
#define HASH_INDEX_BUCKET_H_

struct bucket_impl;
struct bucket_iter_impl;
typedef struct bucket_impl bucket_impl_t;
typedef struct bucket_iter_impl bucket_iter_impl_t;

/**
 * bucket_iter - bucket iterator
 */
typedef struct bucket_iter_interface
{
  void          (*next)   (bucket_iter_impl_t * iter);
  void const *  (*get)    (bucket_iter_impl_t * iter);
} bucket_iter_interface_t;

typedef struct bucket_iter
{
  bucket_iter_interface_t *fnptr_;
  struct bucket_iter_impl *inst_;
} bucket_iter_t;

/**
 * bucket - bucket for mhidx
 */
typedef struct bucket_interface
{
  bucket_iter_t (*begin)  (bucket_impl_t *);
  bool          (*is_end) (bucket_impl_t *, bucket_iter_t iter);
  void          (*remove) (bucket_impl_t *, bucket_iter_t iter);
} bucket_interface_t;

typedef struct bucket
{
  bucket_interface_t *fnptr_;
  struct bucket_impl *inst_;
} bucket_t;

#endif
