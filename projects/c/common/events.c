#include "events.h"

BOOL signal_event(void *event_ptr)
{
  event_st *e;

  if (event_ptr == NULL)
    return FALSE;

  e = event_ptr;

  pthread_mutex_lock(&(e->mtx));
  pthread_cond_signal(&(e->cnd));
  pthread_mutex_unlock(&(e->mtx));

  return TRUE;
}

BOOL wait_event(void *event_ptr)
{
  event_st *e;

  if (event_ptr == NULL)
    return FALSE;

  e = event_ptr;

  pthread_mutex_lock(&(e->mtx));
  pthread_cond_wait(&(e->cnd), &(e->mtx));
  pthread_mutex_unlock(&(e->mtx));

  return TRUE;
}

BOOL create_event(void **event_ptr)
{
  event_st *e;

  if (event_ptr == NULL)
    return FALSE;

  e = malloc(sizeof(event_st));
  if (e == NULL)
    return FALSE;

  pthread_mutex_init(&(e->mtx), NULL);
  pthread_cond_init (&(e->cnd), NULL);

  *event_ptr = e;

  return TRUE;
}

BOOL destroy_event(void *event_ptr)
{
  event_st *e;

  if (event_ptr == NULL)
    return FALSE;

  e = event_ptr;
  
  pthread_mutex_destroy(&(e->mtx));
  pthread_cond_destroy(&(e->cnd));

  free(event_ptr);
  return TRUE;
}
