/*
 * spawn.c - system, spawn, exec and waitpid. The program starts itself
 * again: "spawn child CODE ARGS..." checks its arguments and exits with
 * CODE, and "spawn exec CODE" execs a child that exits with CODE.
 *
 * Copyright (C) clang-retro contributors.
 * SPDX-License-Identifier: AGPL-3.0-only
 */
#include <errno.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

static int failed;

static void
check(const char *what, int ok)
{
    printf("%-40s %s\n", what, ok ? "ok" : "FAILED");
    if (!ok)
        failed++;
}

int
main(int argc, char **argv)
{
    const char *self = argv[0];

    if (argc >= 3 && strcmp(argv[1], "child") == 0) {
        int code = atoi(argv[2]);
        /* "child 7 a b": two plain words; "child 8 ...": the hard ones */
        if (code == 8 && !(argc == 6 && strcmp(argv[3], "two words") == 0 &&
                           strcmp(argv[4], "a\"quote") == 0 && argv[5][0] == '\0'))
            return 100;
        if (code == 6) {
            const char *v = getenv("SPAWNTEST");
            if (!v || strcmp(v, "yes") != 0)
                return 101;
        }
        return code;
    }
    if (argc == 3 && strcmp(argv[1], "exec") == 0) {
        execl(self, self, "child", argv[2], (char *)NULL);
        return 102;
    }

    check("spawnl P_WAIT", spawnl(P_WAIT, self, self, "child", "7", "a", "b", (char *)NULL) == 7);

    char *hard[] = { (char *)self, "child", "8", "two words", "a\"quote", "", NULL };
    check("spawnv quoting", spawnv(P_WAIT, self, hard) == 8);

    char *env[] = { "SPAWNTEST=yes", NULL };
    check("spawnle environment",
          spawnle(P_WAIT, self, self, "child", "6", (char *)NULL, env) == 6);

    int   status = 0;
    char *nowait[] = { (char *)self, "child", "5", NULL };
    int   pid = spawnv(P_NOWAIT, self, nowait);
    check("spawnv P_NOWAIT", pid > 0);
    check("waitpid", waitpid(pid, &status, 0) == pid && WIFEXITED(status) &&
                         WEXITSTATUS(status) == 5);
    check("waitpid, no children", waitpid(-1, &status, 0) == -1 && errno == ECHILD);

    check("execl (spawn and exit)",
          spawnl(P_WAIT, self, self, "exec", "9", (char *)NULL) == 9);

    errno = 0;
    check("spawnvp, no such program",
          spawnlp(P_WAIT, "NOSUCH", "NOSUCH", (char *)NULL) == -1 && errno == ENOENT);

    check("system(NULL)", system(NULL) != 0);
    char cmd[300];
    snprintf(cmd, sizeof(cmd), "%s child 4", self);
    status = system(cmd);
    check("system", WIFEXITED(status) && WEXITSTATUS(status) == 4);

    printf("%d failed\n", failed);
    return failed;
}
