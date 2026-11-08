/* Reads back the resources os2rc.py bound into this program (restest.rc)
 * with DosGetResource, and checks their bytes.
 *   clang --target=i386-pc-os2 -Os restest.c -o restest.exe
 *   python3 ../tools/os2rc.py -I ../include/os2v2 restest.rc restest.exe
 */
#include <os2.h>
#include <stdio.h>
#include <string.h>

static int failed;

static void
check(const char *what, int ok)
{
    printf("%-40s %s\n", what, ok ? "ok" : "FAILED");
    if (!ok)
        failed++;
}

/* Returns the resource, or NULL; *size is its size. */
static const unsigned char *
get(ULONG type, ULONG id, ULONG *size)
{
    PVOID p;
    if (DosQueryResourceSize(NULLHANDLE, type, id, size) ||
        DosGetResource(NULLHANDLE, type, id, &p))
        return NULL;
    return p;
}

static unsigned
u16(const unsigned char *p)
{
    return p[0] | p[1] << 8;
}

int
main(void)
{
    ULONG n;
    const unsigned char *p;
    PVOID none;

    /* Strings 1 and 18: bundles 1 (0-15) and 2 (16-31). */
    p = get(RT_STRING, 1, &n);
    check("string bundle 1", p && p[2] == 1 && p[3] == 0 && p[4] == 4 &&
                             memcmp(p + 5, "one", 4) == 0);
    p = get(RT_STRING, 2, &n);
    check("string bundle 2", p && p[6] == 9 && memcmp(p + 7, "eighteen", 9) == 0);

    p = get(RT_MENU, 1, &n);
    check("menu", p && u16(p) == n && u16(p + 6) == 4 && u16(p + 8) == 1 &&
                  u16(p + 14) == 10 && memcmp(p + 16, "~File", 6) == 0);

    p = get(RT_ACCELTABLE, 1, &n);
    check("accelerator table", p && n == 16 && u16(p) == 2 &&
                               u16(p + 4) == AF_VIRTUALKEY && u16(p + 6) == VK_F3 &&
                               u16(p + 10) == (AF_CHAR | AF_CONTROL) &&
                               u16(p + 12) == 'Q' && u16(p + 14) == 11);

    p = get(RT_DIALOG, 1, &n);
    {
        const DLGTEMPLATE *t = (const DLGTEMPLATE *)p;
        check("dialog template", p && t->cbTemplate == n &&
                                 t->adlgti[0].cChildren == 1 &&
                                 t->adlgti[1].id == DID_OK &&
                                 memcmp(p + t->adlgti[1].offText, "OK", 3) == 0);
    }

    p = get(RT_RCDATA, 7, &n);
    check("rcdata", p && n == 9 && u16(p) == 0x1234 &&
                    memcmp(p + 2, "\xBC\x9A\x78\x56xyz", 7) == 0);

    check("missing resource", DosGetResource(NULLHANDLE, RT_RCDATA, 8, &none) != 0);

    printf("%d failed\n", failed);
    return failed;
}
