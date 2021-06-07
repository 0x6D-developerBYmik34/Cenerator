#include <stdio.h>
#include "cenerator.h"

int await_foo(coro_t *coro) {
  static int counter = 2;
  BEGIN_CORO();

  while (counter < 4) {
    YIELD(counter);
    ++counter;
  }

  return 11 + counter;
}

int foo_two(coro_t *coro)
{
  static int c = 0;
  BEGIN_CORO();

  printf(" hi ");
  YIELD(*(int*)coro->params);
  printf(" Name is Mike ");
  YIELD(3);
  printf(" its may posled ");
  c = AWAIT(await_foo);
  // c += AWAIT(await_foo); second AWAIT doesnt not normal work
  printf(" its was awaiting ");
  printf("%i", c);
  YIELD(7);
  printf(" the end? ");

  return 11;
}

int main ()
{
  printf("Hi");

  int param = 2;

  coro_t my_coro = {0};
  coro_inintial(&my_coro, foo_two);
  my_coro.params = (void *)&param;

  while(!coro_is_done(&my_coro)) {
    printf(" %i ", coro_resume(&my_coro));
  }

  // printf(" %i ", coro_resume(&my_coro));
  // printf(" %i ", coro_resume(&my_coro));
  // printf(" %i ", coro_resume(&my_coro));
  // printf(" %i ", coro_resume(&my_coro));
  // printf(" %i ", coro_resume(&my_coro));
  // printf(" %i ", coro_resume(&my_coro));
  // printf(" %i ", coro_resume(&my_coro));
  // printf(" %i ", coro_resume(&my_coro));
  // printf(" %i ", coro_resume(&my_coro));

  return 0;
}