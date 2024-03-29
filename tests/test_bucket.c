
// Copyright 2022 YANG, YUN-TSE (yangacer).
// Distributed under the Boost Software License, Version 1.0.

#include <assert.h>
#include <stdio.h>
#include "hidx/bucket.h"
#include "hidx/encap.h"

int test_create() {
  bucket_ref bk = create_bucket();
  if (bk.inst_ == 0)
    return 1;
  destroy_bucket(&bk);
  return 0;
}

int test_destroy() {
  bucket_ref bk = create_bucket();
  assert(bk.inst_ != 0);
  destroy_bucket(&bk);
  return (bk.inst_ == 0) ? 0 : 1;
}

int test_size() {
  int result = 0;
  bucket_ref bk = create_bucket();
  assert(bk.inst_ != 0);
  result = (0 == call(bk, size)) ? 0 : 1;
  destroy_bucket(&bk);
  assert(bk.inst_ == 0);
  return result;
}

int test_append() {
  int result = 0;
  bucket_ref bk = create_bucket();
  assert(bk.inst_ != 0);
  int value = 123;
  call(bk, append, &value);
  result = (1 == call(bk, size)) ? 0 : 1;
  destroy_bucket(&bk);
  assert(bk.inst_ == 0);
  return result;
}

int test_append_cause_expand() {
  int result = 0;
  bucket_ref bk = create_bucket();
  assert(bk.inst_ != 0);
  int value[5] = {0, 2, 4, 6, 8};
  for (int i = 0; i < 5; ++i)
    call(bk, append, &value[i]);
  result = (5 == call(bk, size)) ? 0 : 1;
  destroy_bucket(&bk);
  assert(bk.inst_ == 0);
  return result;
}

int test_at() {
  int result = 0;
  bucket_ref bk = create_bucket();
  assert(bk.inst_ != 0);
  int value[5] = {0, 2, 4, 6, 8};
  for (int i = 0; i < 5; ++i)
    call(bk, append, &value[i]);
  assert(5 == call(bk, size));

  for (int i = 0; i < 5; ++i) {
    if (call(bk, at, i) != &(value[i]))
      result = 1;
  }

  destroy_bucket(&bk);
  assert(bk.inst_ == 0);
  return result;
}

key_desc_t get_int_key(void const* val) {
  return (key_desc_t) {
    .raw = val, .size = sizeof(int)
  };
}

int test_find() {
  int result = 0;
  bucket_ref bk = create_bucket();
  assert(bk.inst_ != 0);
  int value[5] = {0, 2, 4, 6, 8};
  for (int i = 0; i < 5; ++i)
    call(bk, append, &value[i]);
  assert(5 == call(bk, size));

  for (int i = 0; i < 5; ++i) {
    key_desc_t key = get_int_key(value +i);
    if (call(bk, find, key, &get_int_key) != &(value[i]))
      result = 1;
  }

  destroy_bucket(&bk);
  assert(bk.inst_ == 0);

  return result;
}

int test_traverse() {
  int result = 0;
  bucket_ref bk = create_bucket();
  assert(bk.inst_ != 0);
  int value[5] = {0, 2, 4, 6, 8};
  for (int i = 0; i < 5; ++i)
    call(bk, append, &value[i]);
  assert(5 == call(bk, size));

  for (int i = 0; i < call(bk, size); ++i) {
    int const* v = call(bk, at, i);
    if (*v != value[i])
      result = 1;
  }

  destroy_bucket(&bk);
  assert(bk.inst_ == 0);

  return result;
}

int test_remove() {
  int result = 0;
  bucket_ref bk = create_bucket();
  assert(bk.inst_ != 0);
  int value[5] = {0, 2, 4, 6, 8};
  for (int i = 0; i < 5; ++i)
    call(bk, append, &value[i]);
  assert(5 == call(bk, size));
  call(bk, remove, 2);
  result = (4 == call(bk, size)) ? 0 : 1;
  destroy_bucket(&bk);
  assert(bk.inst_ == 0);
  return result;
}

int test_remove_in_traverse() {
  int result = 0;
  bucket_ref bk = create_bucket();
  assert(bk.inst_ != 0);
  int value[7] = {9, 1, 2, 4, 6, 8, 7};
  for (int i = 0; i < 7; ++i)
    call(bk, append, &value[i]);
  assert(7 == call(bk, size));
  for (size_t i = 0; i < call(bk, size); ++i) {
    int const* v = call(bk, at, i);
    if (*v % 2) {
      call(bk, remove, i);
      i--;
    }
  }
  result = (4 == call(bk, size)) ? 0 : 1;
  destroy_bucket(&bk);
  assert(bk.inst_ == 0);
  return result;
}

int test_remove_all() {
  int result = 0;
  bucket_ref bk = create_bucket();
  assert(bk.inst_ != 0);
  int value[5] = {0, 2, 4, 6, 8};
  for (int i = 0; i < 5; ++i)
    call(bk, append, &value[i]);
  assert(5 == call(bk, size));

  while (call(bk, size))
    call(bk, remove, 0);
  result = (0 == call(bk, size)) ? 0 : 1;
  destroy_bucket(&bk);
  assert(bk.inst_ == 0);
  return result;
}

int test_remove_keep_order() {
  int result = 0;
  bucket_ref bk = create_bucket();
  assert(bk.inst_ != 0);
  int value[5] = {0, 2, 4, 6, 8};
  for (int i = 0; i < 5; ++i)
    call(bk, append, &value[i]);
  assert(5 == call(bk, size));
  call(bk, remove_keep_order, 2);
  assert(4 == call(bk, size));
  result = (6 == *(int const*)call(bk, at, 2)) ? 0 : 1;
  destroy_bucket(&bk);
  assert(bk.inst_ == 0);
  return result;
}

int test_remove_keep_order_all() {
  int result = 0;
  bucket_ref bk = create_bucket();
  assert(bk.inst_ != 0);
  int value[5] = {0, 2, 4, 6, 8};
  for (int i = 0; i < 5; ++i)
    call(bk, append, &value[i]);
  assert(5 == call(bk, size));

  while (call(bk, size))
    call(bk, remove_keep_order, 0);
  result = (0 == call(bk, size)) ? 0 : 1;
  destroy_bucket(&bk);
  assert(bk.inst_ == 0);
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
  TEST(append);
  TEST(append_cause_expand);
  TEST(at);
  TEST(find);
  TEST(traverse);
  TEST(remove);
  TEST(remove_in_traverse);
  TEST(remove_all);
  TEST(remove_keep_order);
  TEST(remove_keep_order_all);

  printf("--------\n");
  printf("failure: %d passed: %d total: %d\n", failed, total - failed, total);
  return 0;
}
