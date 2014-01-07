#ifndef HASH_INDEX_BUCKET_H_
#define HASH_INDEX_BUCKET_H_
#include <stddef.h>
#include <stdbool.h>

struct bucket_impl;
typedef struct bucket_impl bucket_impl_t;


/**
 * bucket - bucket for mhidx
 */
typedef struct bucket_interface
{
    bucket_impl_t*    (*ctor)               ();
    void              (*dtor)               (bucket_impl_t *);
    bool              (*append)             (bucket_impl_t *, void const *val);
    void              (*remove)             (bucket_impl_t *, size_t offset);
    void              (*remove_keep_order)  (bucket_impl_t *, size_t offset);
    void const*       (*at)                 (bucket_impl_t *, size_t offset);
    size_t            (*size)               (bucket_impl_t const*);
} bucket_interface_t;

typedef struct bucket
{
    bucket_interface_t *fnptr_;
    struct bucket_impl *inst_;
} bucket_ref;

bucket_ref create_bucket();
void destroy_bucket(bucket_ref *ref);

#endif
