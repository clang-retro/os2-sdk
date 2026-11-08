/*
 * threads.c - C11 threads on 32-bit OS/2: thrd_create and thrd_join,
 * mutexes, condition variables, tss, call_once, __thread variables and
 * errno kept per thread, and malloc and printf from several threads.
 *
 * Copyright (C) clang-retro contributors.
 * SPDX-License-Identifier: AGPL-3.0-only
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

#define THREADS 4
#define ROUNDS  2000

static int failed;

static void
check(const char *what, int ok)
{
    printf("%-40s %s\n", what, ok ? "ok" : "FAILED");
    if (!ok)
        failed++;
}

static int
add(void *arg)
{
    return *(int *)arg + 1;
}

/* Many threads, one counter. */
static mtx_t counter_lock;
static long  counter;

static int
count(void *arg)
{
    (void)arg;
    for (int i = 0; i < ROUNDS; i++) {
        mtx_lock(&counter_lock);
        long c = counter;
        if ((i & 63) == 0)
            thrd_yield();
        counter = c + 1;
        mtx_unlock(&counter_lock);
    }
    return 0;
}

/* A queue of one, filled and emptied under a condition variable. */
static mtx_t box_lock;
static cnd_t box_full, box_empty;
static int   box, box_has, box_sum;

static int
consume(void *arg)
{
    (void)arg;
    for (int i = 1; i <= 100; i++) {
        mtx_lock(&box_lock);
        while (!box_has)
            cnd_wait(&box_full, &box_lock);
        box_sum += box;
        box_has = 0;
        cnd_signal(&box_empty);
        mtx_unlock(&box_lock);
    }
    return 0;
}

/* __thread and errno: each thread sees its own. */
static __thread int mine = 5;

static int
own(void *arg)
{
    int n = *(int *)arg;
    if (mine != 5)
        return -1;
    mine = n;
    errno = n;
    thrd_sleep(&(struct timespec){ .tv_nsec = 20000000 }, NULL);
    return mine == n && errno == n ? 0 : -1;
}

/* tss: the destructor runs at thread exit. */
static tss_t key;
static int   dtor_calls;

static void
dtor(void *p)
{
    __atomic_fetch_add(&dtor_calls, 1, __ATOMIC_RELAXED);
    free(p);
}

static int
use_key(void *arg)
{
    tss_set(key, malloc(16));
    return tss_get(key) && arg == NULL ? 0 : -1;
}

static once_flag once = ONCE_FLAG_INIT;
static int       once_calls;

static void
init_once(void)
{
    once_calls++;
}

static int
call_it(void *arg)
{
    (void)arg;
    call_once(&once, init_once);
    return 0;
}

/* malloc, free and snprintf from several threads at once. */
static int
churn(void *arg)
{
    int   n = *(int *)arg;
    char *p[16];
    for (int r = 0; r < 200; r++) {
        for (int i = 0; i < 16; i++) {
            p[i] = malloc(32 + i * n);
            if (!p[i])
                return -1;
            snprintf(p[i], 32, "%d-%d", n, i);
        }
        for (int i = 0; i < 16; i++) {
            char want[32];
            snprintf(want, sizeof(want), "%d-%d", n, i);
            if (strcmp(p[i], want) != 0)
                return -1;
            free(p[i]);
        }
    }
    return 0;
}

static mtx_t held;

static int
try_held(void *arg)
{
    (void)arg;
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    ts.tv_nsec += 50000000;
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
    }
    return mtx_trylock(&held) == thrd_busy && mtx_timedlock(&held, &ts) == thrd_timedout ? 0 : -1;
}

static int
detached(void *arg)
{
    (void)arg;
    return 0;
}

