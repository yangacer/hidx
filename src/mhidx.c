#include "hidx.h"
#include "hash.h"
#include "encap.h"
#include "compat.h"

#ifdef KLD_MODULE
#include <sys/param.h>
#include <sys/kernel.h>

MALLOC_DEFINE(HIDX_M_INSTANCE, "mhidx_instance", "multi-hidx instance");
MALLOC_DEFINE(HIDX_M_ENTRIES, "mhidx_entries", "multi-hidx entries");

#endif

struct mhidx_impl
{
    size_t size;
    bucket_ref *entry;
    hkey_extractor_cb extractor;
};

/**
 * Prototypes
 */

static mhidx_impl_t *mhidx_ctor(size_t entry_num, hkey_extractor_cb extractor);
static void         mhidx_dtor(mhidx_impl_t* inst);
static bool         mhidx_insert(mhidx_impl_t* inst, void const *val);
static void         mhidx_remove(mhidx_impl_t* inst, key_desc_t key);
static size_t       mhidx_count(mhidx_impl_t const* inst, key_desc_t key);
static size_t       mhidx_size(mhidx_impl_t const* inst);
static bucket_ref   mhidx_find(mhidx_impl_t const* inst, key_desc_t key);

static mhidx_interface_t mhidx_fnptr_ = {
    .ctor = &mhidx_ctor,
    .dtor = &mhidx_dtor,
    .insert = &mhidx_insert,
    .remove = &mhidx_remove,
    .count = &mhidx_count,
    .size = &mhidx_size,
    .find = &mhidx_find
};

/**
 * Implementations
 */

mhidx_ref create_mhidx(size_t entry_num, hkey_extractor_cb extractor)
{
    return (mhidx_ref) {
        .fnptr_ = &mhidx_fnptr_,
        .inst_ = mhidx_fnptr_.ctor(entry_num, extractor)
    };
}

void destroy_mhidx(mhidx_ref *ref)
{
    ref->fnptr_->dtor(ref->inst_);
    ref->inst_ = 0;
}

static mhidx_impl_t *mhidx_ctor(size_t entry_num, hkey_extractor_cb extractor)
{
    mhidx_impl_t *inst = HIDX_MALLOC_(sizeof(mhidx_impl_t), HIDX_M_INSTANCE);
    if (0 == inst) 
        return 0;

    (*inst) = (mhidx_impl_t) {
        .size = entry_num,
        .entry = HIDX_CALLOC_(entry_num, sizeof(bucket_ref), HIDX_M_ENTRIES),
        .extractor = extractor
    };
    if (0 == inst->entry) {
        HIDX_FREE_(inst, HIDX_M_INSTANCE);
        return 0;
    }
    for (size_t i = 0; i < entry_num; ++i ) {
        inst->entry[i] = create_bucket();
        if(!is_valid_ref(inst->entry[i])) {
            for(size_t j=0; j < i; ++j) {
                destroy_bucket(&inst->entry[j]);
            }
            HIDX_FREE_(inst->entry, HIDX_M_ENTRIES);
            HIDX_FREE_(inst, HIDX_M_INSTANCE);
            return 0;
        }
        assert(0 == call(inst->entry[i], size));
    }

    assert(0 != inst && 0 != inst->entry &&
           "mhidx allocation failed");

    return inst;
}

static void mhidx_dtor(mhidx_impl_t* inst)
{
    assert(0 != inst);
    for (size_t i = 0; i < inst->size; ++i) {
        bucket_ref collisions = inst->entry[i];
        assert(collisions.inst_ != 0);
        for ( size_t j = 0; j < call(collisions, size); ++j) {
            bucket_ref *bk = (bucket_ref*)call(collisions, at, j);
            destroy_bucket(bk);
            HIDX_FREE_(bk, HIDX_BUCKET_REF);
        }
        destroy_bucket(&collisions);
    }
    HIDX_FREE_(inst->entry, HIDX_M_ENTRIES);
    HIDX_FREE_(inst, HIDX_M_INSTANCE);
}

static bool mhidx_insert(mhidx_impl_t* inst, void const *val)
{
    bool result = false;
    key_desc_t key = inst->extractor(val);
    size_t offset = hash(key, inst->size);
    bucket_ref collisions = inst->entry[offset];
    for(size_t i = 0 ; i < call(collisions, size); ++i) {
        bucket_ref *bucket = (bucket_ref*)call(collisions, at, i);
        // XXX There is a chance an user has removed all values in a bucket,
        // we have to remove it.
        if ( 0 == call(*bucket, size) ) {
            call(collisions, remove, i--);
            destroy_bucket(bucket);
            HIDX_FREE_(bucket, HIDX_BUCKET_REF);
            continue;
        }
        key_desc_t curkey = inst->extractor(call(*bucket, at, 0));
        if (curkey.size == key.size &&
            0 == memcmp(curkey.raw, key.raw, curkey.size))
        {
            if(!call(*bucket, append, val))
                return false;
            result = true;
            break;
        }
    }
    if (!result) {
        bucket_ref *bucket = HIDX_MALLOC_(sizeof(bucket_ref), HIDX_BUCKET_REF);
        *bucket = create_bucket();
        if(is_valid_ref(*bucket) && 
           call(*bucket, append, val) &&
           call(collisions, append, bucket)) 
        {
            result = true;
        } else {
            destroy_bucket(bucket);
        }
    }
    return result;
}

static void mhidx_remove(mhidx_impl_t* inst, key_desc_t key)
{
    size_t offset = hash(key, inst->size);
    bucket_ref collisions = inst->entry[offset];
    for(size_t i = 0 ; i < call(collisions, size); ++i) {
        bucket_ref *bucket = (bucket_ref*)call(collisions, at, i);
        // XXX There is a chance user remove all values in a bucket,
        // we have to remove it.
        if ( 0 == call(*bucket, size) ) {
            call(collisions, remove, i--);
            destroy_bucket(bucket);
            HIDX_FREE_(bucket, HIDX_BUCKET_REF);
            continue;
        }
        key_desc_t curkey = inst->extractor(call(*bucket, at, 0));
        if (curkey.size == key.size &&
            0 == memcmp(curkey.raw, key.raw, curkey.size))
        {
            call(collisions, remove, i);
            destroy_bucket(bucket);
            HIDX_FREE_(bucket, HIDX_BUCKET_REF);
            return;
        }
    }
}

static size_t mhidx_count(mhidx_impl_t const* inst, key_desc_t key)
{
    bucket_ref target = mhidx_find(inst, key);
    if(!is_valid_ref(target))
        return 0;
    return call(target, size);
}

static size_t mhidx_size(mhidx_impl_t const* inst)
{
    return inst->size;
}

static bucket_ref mhidx_find(mhidx_impl_t const* inst, key_desc_t key)
{
    size_t offset = hash(key, inst->size);
    bucket_ref collisions = inst->entry[offset];
    for(size_t i = 0 ; i < call(collisions, size); ++i) {
        bucket_ref *bucket = ((bucket_ref*)call(collisions, at, i));
        // XXX There is a chance user remove all values in a bucket,
        // we have to remove it.
        if ( 0 == call(*bucket, size) ) {
            call(collisions, remove, i--);
            destroy_bucket(bucket);
            HIDX_FREE_(bucket, HIDX_BUCKET_REF);
            continue;
        }
        key_desc_t curkey = inst->extractor(call(*bucket, at, 0));
        if (curkey.size == key.size &&
            0 == memcmp(curkey.raw, key.raw, curkey.size))
        {
            return *bucket;
        }
    }
    return (bucket_ref){0};
}


