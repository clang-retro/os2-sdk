/* The 16-bit Vio, Kbd and Mou calls from a 32-bit program (bsesub.h and
 * the C library's thunk). Run from B:\ by runtests.sh; what it writes on
 * the screen is read back from the screen. */
#include <os2.h>
#include <stdio.h>
#include <string.h>

static int failed;

static void
check(const char *what, int ok, unsigned long v)
{
    printf("%-32s %s (%lu)\n", what, ok ? "ok" : "FAILED", v);
    if (!ok)
        failed++;
}

int
main(void)
{
    USHORT       rc, row = 99, col = 99, n, cp = 0, ansi = 9;
    VIOMODEINFO  mode = { sizeof mode };
    VIOCURSORINFO cur;
    char         buf[80];
    BYTE         cell[2] = { ' ', 0x1f };
    KBDINFO      kbd = { sizeof kbd };
    KBDKEYINFO   key;

    rc = VioWrtTTY("Hello from 32-bit code through VIOCALLS\r\n", 41, 0);
    check("VioWrtTTY", rc == 0, rc);
    rc = VioGetMode(&mode, 0);
    check("VioGetMode", rc == 0 && mode.col == 80 && mode.row >= 25, mode.row);
    rc = VioGetCurPos(&row, &col, 0);
    check("VioGetCurPos", rc == 0 && row < mode.row && col == 0, row);
    rc = VioGetCurType(&cur, 0);
    check("VioGetCurType", rc == 0, cur.cEnd);

    rc = VioWrtCharStr("thunk", 5, 2, 70, 0);
    check("VioWrtCharStr", rc == 0, rc);
    n = sizeof buf;
    memset(buf, 0, sizeof buf);
    n = 5;
    rc = VioReadCharStr(buf, &n, 2, 70, 0);
    check("VioReadCharStr reads it back", rc == 0 && n == 5 && memcmp(buf, "thunk", 5) == 0, n);
    rc = VioWrtNAttr(&cell[1], 5, 2, 70, 0);
    n = 4;
    rc |= VioReadCellStr(buf, &n, 2, 71, 0);
    check("VioWrtNAttr, VioReadCellStr", rc == 0 && n == 4 && buf[0] == 'h' && (BYTE)buf[1] == 0x1f, n);
    rc = VioSetCurPos(row, 0, 0);
    check("VioSetCurPos", rc == 0, rc);
    rc = VioGetCp(0, &cp, 0);
    check("VioGetCp", rc == 0 && cp != 0, cp);
    rc = VioGetAnsi(&ansi, 0);
    check("VioGetAnsi", rc == 0 && ansi <= 1, ansi);

    /* A buffer across a 64 KiB boundary is copied for the 16-bit call. */
    PVOID mem;
    DosAllocMem(&mem, 0x20000, PAG_COMMIT | PAG_READ | PAG_WRITE);
    char *across = (char *)mem + 0x10000 - 3;
    memcpy(across, "boundary", 8);
    rc = VioWrtCharStr(across, 8, 3, 70, 0);
    memset(across, 0, 8);
    n = 8;
    rc |= VioReadCharStr(across, &n, 3, 70, 0);
    check("buffers across 64 KiB", rc == 0 && n == 8 && memcmp(across, "boundary", 8) == 0, n);
    DosFreeMem(mem);

    rc = KbdGetStatus(&kbd, 0);
    check("KbdGetStatus", rc == 0 && kbd.fsMask != 0, kbd.fsMask);
    /* The Enter that runtests.sh pressed at boot is reported, and stays
       reported, as a 16-bit program sees it too. */
    rc = KbdCharIn(&key, IO_NOWAIT, 0);
    check("KbdCharIn, no wait", rc == 0, key.chChar);
    rc = KbdPeek(&key, 0);
    check("KbdPeek", rc == 0, key.fbStatus);

    HMOU hmou;
    rc = MouOpen(NULL, &hmou);
    /* The boot diskettes have no mouse driver: ERROR_MOUSE_NO_DEVICE. */
    check("MouOpen", rc == 0 || rc == ERROR_MOUSE_NO_DEVICE, rc);
    if (rc == 0)
        MouClose(hmou);

    printf("%d failed\n", failed);
    return failed;
}
