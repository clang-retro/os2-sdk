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

typedef struct _POINTL {
    LONG x, y;
} POINTL, FAR *PPOINTL;
typedef struct _RECTL {
    LONG xLeft, yBottom, xRight, yTop;
} RECTL, FAR *PRECTL;

typedef struct _RESULTCODES {
    USHORT codeTerminate;
    USHORT codeResult;
} RESULTCODES, FAR *PRESULTCODES;

#endif /* __OS2DEF_H */