int
main(void)
{
    thrd_t t[THREADS];
    int    arg[THREADS], res;

    arg[0] = 41;
    res = 0;
    check("thrd_create and thrd_join",
          thrd_create(&t[0], add, &arg[0]) == thrd_success && thrd_join(t[0], &res) == thrd_success &&
              res == 42);
    check("thrd_current, thrd_equal",
          thrd_equal(thrd_current(), thrd_current()) && !thrd_equal(thrd_current(), t[0]));

    mtx_init(&counter_lock, mtx_plain);
    for (int i = 0; i < THREADS; i++)
        thrd_create(&t[i], count, NULL);
    for (int i = 0; i < THREADS; i++)
        thrd_join(t[i], NULL);
    check("mtx_lock: no lost counts", counter == (long)THREADS * ROUNDS);

    mtx_t r;
    check("mtx_recursive", mtx_init(&r, mtx_recursive) == thrd_success && mtx_lock(&r) == 0 &&
                               mtx_lock(&r) == 0 && mtx_unlock(&r) == 0 && mtx_unlock(&r) == 0);
    mtx_destroy(&r);

    mtx_t z = { 0 };
    check("zeroed mtx_t locks", mtx_lock(&z) == thrd_success && mtx_unlock(&z) == thrd_success);

    mtx_init(&held, mtx_timed);
    mtx_lock(&held);
    res = -1;
    thrd_create(&t[0], try_held, NULL);
    thrd_join(t[0], &res);
    mtx_unlock(&held);
    mtx_destroy(&held);
    check("mtx_trylock, mtx_timedlock when held", res == 0);

    mtx_init(&box_lock, mtx_plain);
    cnd_init(&box_full);
    cnd_init(&box_empty);
    thrd_create(&t[0], consume, NULL);
    for (int i = 1; i <= 100; i++) {
        mtx_lock(&box_lock);
        while (box_has)
            cnd_wait(&box_empty, &box_lock);
        box = i;
        box_has = 1;
        cnd_signal(&box_full);
        mtx_unlock(&box_lock);
    }
    thrd_join(t[0], NULL);
    check("cnd_wait and cnd_signal", box_sum == 5050);

    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    ts.tv_nsec += 100000000;
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
    }
    mtx_lock(&box_lock);
    check("cnd_timedwait times out", cnd_timedwait(&box_full, &box_lock, &ts) == thrd_timedout);
    mtx_unlock(&box_lock);

    mine = 9;
    errno = 9;
    for (int i = 0; i < THREADS; i++) {
        arg[i] = 100 + i;
        thrd_create(&t[i], own, &arg[i]);
    }
    int own_ok = 1;
    for (int i = 0; i < THREADS; i++) {
        thrd_join(t[i], &res);
        own_ok &= res == 0;
    }
    check("__thread and errno per thread", own_ok && mine == 9 && errno == 9);

    check("tss_create", tss_create(&key, dtor) == thrd_success);
    for (int i = 0; i < THREADS; i++)
        thrd_create(&t[i], use_key, NULL);
    int key_ok = 1;
    for (int i = 0; i < THREADS; i++) {
        thrd_join(t[i], &res);
        key_ok &= res == 0;
    }
    check("tss destructors at thread exit", key_ok && dtor_calls == THREADS);
    tss_delete(key);

    for (int i = 0; i < THREADS; i++)
        thrd_create(&t[i], call_it, NULL);
    for (int i = 0; i < THREADS; i++)
        thrd_join(t[i], NULL);
    check("call_once", once_calls == 1);

    for (int i = 0; i < THREADS; i++) {
        arg[i] = i + 1;
        thrd_create(&t[i], churn, &arg[i]);
    }
    int churn_ok = 1;
    for (int i = 0; i < THREADS; i++) {
        thrd_join(t[i], &res);
        churn_ok &= res == 0;
    }
    check("malloc and snprintf in threads", churn_ok);

    thrd_t d;
    check("thrd_detach", thrd_create(&d, detached, NULL) == thrd_success &&
                             thrd_detach(d) == thrd_success);
    thrd_sleep(&(struct timespec){ .tv_nsec = 50000000 }, NULL);

    printf("%s\n", failed ? "FAILED" : "all ok");
    return failed != 0;
}
