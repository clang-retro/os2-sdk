/* Calls the OS/2 API directly: the version, the date and time, the
 * current drive, and a file written, read back and deleted. Builds for
 * both 16-bit (i8086-pc-os2) and 32-bit (i386-pc-os2) OS/2. */
#include <os2.h>
#include <stdio.h>
#include <string.h>

static void
put(const char *s)
{
#ifdef __32BIT__
    ULONG n;
#else
    USHORT n;
#endif
    DosWrite(1, (PVOID)s, strlen(s), &n);
}

int
main(void)
{
    char     buf[128];
    DATETIME dt;
#ifdef __32BIT__
    ULONG ver[3], disk, drives;
    DosQuerySysInfo(QSV_VERSION_MAJOR, QSV_VERSION_REVISION, ver, sizeof(ver));
    snprintf(buf, sizeof(buf), "OS/2 version %lu.%lu (32-bit program)\r\n", ver[0] / 10,
             ver[1]);
    DosQueryCurrentDisk(&disk, &drives);
#else
    USHORT ver;
    USHORT disk;
    ULONG  drives;
    DosGetVersion(&ver);
    snprintf(buf, sizeof(buf), "OS/2 version %u.%u (16-bit program)\r\n", HIBYTE(ver) / 10,
             LOBYTE(ver));
    DosQCurDisk(&disk, &drives);
#endif
    put(buf);
    DosGetDateTime(&dt);
    snprintf(buf, sizeof(buf), "Date %04u-%02u-%02u, time %02u:%02u:%02u\r\n",
             (unsigned)dt.year, (unsigned)dt.month, (unsigned)dt.day, (unsigned)dt.hours,
             (unsigned)dt.minutes, (unsigned)dt.seconds);
    put(buf);
    snprintf(buf, sizeof(buf), "Current drive %c:\r\n", (char)('A' + disk - 1));
    put(buf);

    FILE *f = fopen("SYSINFO.TMP", "w");
    if (!f) {
        perror("SYSINFO.TMP");
        return 1;
    }
    fprintf(f, "line %d\n", 42);
    fclose(f);
    f = fopen("SYSINFO.TMP", "r");
    int v = 0;
    if (!f || fscanf(f, "line %d", &v) != 1 || v != 42) {
        puts("file test FAILED");
        return 1;
    }
    fclose(f);
    remove("SYSINFO.TMP");
    puts("file test passed");
    return 0;
}
