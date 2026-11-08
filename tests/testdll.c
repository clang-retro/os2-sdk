/* testdll.c - a 16-bit OS/2 DLL that uses the C library, for dllapp.c.
 * Built with clang --target=i8086-pc-os2 -shared (see tests/README.md).
 *
 * Copyright (C) clang-retro contributors.
 * SPDX-License-Identifier: AGPL-3.0-only
 */
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int counter; /* each process has its own */
static int ctor_ran;

__attribute__((constructor)) static void
init(void)
{
    ctor_ran = 1;
}

/* Counts the calls in this process. */
int __export __far __pascal
DllCount(void)
{
    return ++counter;
}

int __export __far __pascal
DllCtorRan(void)
{
    return ctor_ran;
}

static void
fcopy(char __far *out, const char *s, unsigned size)
{
    unsigned i = 0;
    for (; s[i] && i + 1 < size; i++)
        out[i] = s[i];
    out[i] = '\0';
}

/* A local whose address escapes (buf), and printf, with SS != DS. */
int __export __far __pascal
DllFormat(char __far *out, unsigned size, int n)
{
    char buf[40];
    int  len = snprintf(buf, sizeof buf, "n=%d hex=%x", n, n);
    fcopy(out, buf, size);
    return len;
}

/* The heap, from the DLL's DGROUP or its own segment. */
int __export __far __pascal
DllHeap(void)
{
    char *p = malloc(3000), *q = malloc(5000);
    int   ok = p && q;
    if (ok) {
        memset(p, 'a', 3000);
        memset(q, 'b', 5000);
        ok = p[2999] == 'a' && q[0] == 'b';
    }
    free(p);
    free(q);
    return ok;
}

/* The environment of the process that loaded the DLL. */
int __export __far __pascal
DllGetEnv(const char __far *name, char __far *out, unsigned size)
{
    char n[32];
    unsigned i = 0;
    for (; name[i] && i + 1 < sizeof n; i++)
        n[i] = name[i];
    n[i] = '\0';
    const char *v = getenv(n);
    if (!v)
        return 0;
    fcopy(out, v, size);
    return 1;
}
