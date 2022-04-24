#include <assert.h>
#include <stdio.h>
#include "hidx/encap.h"
#include "hidx/hidx.h"

typedef struct record {
  char const* str;
  size_t strsize;
  int num;
} record_t;

key_desc_t get_str(void const* val) {
  record_t const* rec = val;
  return (key_desc_t){.raw = rec->str, .size = rec->strsize};
}

key_desc_t get_num(void const* val) {
  record_t const* rec = val;
  return (key_desc_t){.raw = &(rec->num), .size = sizeof(int)};
}

int test_create() {
  hidx_ref idx = create_hidx(1024, 0);
  if (!is_valid_ref(idx))
    return 1;
  destroy_hidx(&idx);
  return 0;
}

int test_destroy() {
  hidx_ref idx = create_hidx(1024, 0);
  assert(idx.inst_ != 0);
  destroy_hidx(&idx);
  return (idx.inst_ == 0) ? 0 : 1;
}

int test_size() {
  int result = 0;
  hidx_ref idx = create_hidx(1024, 0);
  assert(is_valid_ref(idx));
  result = (1024 == call(idx, size)) ? 0 : 1;
  destroy_hidx(&idx);
  return result;
}

int test_insert_no_collision() {
  int result = 0;
  hidx_ref idx = create_hidx(1024, &get_str);
  record_t r[3];

  r[0] = (record_t){.str = "acer", .strsize = 4, .num = 1};
  r[1] = (record_t){.str = "yang", .strsize = 4, .num = 2};
  r[2] = (record_t){.str = "benq", .strsize = 4, .num = 3};

  for (int i = 0; i < 3; i++)
    call(idx, insert, &r[i]);

  for (int i = 0; i < 3; i++) {
    if (1 != call(idx, count, get_str(&r[i])))
      result = 1;
  }
  destroy_hidx(&idx);
  return result;
}

int test_insert_collision() {
  int result = 0;
  hidx_ref idx = create_hidx(1024, &get_str);
  record_t r[3];

  r[0] = (record_t){.str = "acer", .strsize = 4, .num = 1};
  r[1] = (record_t){.str = "asus", .strsize = 4, .num = 2};
  r[2] = (record_t){.str = "aaaa", .strsize = 4, .num = 3};

  for (int i = 0; i < 3; ++i)
    call(idx, insert, &r[i]);

  for (int i = 0; i < 3 && result == 0; i++) {
    result = (1 == call(idx, count, get_str(&r[i]))) ? 0 : 1;
  }
  destroy_hidx(&idx);
  return result;
}

int test_remove() {
  int result = 0;
  hidx_ref idx = create_hidx(1024, &get_str);
  record_t r[3];

  r[0] = (record_t){.str = "acer", .strsize = 4, .num = 1};
  r[1] = (record_t){.str = "yang", .strsize = 4, .num = 2};
  r[2] = (record_t){.str = "benq", .strsize = 4, .num = 3};

  for (int i = 0; i < 3; i++)
    call(idx, insert, &r[i]);

  for (int i = 0; i < 3 && result == 0; i++) {
    call(idx, remove, get_str(&r[i]));
    result = (0 == call(idx, count, get_str(&r[i]))) ? 0 : 1;
  }

  destroy_hidx(&idx);
  return result;
}

int test_remove_collision() {
  int result = 0;
  hidx_ref idx = create_hidx(1024, &get_str);
  record_t r[3];

  r[0] = (record_t){.str = "acer", .strsize = 4, .num = 1};
  r[1] = (record_t){.str = "asus", .strsize = 4, .num = 2};
  r[2] = (record_t){.str = "aaaa", .strsize = 4, .num = 3};

  for (int i = 0; i < 3; i++)
    call(idx, insert, &r[i]);

  for (int i = 0; i < 3 && result == 0; i++) {
    result = (0 != call(idx, count, get_str(&r[i]))) ? 0 : 1;
  }
  if (!result) {
    call(idx, remove, get_str(&r[1]));
    result = (1 == call(idx, count, get_str(&r[2]))) ? 0 : 1;

    if (!result) {
      r[1].str = "aqua";
      result = call(idx, insert, &r[1]) ? 0 : 1;
      for (int i = 0; i < 3 && result == 0; ++i) {
        result = (1 == call(idx, count, get_str(&r[i]))) ? 0 : 1;
      }
    }
  }
  destroy_hidx(&idx);
  return result;
}

int check(int return_code) {
  if (return_code)
    printf("fail\n");
  else
    printf("ok\n");
  return return_code;
}

int main() {
  int failed = 0;
  int total = 0;

#define TEST(CASE)                  \
  printf("%s ... ", #CASE);         \
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
  printf("failure: %d passed: %d total: %d\n", failed, total - failed, total);

  return 0;
}
