/* The C library's directory and process calls on OS/2 (16 and 32-bit).
 * Run from B:\ by runtests.sh. */
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static int failed;

static void
check(const char *what, int ok)
{
    printf("%-36s %s\n", what, ok ? "ok" : "FAILED");
    if (!ok)
        failed++;
}

int
main(void)
{
    char cwd[300];
    check("getcwd", getcwd(cwd, sizeof cwd) != NULL && cwd[1] == ':' && cwd[2] == '\\');
    printf("  cwd %s\n", cwd);
    check("getcwd ERANGE", getcwd(cwd, 2) == NULL && errno == ERANGE);
    check("getpid", getpid() > 0);
    printf("  pid %ld, parent %ld\n", (long)getpid(), (long)getppid());

    check("mkdir", mkdir("B:\\PXDIR", 0777) == 0);
    FILE *f = fopen("B:\\PXDIR\\ONE.TXT", "w");
    check("create a file", f != NULL);
    if (f)
        fclose(f);
    check("mkdir a subdirectory", mkdir("B:\\PXDIR\\SUB", 0777) == 0);

    DIR *d = opendir("B:\\PXDIR");
    check("opendir", d != NULL);
    int files = 0, dirs = 0, total = 0;
    struct dirent *e;
    while (d && (e = readdir(d)) != NULL) {
        total++;
        if (strcmp(e->d_name, "ONE.TXT") == 0 && e->d_type == DT_REG)
            files++;
        if (strcmp(e->d_name, "SUB") == 0 && e->d_type == DT_DIR)
            dirs++;
    }
    check("readdir: ., .., ONE.TXT and SUB", total == 4 && files == 1 && dirs == 1);
    if (d) {
        rewinddir(d);
        int again = 0;
        while (readdir(d))
            again++;
        check("rewinddir", again == 4);
        check("closedir", closedir(d) == 0);
    }

    d = opendir("B:\\PXDIR\\SUB\\");
    int n = 0;
    while (d && readdir(d))
        n++;
    check("opendir with a trailing \\", d != NULL && n == 2);
    if (d)
        closedir(d);

    /* chdir sets the current directory of B:, not the current drive. */
    check("chdir", chdir("B:\\PXDIR\\SUB") == 0);
    check("chdir to a file fails", chdir("B:\\PXDIR\\ONE.TXT") != 0);
    check("chdir back", chdir("B:\\") == 0);

    errno = 0;
    check("opendir a file: ENOTDIR",
          opendir("B:\\PXDIR\\ONE.TXT") == NULL && errno == ENOTDIR);
    errno = 0;
    check("opendir nothing: ENOENT", opendir("B:\\NOSUCH") == NULL && errno == ENOENT);

    unlink("B:\\PXDIR\\ONE.TXT");
    rmdir("B:\\PXDIR\\SUB");
    check("rmdir", rmdir("B:\\PXDIR") == 0);

    printf("%d failed\n", failed);
    return failed;
}
