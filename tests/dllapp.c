/* dllapp.c - calls testdll.c's DLL (TESTDLL.DLL), which must be on
 * LIBPATH. Run from B:\ by runtests.sh. With an argument, it is the child
 * that checks that each process has its own copy of the DLL's data.
 *
 * Copyright (C) clang-retro contributors.
 * SPDX-License-Identifier: AGPL-3.0-only
 */
#include <os2.h>
#include <stdio.h>
#include <string.h>

int __far __pascal DllCount(void);
int __far __pascal DllCtorRan(void);
int __far __pascal DllFormat(char __far *out, unsigned size, int n);
int __far __pascal DllHeap(void);
int __far __pascal DllGetEnv(const char __far *name, char __far *out, unsigned size);

static int failed;

static void
check(const char *what, int ok)
{
    printf("%s %s\n", ok ? "ok  " : "FAIL", what);
    if (!ok)
        failed++;
}

int
main(int argc, char **argv)
{
    if (argc > 1)
        return DllCount(); /* the child: 1 if its DLL data is its own */

    char buf[64];
    setvbuf(stdout, NULL, _IONBF, 0); /* all the output before a crash */
    check("constructor ran", DllCtorRan() == 1);
    check("count 1", DllCount() == 1);
    check("count 2", DllCount() == 2);
    int len = DllFormat(buf, sizeof buf, 300);
    check("format", len == 13 && strcmp(buf, "n=300 hex=12c") == 0);
    printf("     %s\n", buf);
    check("heap", DllHeap());
    check("getenv COMSPEC", DllGetEnv("COMSPEC", buf, sizeof buf) && strchr(buf, '\\'));
    printf("     %s\n", buf);
    check("getenv missing", !DllGetEnv("NO_SUCH_VARIABLE", buf, sizeof buf));

    /* A second process gets the DLL's data afresh. */
    RESULTCODES rc;
    char args[] = "B:\\DLLAPP.EXE\0child\0";
    USHORT err = DosExecPgm(NULL, 0, EXEC_SYNC, (PSZ)args, NULL, &rc, (PSZ)"B:\\DLLAPP.EXE");
    check("child process", err == 0 && rc.codeResult == 1);
    printf("     rc %u result %u\n", err, rc.codeResult);
    check("count 3 after the child", DllCount() == 3);

    printf("%d failed\n", failed);
    return failed != 0;
}
