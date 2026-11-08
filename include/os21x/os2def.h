/*
 * os2def.h - basic types and macros of the 16-bit OS/2 1.x API, for
 * clang-retro (i8086-pc-os2). Written from the OS/2 1.x programming
 * documentation; see the SDK's README.
 */
#ifndef __OS2DEF_H
#define __OS2DEF_H

#define FAR     __far
#define NEAR    __near
#define PASCAL  __pascal
#define CDECL
#define APIENTRY __far __pascal
#define EXPENTRY __far __pascal __loadds
#define PASCAL16 __pascal

#define VOID    void
#define NULL_SEL 0
#ifndef NULL
#define NULL ((void *)0)
#endif
#define FALSE 0
#define TRUE  1

typedef unsigned short  SHANDLE;
typedef void FAR       *LHANDLE;
typedef unsigned char   UCHAR, BYTE;
typedef char            CHAR;
typedef short           SHORT;
typedef unsigned short  USHORT, BOOL, SEL;
typedef long            LONG;
typedef unsigned long   ULONG;
typedef int             INT;
typedef unsigned int    UINT;

typedef CHAR FAR *PCHAR;
typedef CHAR FAR *PCH;
typedef CHAR FAR *PSZ;
typedef const CHAR FAR *PCSZ;
typedef UCHAR FAR      *PUCHAR;
typedef BYTE FAR       *PBYTE;
typedef SHORT FAR      *PSHORT;
typedef USHORT FAR     *PUSHORT;
typedef BOOL FAR       *PBOOL;
typedef LONG FAR       *PLONG;
typedef ULONG FAR      *PULONG;
typedef SEL FAR        *PSEL;
typedef VOID FAR       *PVOID;
typedef PVOID FAR      *PPVOID;
typedef PSZ FAR        *PPSZ;

typedef int (APIENTRY *PFN)(void);
typedef PFN FAR        *PPFN;

typedef USHORT HFILE;
typedef USHORT FAR *PHFILE;
typedef USHORT HMODULE;
typedef USHORT FAR *PHMODULE;
typedef USHORT PID;
typedef USHORT FAR *PPID;
typedef USHORT TID;
typedef USHORT FAR *PTID;
typedef USHORT HDIR;
typedef USHORT FAR *PHDIR;
typedef USHORT HVIO;
typedef USHORT FAR *PHVIO;
typedef USHORT HKBD;
typedef USHORT FAR *PHKBD;
typedef USHORT HMOU;
typedef USHORT FAR *PHMOU;
typedef USHORT HQUEUE;
typedef USHORT FAR *PHQUEUE;
typedef USHORT HPIPE;
typedef USHORT FAR *PHPIPE;
typedef ULONG           HSYSSEM;
typedef VOID FAR       *HSEM;
typedef HSEM FAR       *PHSEM;

/* Far pointers from a selector and an offset, and back. */
#define MAKEP(sel, off)   ((PVOID)(((ULONG)(USHORT)(sel) << 16) | (USHORT)(off)))
#define SELECTOROF(p)     ((USHORT)((ULONG)(PVOID)(p) >> 16))
#define OFFSETOF(p)       ((USHORT)(ULONG)(PVOID)(p))
#define MAKETYPE(v, type) (*((type FAR *)&v))

#define MAKEULONG(l, h)   ((ULONG)(((USHORT)(l)) | ((ULONG)((USHORT)(h))) << 16))
#define MAKELONG(l, h)    ((LONG)MAKEULONG(l, h))
#define MAKEUSHORT(l, h)  (((USHORT)(l)) | ((USHORT)(h)) << 8)
#define MAKESHORT(l, h)   ((SHORT)MAKEUSHORT(l, h))
#define LOBYTE(w)         ((BYTE)(w))
#define HIBYTE(w)         ((BYTE)((USHORT)(w) >> 8))
#define LOUCHAR(w)        ((UCHAR)(w))
#define HIUCHAR(w)        ((UCHAR)((USHORT)(w) >> 8))
#define LOUSHORT(l)       ((USHORT)(ULONG)(l))
#define HIUSHORT(l)       ((USHORT)((ULONG)(l) >> 16))

