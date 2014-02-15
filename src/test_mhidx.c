#include "hidx.h"
#include "encap.h"

#ifdef KLD_MODULE
#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#define assert(STMT) if(!STMT){ printf("assert failed: %s\n", #STMT); }
#else
#include <stdio.h>
#include <assert.h>
#endif

typedef struct record
{
    char const *str;
    size_t strsize;
    int  num;
} record_t;

static key_desc_t get_str(void const *val);
static int test_create(void);
static int test_destroy(void);
static int test_size(void);
static int test_insert_no_collision(void);
static int test_insert_collision(void);
static int test_remove(void);
static int test_remove_collision(void);
static int check(int return_code);
static int do_test(void);

static key_desc_t get_str(void const *val)
{
    record_t const *rec = val;
    return (key_desc_t) {
        .raw = rec->str,
            .size = rec->strsize
    };
}

static int test_create(void)
{
    mhidx_ref idx = create_mhidx(1024, 0);
    if(!is_valid_ref(idx)) 
        return 1;
    destroy_mhidx(&idx);
    return 0;
}

static int test_destroy(void)
{
    mhidx_ref idx = create_mhidx(1024, 0);
    assert(is_valid_ref(idx));
    destroy_mhidx(&idx);
    return !is_valid_ref(idx) ? 0 : 1;
}

static int test_size(void)
{
    int result = 0;
    mhidx_ref idx = create_mhidx(1024, 0);
    assert(is_valid_ref(idx));
    result = (1024 == call(idx, size)) ? 0 : 1;
    destroy_mhidx(&idx);
    return result;
}

static int test_insert_no_collision(void)
{
    int result = 0;
    mhidx_ref idx = create_mhidx(1024, &get_str);
    record_t r[3];

    r[0] = (record_t) {.str = "acer", .strsize = 4, .num = 1};
    r[1] = (record_t) {.str = "yang", .strsize = 4, .num = 2};
    r[2] = (record_t) {.str = "benq", .strsize = 4, .num = 3};

    for (int i=0; i < 3; i++)
        call_n(idx, insert, &r[i]);

    for (int i=0; i < 3 && result == 0; i++) {
        result = ( 1 == call_n(idx, count, get_str(&r[i]))) ? 0 : 1;
    }
    destroy_mhidx(&idx);
    return result;
}

static int test_insert_collision(void)
{
    int result = 0;
    mhidx_ref idx = create_mhidx(1024, &get_str);
    record_t r[3];

    r[0] = (record_t) {.str = "acer", .strsize = 4, .num = 1};
    r[1] = (record_t) {.str = "asus", .strsize = 4, .num = 2};
    r[2] = (record_t) {.str = "aaaa", .strsize = 4, .num = 3};

    for(int i =0; i <3; ++i)
        call_n(idx, insert, &r[i]);

    for (int i=0; i < 3 && result == 0; i++) {
        result = (1 == call_n(idx, count, get_str(&r[i]))) ? 0 : 1;
    }
    destroy_mhidx(&idx);
    return result;
}

static int test_remove(void)
{
    int result = 0;
    mhidx_ref idx = create_mhidx(1024, &get_str);
    record_t r[3];

    r[0] = (record_t) {.str = "acer", .strsize = 4, .num = 1};
    r[1] = (record_t) {.str = "yang", .strsize = 4, .num = 2};
    r[2] = (record_t) {.str = "benq", .strsize = 4, .num = 3};

    for (int i=0; i < 3; i++)
        call_n(idx, insert, &r[i]);

    for (int i=0; i < 3 && result == 0; i++) {
        call_n(idx, remove, get_str(&r[i]));
        result = (0 == call_n(idx, count, get_str(&r[i]))) ? 0 : 1;
    }

    destroy_mhidx(&idx);
    return result;
}

static int test_remove_collision(void)
{
    int result = 0;
    mhidx_ref idx = create_mhidx(1024, &get_str);
    record_t r[3];

    r[0] = (record_t) {.str = "acer", .strsize = 4, .num = 1};
    r[1] = (record_t) {.str = "asus", .strsize = 4, .num = 2};
    r[2] = (record_t) {.str = "aaaa", .strsize = 4, .num = 3};

    for (int i=0; i < 3; i++)
        call_n(idx, insert, &r[i]);

    for (int i=0; i < 3 ; i++) {
        assert(0 != call_n(idx, count, get_str(&r[i])));
    }
    call_n(idx, remove, get_str(&r[1]));
    assert( 1 == call_n(idx, count, get_str(&r[0])) );
    assert( 0 == call_n(idx, count, get_str(&r[1])) );
    assert( 1 == call_n(idx, count, get_str(&r[2])) );

    r[1].str = "aqua";
    result = call_n(idx, insert, &r[1]) ? 0 : 1;
    for (int i=0; i < 3 && result == 0; ++i) {
        result = (1 == call_n(idx, count, get_str(&r[i]))) ? 0 : 1;
    }
    destroy_mhidx(&idx);
    return result;
}

static int check(int return_code)
{
    if (return_code) 
        printf("fail\n");
    else
        printf("ok\n");
    return return_code;
}

static int do_test(void)
{
    int failed = 0;
    int total = 0;

#define TEST(CASE) \
    printf("%s ... ", #CASE); \
    failed += (check(test_##CASE())); \
    ++total;

    TEST(create);
    TEST(destroy);
    TEST(size);
    TEST(insert_no_collision);
    TEST(insert_collision);
    TEST(remove);
    TEST(remove_collision);

    printf("--------\n");
    printf("failure: %d passed: %d total: %d\n",
           failed, total - failed, total);

    return failed;
}

#ifdef KLD_MODULE
static int load(struct module *module, int event, void *arg)
{
    int ec = 0;
    switch(event) {
    case MOD_LOAD:
        ec = do_test();
    break;
    case MOD_UNLOAD:
    break;
    default:
        ec = EOPNOTSUPP;
    break;
    }
    return ec;
}

static moduledata_t module = {
    "hidx_test",
    load,
    NULL
};

DECLARE_MODULE(hidx_test, module, SI_SUB_DRIVERS, SI_ORDER_MIDDLE);
MODULE_DEPEND(hidx_test, hidx, 1, 1, 1);

#else
int main(void)
{
    return do_test();
}
#endif
