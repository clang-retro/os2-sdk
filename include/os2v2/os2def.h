/*
 * os2def.h - basic types and macros of the 32-bit OS/2 API (2.0 and
 * later), for clang-retro (i386-pc-os2). Written from the OS/2 2.x
 * programming documentation; see the SDK's README.
 *
 * The system calls use the _System convention: arguments on the stack
 * right to left, the caller removes them, the result in EAX. That is the
 * C convention, so APIENTRY and _System are empty.
 */
#ifndef __OS2DEF_H
#define __OS2DEF_H

#define _System
#define APIENTRY
#define EXPENTRY
#define APIRET ULONG
#define CDECL
#define PASCAL
#define FAR
#define NEAR

#define VOID void
#ifndef NULL
#define NULL ((void *)0)
#endif
#define FALSE 0
#define TRUE  1
#define NULLHANDLE ((LHANDLE)0)

typedef unsigned long  LHANDLE;
typedef unsigned short SHANDLE;
typedef unsigned char  UCHAR, BYTE;
typedef char           CHAR;
typedef short          SHORT;
typedef unsigned short USHORT;
typedef long           LONG;
typedef unsigned long  ULONG, BOOL32;
typedef int            INT;
typedef unsigned int   UINT;
typedef unsigned long  BOOL;
typedef unsigned short SEL;

typedef CHAR          *PCHAR, *PCH, *PSZ;
typedef const CHAR    *PCSZ;
typedef UCHAR         *PUCHAR;
typedef BYTE          *PBYTE;
typedef SHORT         *PSHORT;
typedef USHORT        *PUSHORT;
typedef LONG          *PLONG;
typedef ULONG         *PULONG;
typedef BOOL          *PBOOL;
typedef BOOL32        *PBOOL32;
typedef VOID          *PVOID;
typedef PVOID         *PPVOID;
typedef PSZ           *PPSZ;
typedef APIRET        *PAPIRET;

typedef int (*PFN)(void);
typedef PFN           *PPFN;

typedef LHANDLE        HFILE, *PHFILE;
typedef LHANDLE        HMODULE, *PHMODULE;
typedef LHANDLE        PID, *PPID;
typedef LHANDLE        TID, *PTID;
typedef LHANDLE        HDIR, *PHDIR;
typedef LHANDLE        HEV, *PHEV;
typedef LHANDLE        HMTX, *PHMTX;
typedef LHANDLE        HMUX, *PHMUX;
typedef LHANDLE        HPIPE, *PHPIPE;
typedef LHANDLE        HQUEUE, *PHQUEUE;
typedef LHANDLE        HAB, *PHAB;
typedef LHANDLE        HMQ, *PHMQ;
typedef LHANDLE        HWND, *PHWND;
typedef LHANDLE        HPS, *PHPS;
typedef LHANDLE        HDC, *PHDC;
typedef LHANDLE        HPOINTER, HBITMAP, HMF, HRGN, HACCEL, HSWITCH;
typedef USHORT         HVIO, *PHVIO, HKBD, *PHKBD, HMOU, *PHMOU;
typedef VOID          *MPARAM, **PMPARAM;
typedef VOID          *MRESULT, **PMRESULT;
typedef LONG           COLOR, *PCOLOR;
typedef ULONG          ERRORID;

#define MAKEULONG(l, h)  ((ULONG)(((USHORT)(l)) | ((ULONG)((USHORT)(h))) << 16))
#define MAKELONG(l, h)   ((LONG)MAKEULONG(l, h))
#define MAKEUSHORT(l, h) (((USHORT)(l)) | ((USHORT)(h)) << 8)
#define MAKESHORT(l, h)  ((SHORT)MAKEUSHORT(l, h))
#define LOBYTE(w)        ((BYTE)(w))
#define HIBYTE(w)        ((BYTE)((USHORT)(w) >> 8))
#define LOUCHAR(w)       ((UCHAR)(w))
#define HIUCHAR(w)       ((UCHAR)((USHORT)(w) >> 8))
#define LOUSHORT(l)      ((USHORT)(ULONG)(l))
#define HIUSHORT(l)      ((USHORT)((ULONG)(l) >> 16))

/* Message parameters and results. */
#define MPVOID              ((MPARAM)0L)
#define MPFROMP(p)          ((MPARAM)(VOID *)(p))
#define MPFROMHWND(hwnd)    ((MPARAM)(HWND)(hwnd))
#define MPFROMCHAR(ch)      ((MPARAM)(USHORT)(ch))
#define MPFROMSHORT(s)      ((MPARAM)(USHORT)(s))
#define MPFROM2SHORT(s1, s2) ((MPARAM)MAKELONG(s1, s2))
#define MPFROMLONG(l)       ((MPARAM)(ULONG)(l))
#define PVOIDFROMMP(mp)     ((VOID *)(mp))
#define HWNDFROMMP(mp)      ((HWND)(mp))
#define CHAR1FROMMP(mp)     ((UCHAR)(ULONG)(mp))
#define CHAR2FROMMP(mp)     ((UCHAR)((ULONG)(mp) >> 8))
#define CHAR3FROMMP(mp)     ((UCHAR)((ULONG)(mp) >> 16))
#define CHAR4FROMMP(mp)     ((UCHAR)((ULONG)(mp) >> 24))
#define SHORT1FROMMP(mp)    ((USHORT)(ULONG)(mp))
#define SHORT2FROMMP(mp)    ((USHORT)((ULONG)(mp) >> 16))
#define LONGFROMMP(mp)      ((ULONG)(mp))
#define MRFROMP(p)          ((MRESULT)(VOID *)(p))
#define MRFROMSHORT(s)      ((MRESULT)(USHORT)(s))
#define MRFROM2SHORT(s1, s2) ((MRESULT)MAKELONG(s1, s2))
#define MRFROMLONG(l)       ((MRESULT)(ULONG)(l))
#define LONGFROMMR(mr)      ((ULONG)(mr))
#define SHORT1FROMMR(mr)    ((USHORT)((ULONG)(mr)))
#define SHORT2FROMMR(mr)    ((USHORT)((ULONG)(mr) >> 16))

typedef struct _POINTL {
    LONG x, y;
} POINTL, *PPOINTL;
typedef struct _RECTL {
    LONG xLeft, yBottom, xRight, yTop;
} RECTL, *PRECTL;
typedef struct _SIZEL {
    LONG cx, cy;
} SIZEL, *PSIZEL;

typedef struct _RESULTCODES {
    ULONG codeTerminate;
    ULONG codeResult;
} RESULTCODES, *PRESULTCODES;

#endif /* __OS2DEF_H */
