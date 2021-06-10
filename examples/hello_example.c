#include <stdio.h>
#include "cenerator.h"


int await_foo(coro_t *coro) {
  static int counter = 0;
  BEGIN_CORO();
  counter = 2;

  while (counter < 4) {
    YIELD(counter);
    ++counter;
  }

  return 11 + counter;
}

int await_yet(coro_t *coro) {
  static int c = 0;
  BEGIN_CORO();
  c = 0;

  while(c < 50) {
    printf("await_yet");
    c += 10;
    YIELD(c);
  }
  
  printf("await_yet end");
  return c + 10;
}

CORO_PARAM_INIT(foo_two, {
  int some_num;
})
int foo_two(coro_t *coro)
{
  static int c = 0;
  BEGIN_CORO();

  printf(" hi ");
  YIELD(GET_CORO_PARAM(foo_two, some_num));
  printf(" Name is Mike ");
  YIELD(3);
  printf(" its may posled ");

  c = AWAIT(await_foo);
  c += AWAIT(await_foo);

  printf(" its was awaiting ");
  printf("%i", c);
  printf(" love with await_yet: %i", AWAIT(await_yet));
  YIELD(7);
  printf(" the end? ");

  return 11;
}

int main ()
{
  printf("Hi");

  coro_t my_coro = {0};
  coro_inintial(&my_coro, foo_two);
  my_coro.params = ADD_CORO_PARAMS(foo_two, {
    .some_num = 67
  });

  while(!coro_is_done(&my_coro)) {
    printf(" %i ", coro_resume(&my_coro));
  }

  return 0;
}
