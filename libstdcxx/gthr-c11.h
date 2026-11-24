/* Threads for libstdc++ on the C11 threads of the C library (<threads.h>).

   build-libstdcxx.sh copies this file into GCC's libgcc/config as
   gthr-c11.h and selects it as the thread model "c11". It maps GCC's
   gthreads interface (libgcc/gthr.h) onto thrd_create, mtx_*, cnd_*,
   tss_* and call_once, which the OS/2 C library implements with OS/2's
   threads and semaphores.

   This file is part of clang-retro/os2-sdk. It is under the GPL 3 with
   the GCC Runtime Library Exception, as the libgcc headers it is made
   like.  */

#ifndef GCC_GTHR_C11_H
#define GCC_GTHR_C11_H

#define __GTHREADS 1
#define __GTHREADS_CXX0X 1
#define __GTHREAD_HAS_COND 1

#include <threads.h>
#include <stdlib.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef thrd_t __gthread_t;
typedef tss_t __gthread_key_t;
typedef once_flag __gthread_once_t;
typedef mtx_t __gthread_mutex_t;
typedef mtx_t __gthread_recursive_mutex_t;
typedef cnd_t __gthread_cond_t;
typedef struct timespec __gthread_time_t;

/* C11 mutexes and condition variables have no static initializer.  */
#define __GTHREAD_ONCE_INIT ONCE_FLAG_INIT
#define __GTHREAD_MUTEX_INIT_FUNCTION __gthread_mutex_init_function
#define __GTHREAD_RECURSIVE_MUTEX_INIT_FUNCTION \
  __gthread_recursive_mutex_init_function
#define __GTHREAD_COND_INIT_FUNCTION __gthread_cond_init_function
#define __GTHREAD_TIME_INIT {0, 0}

#ifdef __cplusplus
# define __GTHREAD_INLINE inline __attribute__((__always_inline__))
#else
# define __GTHREAD_INLINE static inline
#endif

/* Tells if the program can have threads. It can always.  */
__GTHREAD_INLINE int
__gthread_active_p (void)
{
  return 1;
}

/* The function and argument of a thread that __gthread_create starts.  */
struct __gthread_c11_start
{
  void *(*__func) (void *);
  void *__arg;
};

/* Runs the function of a thread that __gthread_create started, and frees
   its start data.  */
static int
__gthread_c11_trampoline (void *__p)
{
  struct __gthread_c11_start __s = *(struct __gthread_c11_start *) __p;
  free (__p);
  __s.__func (__s.__arg);
  return 0;
}

/* Starts a thread that runs FUNC (ARGS), and gives it in THR.  */
__GTHREAD_INLINE int
__gthread_create (__gthread_t *__thr, void *(*__func) (void *),
		  void *__args)
{
  struct __gthread_c11_start *__s
    = (struct __gthread_c11_start *) malloc (sizeof *__s);
  if (!__s)
    return 1;
  __s->__func = __func;
  __s->__arg = __args;
  if (thrd_create (__thr, __gthread_c11_trampoline, __s) != thrd_success)
    {
      free (__s);
      return 1;
    }
  return 0;
}

/* Waits for THR to end. The thread's value is not kept: VALUE_PTR
   gets a null pointer.  */
__GTHREAD_INLINE int
__gthread_join (__gthread_t __thr, void **__value_ptr)
{
  int __res;
  if (thrd_join (__thr, &__res) != thrd_success)
    return 1;
  if (__value_ptr)
    *__value_ptr = 0;
  return 0;
}

/* Lets THR end without a join.  */
__GTHREAD_INLINE int
__gthread_detach (__gthread_t __thr)
{
  return thrd_detach (__thr) == thrd_success ? 0 : 1;
}

/* Tells if T1 and T2 are the same thread.  */
__GTHREAD_INLINE int
__gthread_equal (__gthread_t __t1, __gthread_t __t2)
{
  return thrd_equal (__t1, __t2);
}

/* Gives the thread that calls it.  */
__GTHREAD_INLINE __gthread_t
__gthread_self (void)
{
  return thrd_current ();
}

/* Gives the rest of the time slice to other threads.  */
__GTHREAD_INLINE int
__gthread_yield (void)
{
  thrd_yield ();
  return 0;
}

/* Runs FUNC once, the first time that a thread calls it with ONCE.  */
__GTHREAD_INLINE int
__gthread_once (__gthread_once_t *__once, void (*__func) (void))
{
  call_once (__once, __func);
  return 0;
}

/* Makes a thread-local key. DTOR runs for each thread's value when the
   thread ends.  */
__GTHREAD_INLINE int
__gthread_key_create (__gthread_key_t *__key, void (*__dtor) (void *))
{
  return tss_create (__key, __dtor) == thrd_success ? 0 : 1;
}

/* Frees the thread-local key KEY.  */
__GTHREAD_INLINE int
__gthread_key_delete (__gthread_key_t __key)
{
  tss_delete (__key);
  return 0;
}

/* Gives this thread's value of KEY.  */
__GTHREAD_INLINE void *
__gthread_getspecific (__gthread_key_t __key)
{
  return tss_get (__key);
}

/* Sets this thread's value of KEY.  */
__GTHREAD_INLINE int
__gthread_setspecific (__gthread_key_t __key, const void *__ptr)
{
  return tss_set (__key, (void *) __ptr) == thrd_success ? 0 : 1;
}

