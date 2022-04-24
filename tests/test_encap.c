#include <assert.h>
#include <stdlib.h>

typedef void (*fnptr)(int val, char a);

struct incom;
typedef struct incom incom_t;

void f(int v, char a);

static fnptr stptr = &f;

void f(int v, char a) {}

struct ini {
  int a;
  char b;
};

#define FORWARD(func, arg1, ...) (func(arg1 __VA_OPT__(, ) __VA_ARGS__))

void handler1(int a) {}
void handler2(int a, int b) {}

int main(void) {
  incom_t* ptr = 0;
  fnptr a = &f;
  void (*aa)(int val, char a) = &f;
  struct ini i;
  i = (struct ini){.a = 123, .b = 'c'};

  int* intp = malloc(sizeof(int) * 4);
  int arr[3] = {1, 2, 3};

  assert(sizeof(arr) / sizeof(int) == 3);
  FORWARD(handler1, 123);
  FORWARD(handler2, 123, 456);
  return 0;
}
