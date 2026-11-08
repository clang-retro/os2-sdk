/*
 * spawn.c - system, spawn, exec, waitpid, pipe, dup and popen. The
 * program starts itself again: "spawn child CODE ARGS..." checks its
 * arguments and exits with CODE, "spawn exec CODE" execs a child that
 * exits with CODE, "spawn echo" writes a line and "spawn read" exits with
 * 42 when it reads "hello".
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
    if (argc == 2 && strcmp(argv[1], "echo") == 0) {
        printf("from the child\n");
        return 0;
    }
    if (argc == 2 && strcmp(argv[1], "read") == 0) {
        char line[40];
        return fgets(line, sizeof(line), stdin) && strncmp(line, "hello", 5) == 0 ? 42 : 103;
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

    int  fds[2];
    char buf[16] = "";
    check("pipe", pipe(fds) == 0 && write(fds[1], "abc", 3) == 3 && close(fds[1]) == 0 &&
                      read(fds[0], buf, sizeof(buf)) == 3 && memcmp(buf, "abc", 3) == 0 &&
                      read(fds[0], buf, sizeof(buf)) == 0 && close(fds[0]) == 0);
    int d = dup(1);
    check("dup, dup2", d > 2 && dup2(d, 9) == 9 && write(9, "", 0) == 0 && close(9) == 0 &&
                           close(d) == 0);

    snprintf(cmd, sizeof(cmd), "%s echo", self);
    FILE *f = popen(cmd, "r");
    char  line[40] = "";
    check("popen \"r\"", f && fgets(line, sizeof(line), f) &&
                             strncmp(line, "from the child", 14) == 0);
    status = f ? pclose(f) : -1;
    check("pclose", WIFEXITED(status) && WEXITSTATUS(status) == 0);
    snprintf(cmd, sizeof(cmd), "%s read", self);
    f = popen(cmd, "w");
    check("popen \"w\"", f && fputs("hello\n", f) >= 0);
    status = f ? pclose(f) : -1;
    check("pclose, exit code", WIFEXITED(status) && WEXITSTATUS(status) == 42);

    printf("%d failed\n", failed);
    return failed;
}
