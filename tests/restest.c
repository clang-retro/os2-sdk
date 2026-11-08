/* Reads back the resources os2rc.py bound into this program (restest.rc)
 * with DosGetResource, and checks their bytes.
 *   clang --target=i386-pc-os2 -Os restest.c -o restest.exe
 *   python3 ../tools/os2rc.py restest.rc restest.exe
 * and the same for 16-bit, with --target=i8086-pc-os2 (DosGetResource2).
 */
#include <os2.h>
#include <stdio.h>

static int failed;

static void
check(const char *what, int ok)
{
    printf("%-40s %s\n", what, ok ? "ok" : "FAILED");
    if (!ok)
        failed++;
}

typedef const unsigned char FAR *RES;

/* Returns the resource, or NULL; *size is its size. */
static RES
get(ULONG type, ULONG id, ULONG *size)
{
    PVOID p;
#ifdef __32BIT__
    if (DosQueryResourceSize(NULLHANDLE, type, id, size) ||
        DosGetResource(NULLHANDLE, type, id, &p))
        return NULL;
#else
    /* The resource is a segment of its own, as long as the data. */
    if (DosGetResource2(0, (USHORT)type, (USHORT)id, &p) ||
        DosSizeSeg(SELECTOROF(p), size))
        return NULL;
#endif
    return p;
}

static unsigned
u16(RES p)
{
    return p[0] | p[1] << 8;
}

/* memcmp, for far pointers in the 16-bit memory models */
static int
same(RES p, const char *s, unsigned n)
{
    while (n--)
        if (*p++ != (unsigned char)*s++)
            return 0;
    return 1;
}

int
main(void)
{
    ULONG n;
    RES p;
    PVOID none;

    /* Strings 1 and 18: bundles 1 (0-15) and 2 (16-31). */
    p = get(RT_STRING, 1, &n);
    check("string bundle 1", p && p[2] == 1 && p[3] == 0 && p[4] == 4 &&
                             same(p + 5, "one", 4));
    p = get(RT_STRING, 2, &n);
    check("string bundle 2", p && p[6] == 9 && same(p + 7, "eighteen", 9));

    p = get(RT_MENU, 1, &n);
    check("menu", p && u16(p) == n && u16(p + 6) == 4 && u16(p + 8) == 1 &&
                  u16(p + 14) == 10 && same(p + 16, "~File", 6));

    p = get(RT_ACCELTABLE, 1, &n);
    check("accelerator table", p && n == 16 && u16(p) == 2 &&
                               u16(p + 4) == AF_VIRTUALKEY && u16(p + 6) == VK_F3 &&
                               u16(p + 10) == (AF_CHAR | AF_CONTROL) &&
                               u16(p + 12) == 'Q' && u16(p + 14) == 11);

    p = get(RT_DIALOG, 1, &n);
    {
        const DLGTEMPLATE FAR *t = (const DLGTEMPLATE FAR *)p;
        check("dialog template", p && t->cbTemplate == n &&
                                 t->adlgti[0].cChildren == 1 &&
                                 t->adlgti[1].id == DID_OK &&
                                 same(p + t->adlgti[1].offText, "OK", 3));
    }

    p = get(RT_RCDATA, 7, &n);
    check("rcdata", p && n == 9 && u16(p) == 0x1234 &&
                    same(p + 2, "\xBC\x9A\x78\x56xyz", 7));

#ifdef __32BIT__
    check("missing resource", DosGetResource(NULLHANDLE, RT_RCDATA, 8, &none) != 0);
#else
    check("missing resource", DosGetResource2(0, RT_RCDATA, 8, &none) != 0);
#endif

    printf("%d failed\n", failed);
    return failed;
}
