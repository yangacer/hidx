#include <assert.h>
#include <stdlib.h>

typedef void(*fnptr)(int val, char a);

struct incom;
typedef struct incom incom_t;

void f(int v, char a);

static fnptr stptr = &f;

void f(int v, char a){}

struct ini
{
  int a;
  char b;
};

int main()
{
  incom_t *ptr = 0;
  fnptr a = &f;
  void(*aa)(int val, char a) = &f;
  struct ini i;
  i = (struct ini){.a = 123, .b = 'c'};
 
  int *intp = malloc(sizeof(int)*4);
  return 0;
}
