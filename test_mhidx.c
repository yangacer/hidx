#include <stdio.h>
#include <assert.h>
#include "hidx.h"
#include "encap.h"

typedef struct record
{
    char const *str;
    size_t strsize;
    int  num;
} record_t;

key_desc_t get_str(void const *val)
{
    record_t const *rec = val;
    return (key_desc_t) {
        .raw = rec->str,
            .size = rec->strsize
    };
}

key_desc_t get_num(void const *val)
{
    record_t const *rec = val;
    return (key_desc_t) {
        .raw = &(rec->num),
            .size = sizeof(int)
    };
}

int test_create()
{
    mhidx_ref idx = create_mhidx(1024, 0);
    return (idx.inst_ == 0) ? 1 : 0; 
}

int test_destroy()
{
    mhidx_ref idx = create_mhidx(1024, 0);
    assert(idx.inst_ != 0);
    destroy_mhidx(&idx);
    return (idx.inst_ == 0) ? 0 : 1;
}

int check(int return_code)
{
    if (return_code) 
        printf("fail\n");
    else
        printf("ok\n");
    return return_code;
}

int main()
{
    int failed = 0;
    int total = 0;

#define TEST(CASE) \
    printf("%s ... ", #CASE); \
    failed += (check(test_##CASE())); \
    ++total;

    TEST(create);
    TEST(destroy);
    //TEST(size);
    //TEST(insert_no_collision);
    //TEST(insert_collision);
    //TEST(remove);
    //TEST(remove_collision);

    printf("--------\n");
    printf("failure: %d passed: %d total: %d\n",
           failed, total - failed, total);

    return 0;
}
