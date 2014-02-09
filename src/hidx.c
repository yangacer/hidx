#include "hidx.h"
#include "hash.h"
#include "encap.h"
#include "compat.h"

#ifdef KLD_MODULE
#include <sys/param.h>
#include <sys/kernel.h>

MALLOC_DEFINE(HIDX_INSTANCE, "hidx_instance", "hidx instance");
MALLOC_DEFINE(HIDX_ENTRIES, "hidx_entries", "hidx entries");

#endif

struct hidx_impl 
{
    size_t size;
    bucket_ref *entry;
    hkey_extractor_cb extractor;
};

/**
 * Prototypes
 * */
static hidx_impl_t *hidx_ctor(size_t entry_num, hkey_extractor_cb extractor);
static void         hidx_dtor(hidx_impl_t* inst);
static bool         hidx_insert(hidx_impl_t* inst, void const *val);
static void         hidx_remove(hidx_impl_t* inst, key_desc_t key);
static size_t       hidx_count(hidx_impl_t const* inst, key_desc_t key);
static size_t       hidx_size(hidx_impl_t const* inst);
static void const * hidx_find(hidx_impl_t const* inst, key_desc_t key);

static hidx_interface_t hidx_fnptr_ = {
    .ctor = &hidx_ctor,
    .dtor = &hidx_dtor,
    .insert = &hidx_insert,
    .remove = &hidx_remove,
    .count = &hidx_count,
    .size = &hidx_size,
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

static hidx_impl_t *hidx_ctor(size_t entry_num, hkey_extractor_cb extractor)
{
    hidx_impl_t *inst = HIDX_MALLOC_(sizeof(hidx_impl_t), HIDX_INSTANCE);
    if (0 == inst) 
        return 0;

    (*inst) = (hidx_impl_t) {
        .size = entry_num,
        .entry = HIDX_CALLOC_(entry_num, sizeof(bucket_ref), HIDX_ENTRIES),
        .extractor = extractor
    };
    if (0 == inst->entry) {
        HIDX_FREE_(inst, HIDX_INSTANCE);
        return 0;
    }
    for (size_t i = 0; i < entry_num; ++i ) {
        inst->entry[i] = create_bucket();
        if(!is_valid_ref(inst->entry[i])) {
            for(size_t j=0; j < i; ++j) {
                destroy_bucket(&inst->entry[j]);
            }
            HIDX_FREE_(inst->entry, HIDX_ENTRIES);
            HIDX_FREE_(inst, HIDX_INSTANCE);
            return 0;
        }
        assert(0 == call(inst->entry[i], size));
    }

    assert(0 != inst && 0 != inst->entry &&
           "hidx allocation failed");
    return inst;
}

static void hidx_dtor(hidx_impl_t* inst)
{
    for (size_t i = 0; i < inst->size; ++i) {
        destroy_bucket(inst->entry + i);
    }
    HIDX_FREE_(inst->entry, HIDX_ENTRIES);
    HIDX_FREE_(inst, HIDX_INSTANCE);
}

static bool hidx_insert(hidx_impl_t* inst, void const *val)
{
    key_desc_t key = inst->extractor(val);
    size_t offset = hash(key, inst->size);
    bucket_ref collisions = inst->entry[offset];
    // search for dup in collisions
    for (size_t i = 0; i < call(collisions, size); ++i) {
        void const *curval = call(collisions, at, i);
        key_desc_t curkey = inst->extractor(curval);
        if ( curkey.size == key.size && 
             0 == memcmp(curkey.raw, key.raw, curkey.size ))
        {
            // duplicated key
            return false;
        } 
    }
    return call(collisions, append, val);
}

static void hidx_remove(hidx_impl_t* inst, key_desc_t key)
{
    size_t offset = hash(key, inst->size);
    bucket_ref collisions = inst->entry[offset];
    // search for dup in collisions
    for (size_t i = 0; i < call(collisions, size); ++i) {
        void const *curval = call(collisions, at, i);
        key_desc_t curkey = inst->extractor(curval);
        if ( curkey.size == key.size && 
             0 == memcmp(curkey.raw, key.raw, curkey.size ))
        {
            call(collisions, remove, i);
            return;
        } 
    }
}

static size_t hidx_count(hidx_impl_t const* inst, key_desc_t key)
{
    return hidx_find(inst, key) == 0 ? 0 : 1;
}

static size_t hidx_size(hidx_impl_t const* inst)
{
    return inst->size;
}

static void const *hidx_find(hidx_impl_t const* inst, key_desc_t key)
{
    size_t offset = hash(key, inst->size);
    bucket_ref collisions = inst->entry[offset];
    // search for dup in collisions
    for (size_t i = 0; i < call(collisions, size); ++i) {
        void const *curval = call(collisions, at, i);
        key_desc_t curkey = inst->extractor(curval);
        if ( curkey.size == key.size && 
             0 == memcmp(curkey.raw, key.raw, curkey.size ))
        {
            return curval;
        } 
    }
    return 0;
}

