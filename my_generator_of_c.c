/******************************************************************************

                            Online C Compiler.
                Code, Compile, Run and Debug C program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <stdio.h>
#include <setjmp.h>

#define YIELD(value) ({\
    if(setjmp (&(coro->coro_env)) != 0) {\
      memset(coro->coro_env, 0, sizeof(coro->coro_env));\
      coro->post_init_coro = 0;\
      printf (" continue ");\
    } else {\
     coro->yield_value = value;\
     coro->post_init_coro = 1;\
     printf (" jump ");\
     longjmp (&(coro->yields_env), 1);\
    }\
  })

#define INIT() if(coro->post_init_coro) longjmp(&(coro->coro_env), 1)

#define AWAIT(func) ({ \
  if(setjmp (&(coro->coro_env)) != 0) { \
      if(coro->awaiting_coro->is_complete) { \
        memset(coro->coro_env, 0, sizeof(coro->coro_env)); \
        coro->post_init_coro = 0; \
        coro_free(coro->awaiting_coro); \
        printf (" continue "); \
      } else { \
        coro->yield_value = coro_resume(coro->awaiting_coro); \
        coro->post_init_coro = 1; \
        printf (" jump "); \
        longjmp(&(coro->yields_env), 1); \
      } \
    } else { \
      coro->awaiting_coro = coro_new(func); \
      coro->yield_value = coro_resume(coro->awaiting_coro); \
      coro->post_init_coro = 1; \
      printf (" jump "); \
      longjmp(&(coro->yields_env), 1); \
    } \
    coro->yield_value; \
})

jmp_buf env;

typedef struct coro_t_ coro_t;
typedef int (*coro_function_t) (coro_t * coro);

struct coro_t_
{
  coro_function_t function;
  jmp_buf yields_env;
  jmp_buf coro_env;
  int yield_value;
  int is_complete;
  int post_init_coro;
  coro_t *awaiting_coro;
};

// coro_t* coro_new(coro_function_t function);
int coro_resume(coro_t * coro);
void coro_yield(coro_t * coro, int value);
void coro_init(coro_t * coro);

coro_t *coro_new(coro_function_t function)
{
  coro_t *coro = calloc (1, sizeof (*coro));

  coro->is_complete = 0;
  coro->post_init_coro = 0;
  coro->function = function;
  return coro;
}

void coro_inintial(coro_t * coro, coro_function_t function){
  coro->is_complete = 0;
  coro->post_init_coro = 0;
  coro->function = function;
}

void coro_free(coro_t * coro)
{
  free (coro);
}

int coro_resume(coro_t * coro)
{
  if (coro->is_complete) return -1;
  
  if (setjmp (&(coro->yields_env)) != 0) {
      printf (" na te ");
//    jmp_buf env;
//    coro->yields_env = env;
    //   coro->yields_env[1] = 0;
      memset(coro->yields_env, 0, sizeof(coro->yields_env));
      return coro->yield_value;
    } else {
      printf (" tut ");
      int return_value;
      return_value = (coro->function) (coro);
      coro->is_complete = 1;
      printf (" end ");
      return return_value;
    }
}

inline void coro_yield (coro_t * coro, int value)
{
  if(setjmp (&(coro->coro_env)) != 0) {
    memset(coro->coro_env, 0, sizeof(coro->coro_env));
    coro->post_init_coro = 0;
    printf (" continue ");
    return;
  }
  coro->yield_value = value;
  coro->post_init_coro = 1;
  printf (" jump ");
  longjmp (&(coro->yields_env), 1);
}

inline void coro_init (coro_t * coro)
{
  if (coro->post_init_coro) longjmp(&(coro->coro_env), 1);
}

int foo(coro_t * coro)
{
  coro_init(coro);
  printf(" hi ");
  coro_yield(coro, 1);
  printf(" Name is Mike ");
  coro_yield(coro, 3);
  printf(" its may posled ");
  coro_yield(coro, 7);
  printf(" the end? ");
  return 11;
}

int await_foo(coro_t *coro) {
  static int counter = 2;
  INIT();

  while (counter < 4) {
    YIELD(counter);
    ++counter;
  }

  return 11 + counter;
}

int foo_two(coro_t * coro)
{
  static int c = 0;
  INIT();
  printf(" hi ");
  YIELD(1);
  printf(" Name is Mike ");
  YIELD(3);
  printf(" its may posled ");
  c = AWAIT(await_foo);
  printf(" its was awaiting ");
  printf("d%", c);
  YIELD(7);
  printf(" the end? ");
  return 11;
}

int main ()
{
  printf("Hi");
  
  coro_t *my_coro = coro_new(foo_two);
  printf(" %i ", coro_resume(my_coro));
  printf(" %i ", coro_resume(my_coro));
  printf(" %i ", coro_resume(my_coro));
  printf(" %i ", coro_resume(my_coro));
  printf(" %i ", coro_resume(my_coro));
  coro_free(my_coro);

  return 0;
}