/* Makes the mutex MUTEX. It can be timed (std::timed_mutex).  */
__GTHREAD_INLINE void
__gthread_mutex_init_function (__gthread_mutex_t *__mutex)
{
  mtx_init (__mutex, mtx_plain | mtx_timed);
}

/* Frees the mutex MUTEX.  */
__GTHREAD_INLINE int
__gthread_mutex_destroy (__gthread_mutex_t *__mutex)
{
  mtx_destroy (__mutex);
  return 0;
}

/* Takes MUTEX.  */
__GTHREAD_INLINE int
__gthread_mutex_lock (__gthread_mutex_t *__mutex)
{
  return mtx_lock (__mutex) == thrd_success ? 0 : 1;
}

/* Takes MUTEX if no thread has it. Gives 0 if it took it.  */
__GTHREAD_INLINE int
__gthread_mutex_trylock (__gthread_mutex_t *__mutex)
{
  return mtx_trylock (__mutex) == thrd_success ? 0 : 1;
}

/* Takes MUTEX, waiting until ABS_TIMEOUT (TIME_UTC) at most.  */
__GTHREAD_INLINE int
__gthread_mutex_timedlock (__gthread_mutex_t *__mutex,
			   const __gthread_time_t *__abs_timeout)
{
  return mtx_timedlock (__mutex, __abs_timeout) == thrd_success ? 0 : 1;
}

/* Releases MUTEX.  */
__GTHREAD_INLINE int
__gthread_mutex_unlock (__gthread_mutex_t *__mutex)
{
  return mtx_unlock (__mutex) == thrd_success ? 0 : 1;
}

/* Makes the recursive mutex MUTEX.  */
__GTHREAD_INLINE int
__gthread_recursive_mutex_init_function (__gthread_recursive_mutex_t *__mutex)
{
  return mtx_init (__mutex, mtx_recursive | mtx_timed) == thrd_success
	 ? 0 : 1;
}

/* Frees the recursive mutex MUTEX.  */
__GTHREAD_INLINE int
__gthread_recursive_mutex_destroy (__gthread_recursive_mutex_t *__mutex)
{
  mtx_destroy (__mutex);
  return 0;
}

/* Takes the recursive mutex MUTEX, again if this thread has it.  */
__GTHREAD_INLINE int
__gthread_recursive_mutex_lock (__gthread_recursive_mutex_t *__mutex)
{
  return __gthread_mutex_lock (__mutex);
}

/* Takes the recursive mutex MUTEX if no other thread has it.  */
__GTHREAD_INLINE int
__gthread_recursive_mutex_trylock (__gthread_recursive_mutex_t *__mutex)
{
  return __gthread_mutex_trylock (__mutex);
}

/* Takes the recursive mutex MUTEX, waiting until ABS_TIMEOUT at most.  */
__GTHREAD_INLINE int
__gthread_recursive_mutex_timedlock (__gthread_recursive_mutex_t *__mutex,
				     const __gthread_time_t *__abs_timeout)
{
  return __gthread_mutex_timedlock (__mutex, __abs_timeout);
}

/* Releases the recursive mutex MUTEX once.  */
__GTHREAD_INLINE int
__gthread_recursive_mutex_unlock (__gthread_recursive_mutex_t *__mutex)
{
  return __gthread_mutex_unlock (__mutex);
}

/* Makes the condition variable COND.  */
__GTHREAD_INLINE void
__gthread_cond_init_function (__gthread_cond_t *__cond)
{
  cnd_init (__cond);
}

/* Wakes each thread that waits on COND.  */
__GTHREAD_INLINE int
__gthread_cond_broadcast (__gthread_cond_t *__cond)
{
  return cnd_broadcast (__cond) == thrd_success ? 0 : 1;
}

/* Wakes one thread that waits on COND.  */
__GTHREAD_INLINE int
__gthread_cond_signal (__gthread_cond_t *__cond)
{
  return cnd_signal (__cond) == thrd_success ? 0 : 1;
}

/* Releases MUTEX, waits on COND and takes MUTEX again.  */
__GTHREAD_INLINE int
__gthread_cond_wait (__gthread_cond_t *__cond, __gthread_mutex_t *__mutex)
{
  return cnd_wait (__cond, __mutex) == thrd_success ? 0 : 1;
}

/* The same with a recursive mutex.  */
__GTHREAD_INLINE int
__gthread_cond_wait_recursive (__gthread_cond_t *__cond,
			       __gthread_recursive_mutex_t *__mutex)
{
  return __gthread_cond_wait (__cond, __mutex);
}

/* Releases MUTEX, waits on COND until ABS_TIMEOUT (TIME_UTC) at most and
   takes MUTEX again. Gives a value other than 0 when the time ran out.  */
__GTHREAD_INLINE int
__gthread_cond_timedwait (__gthread_cond_t *__cond,
			  __gthread_mutex_t *__mutex,
			  const __gthread_time_t *__abs_timeout)
{
  return cnd_timedwait (__cond, __mutex, __abs_timeout) == thrd_success
	 ? 0 : 1;
}

/* Frees the condition variable COND.  */
__GTHREAD_INLINE int
__gthread_cond_destroy (__gthread_cond_t *__cond)
{
  cnd_destroy (__cond);
  return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* ! GCC_GTHR_C11_H */
