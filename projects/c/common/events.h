#include <stdlib.h>
#include <pthread.h>
#include "lib-c/utils/types.h"

typedef struct
{
  pthread_mutex_t mtx;
  pthread_cond_t  cnd;
} event_st;

BOOL signal_event(void *event_ptr);
BOOL wait_event(void *event_ptr);
BOOL create_event(void **event_ptr);
BOOL destroy_event(void *event_ptr);
