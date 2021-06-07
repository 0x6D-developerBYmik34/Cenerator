#pragma once


#include <stdio.h>
#include <setjmp.h>


#define bool _Bool


#define BEGIN_CORO() ({ \
    static coro_t awaiting_coro = {0}; \
    coro->awaiting_coro = &awaiting_coro; \
    if(coro_is_suspend(coro)) longjmp(&(coro->coro_env), 1); \
    coro->status = RUNNING; \
})


#define YIELD(value) ({\
    if(setjmp (&(coro->coro_env)) != 0) {\
      memset(coro->coro_env, 0, sizeof(jmp_buf));\
      coro->status = RUNNING;\
      printf(" continue ");\
    } else {\
     coro->status = YIELDING;\
     coro->yield_value = value;\
     printf(" jump ");\
     longjmp(&(coro->yields_env), 1);\
    }\
})


#define AWAIT(func) ({ \
  if(setjmp (&(coro->coro_env)) != 0) { \
      if(coro_is_done(coro->awaiting_coro)) { \
        memset(coro->coro_env, 0, sizeof(jmp_buf)); \
        memset(coro->awaiting_coro, 0, sizeof(coro_t)); \
        printf(" continue "); \
        coro->status = RUNNING; \
      } else { \
        coro->status = YIELDING; \
        coro->yield_value = coro_resume(coro->awaiting_coro); \
        printf(" jump is AWAIT "); \
        longjmp(&(coro->yields_env), 1); \
      } \
    } else { \
      coro_inintial(coro->awaiting_coro, func); \
      coro->status = YIELDING; \
      coro->yield_value = coro_resume(coro->awaiting_coro); \
      printf (" jump is AWAIT "); \
      longjmp(&(coro->yields_env), 1); \
    } \
    coro->yield_value; \
})


typedef struct coro_t_ coro_t;
typedef int (*coro_function_t) (coro_t *coro);


typedef enum enum_coro_status {
    STARTING,
    COMPLETE,
    RUNNING,
    YIELDING,
    SUSPEND,
    TERMINATED
} coro_status;


struct coro_t_
{
  coro_function_t function;
  coro_status status;
  jmp_buf yields_env;
  jmp_buf coro_env;
  int yield_value;
  coro_t *awaiting_coro;
  void *params;
};


inline __attribute__((always_inline)) void coro_inintial(coro_t *coro, coro_function_t function);
int coro_resume(coro_t *coro);
inline __attribute__((always_inline)) bool coro_is_done(coro_t *coro);
inline __attribute__((always_inline)) bool coro_is_suspend(coro_t *coro);
inline __attribute__((always_inline)) bool coro_is_running(coro_t *coro);
inline __attribute__((always_inline)) bool coro_is_yielding(coro_t *coro);
inline __attribute__((always_inline)) bool coro_is_complete(coro_t *coro);
inline __attribute__((always_inline)) bool coro_is_terminated(coro_t *coro);
inline __attribute__((always_inline)) bool coro_is_starting(coro_t *coro);


void coro_inintial(coro_t *coro, coro_function_t function){
  coro->status = STARTING;
  coro->function = function;
}


int coro_resume(coro_t *coro)
{
  if (coro_is_done(coro)) return -1;
  
  if (setjmp(&(coro->yields_env)) != 0) {
      printf(" na te ");
      memset(coro->yields_env, 0, sizeof(jmp_buf));
      coro->status = SUSPEND;
      return coro->yield_value;
    } else {
      printf(" tut ");
      int return_value;
      return_value = (coro->function)(coro);
      coro->status = COMPLETE;
      memset(coro->coro_env, 0, sizeof(jmp_buf));
      memset(coro->yields_env, 0, sizeof(jmp_buf));
      printf(" end ");
      return return_value;
    }
}


bool coro_is_done(coro_t *coro) {
    return (coro->status == COMPLETE) || (coro->status == TERMINATED);
}


bool coro_is_suspend(coro_t *coro) {
    return (coro->status == SUSPEND);
}


bool coro_is_running(coro_t *coro) {
    return (coro->status == RUNNING);
}


bool coro_is_yielding(coro_t *coro) {
    return (coro->status == YIELDING);
}


bool coro_is_complete(coro_t *coro) {
    return (coro->status == COMPLETE);
}


bool coro_is_terminated(coro_t *coro) {
    return (coro->status == TERMINATED);
}


bool coro_is_starting(coro_t *coro) {
    return (coro->status == STARTING);
}

#undef bool