/* The Presentation Manager (OS/2 1.1 and later). */
typedef LHANDLE HAB, FAR *PHAB;
typedef LHANDLE HMQ, FAR *PHMQ;
typedef LHANDLE HWND, FAR *PHWND;
typedef LHANDLE HPS, FAR *PHPS;
typedef LHANDLE HDC, FAR *PHDC;
typedef LHANDLE HPOINTER, HBITMAP, HMF, HRGN, HACCEL, HSWITCH, HHEAP, HATOMTBL;
typedef VOID FAR *MPARAM;
typedef MPARAM FAR *PMPARAM;
typedef VOID FAR *MRESULT;
typedef MRESULT FAR *PMRESULT;
typedef LONG COLOR, FAR *PCOLOR;
typedef ULONG ERRORID;
#define NULLHANDLE ((LHANDLE)0)
typedef USHORT ATOM;

/* Message parameters and results, which are 32 bits. */
#define MPVOID               ((MPARAM)0L)
#define MPFROMP(p)           ((MPARAM)(VOID FAR *)(p))
#define MPFROMHWND(hwnd)     ((MPARAM)(HWND)(hwnd))
#define MPFROMCHAR(ch)       ((MPARAM)(ULONG)(USHORT)(ch))
#define MPFROMSHORT(s)       ((MPARAM)(ULONG)(USHORT)(s))
#define MPFROM2SHORT(s1, s2) ((MPARAM)MAKEULONG(s1, s2))
#define MPFROMSH2CH(s, uch1, uch2) ((MPARAM)MAKEULONG(s, MAKEUSHORT(uch1, uch2)))
#define MPFROMLONG(l)        ((MPARAM)(ULONG)(l))
#define PVOIDFROMMP(mp)      ((VOID FAR *)(mp))
#define HWNDFROMMP(mp)       ((HWND)(mp))
#define CHAR1FROMMP(mp)      ((UCHAR)(ULONG)(mp))
#define CHAR2FROMMP(mp)      ((UCHAR)((ULONG)(mp) >> 8))
#define CHAR3FROMMP(mp)      ((UCHAR)((ULONG)(mp) >> 16))
#define CHAR4FROMMP(mp)      ((UCHAR)((ULONG)(mp) >> 24))
#define SHORT1FROMMP(mp)     ((USHORT)(ULONG)(mp))
#define SHORT2FROMMP(mp)     ((USHORT)((ULONG)(mp) >> 16))
#define LONGFROMMP(mp)       ((ULONG)(mp))
#define MRFROMP(p)           ((MRESULT)(VOID FAR *)(p))
#define MRFROMSHORT(s)       ((MRESULT)(ULONG)(USHORT)(s))
#define MRFROM2SHORT(s1, s2) ((MRESULT)MAKEULONG(s1, s2))
#define MRFROMLONG(l)        ((MRESULT)(ULONG)(l))
#define PVOIDFROMMR(mr)      ((VOID FAR *)(mr))
#define LONGFROMMR(mr)       ((ULONG)(mr))
#define SHORT1FROMMR(mr)     ((USHORT)((ULONG)(mr)))
#define SHORT2FROMMR(mr)     ((USHORT)((ULONG)(mr) >> 16))

typedef struct _POINTL {
    LONG x, y;
} POINTL, FAR *PPOINTL;
typedef struct _POINTS {
    SHORT x, y;
} POINTS, FAR *PPOINTS;
typedef struct _RECTL {
    LONG xLeft, yBottom, xRight, yTop;
} RECTL, FAR *PRECTL;
typedef struct _SIZEL {
    LONG cx, cy;
} SIZEL, FAR *PSIZEL;
typedef CHAR STR8[8];
typedef STR8 FAR *PSTR8;

typedef struct _RESULTCODES {
    USHORT codeTerminate;
    USHORT codeResult;
} RESULTCODES, FAR *PRESULTCODES;

#endif /* __OS2DEF_H */
