/*
 * bsesub.h - the video (VIOCALLS), keyboard (KBDCALLS) and mouse
 * (MOUCALLS) API for 32-bit programs.
 *
 * Copyright (C) clang-retro contributors.
 * SPDX-License-Identifier: AGPL-3.0-only
 *
 * These are 16-bit calls on every OS/2. Each function here is an inline
 * that calls the 16-bit one through __os2_call16 and a 16-bit stub, both
 * in libos2.a: it runs on a 16-bit stack of the thread's own, and its
 * pointers become 16:16 ones. A buffer that crosses a 64 KiB boundary is
 * copied (8 KiB at most, in all, for one call). The thunk needs the
 * program's memory to be below 512 MiB, which it is unless it asks for
 * high memory (OBJ_ANY). Written by tools/gen-bsesub.py.
 */
#ifndef __BSESUB_H
#define __BSESUB_H

#include <os2def.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The 16-bit structures, as they are in memory. */
#pragma pack(push, 2)

/* Video */
typedef struct _VIOMODEINFO {
    USHORT cb;
    UCHAR  fbType;
    UCHAR  color;
    USHORT col;
    USHORT row;
    USHORT hres;
    USHORT vres;
    UCHAR  fmt_ID;
    UCHAR  attrib;
} VIOMODEINFO, *PVIOMODEINFO;
#define VGMT_OTHER        0x01
#define VGMT_GRAPHICS     0x02
#define VGMT_DISABLEBURST 0x04
typedef struct _VIOCURSORINFO {
    USHORT yStart;
    USHORT cEnd;
    USHORT cx;
    USHORT attr; /* 0xffff hides the cursor */
} VIOCURSORINFO, *PVIOCURSORINFO;

/* Keyboard */
typedef struct _KBDKEYINFO {
    UCHAR  chChar;
    UCHAR  chScan;
    UCHAR  fbStatus;
    UCHAR  bNlsShift;
    USHORT fsState;
    ULONG  time;
} KBDKEYINFO, *PKBDKEYINFO;
typedef struct _STRINGINBUF {
    USHORT cb;
    USHORT cchIn;
} STRINGINBUF, *PSTRINGINBUF;
typedef struct _KBDINFO {
    USHORT cb;
    USHORT fsMask;
    USHORT chTurnAround;
    USHORT fsInterim;
    USHORT fsState;
} KBDINFO, *PKBDINFO;
#define IO_WAIT   0
#define IO_NOWAIT 1
#define KEYBOARD_ECHO_ON     0x0001
#define KEYBOARD_ECHO_OFF    0x0002
#define KEYBOARD_BINARY_MODE 0x0004
#define KEYBOARD_ASCII_MODE  0x0008
#define KBDTRF_FINAL_CHAR_IN 0x40

/* Mouse */
typedef struct _MOUEVENTINFO {
    USHORT fs;
    ULONG  time;
    SHORT  row;
    SHORT  col;
} MOUEVENTINFO, *PMOUEVENTINFO;
typedef struct _MOUQUEINFO {
    USHORT cEvents;
    USHORT cmaxEvents;
} MOUQUEINFO, *PMOUQUEINFO;
typedef struct _PTRLOC {
    USHORT row;
    USHORT col;
} PTRLOC, *PPTRLOC;
typedef struct _NOPTRRECT {
    USHORT row, col, cRow, cCol;
} NOPTRRECT, *PNOPTRRECT;
#define MOU_NOWAIT 0
#define MOU_WAIT   1
#define MOUSE_MOTION              0x0001
#define MOUSE_MOTION_WITH_BN1_DOWN 0x0002
#define MOUSE_BN1_DOWN            0x0004
#define MOUSE_MOTION_WITH_BN2_DOWN 0x0008
#define MOUSE_BN2_DOWN            0x0010
#define MOUSE_MOTION_WITH_BN3_DOWN 0x0020
#define MOUSE_BN3_DOWN            0x0040

#pragma pack(pop)

/* The thunk (lib/call16.c, in libos2.a). */
struct __os2_buf16 {
    unsigned short argoff;
    unsigned short size;
    unsigned char  out;
};
unsigned long __os2_ptr16(const void *p);
unsigned __os2_call16(const void *stub, void *args, unsigned nargs,
                      const struct __os2_buf16 *bufs, unsigned nbufs);

static __inline__ unsigned short __os2_strsize(const char *s)
{
    unsigned short n = 1;
    while (*s++)
        n++;
    return n;
}

/* Video */
static __inline__ USHORT VioWrtTTY(PCCH pch, USHORT cb, HVIO hvio)
{
    extern const char __os2_stub_VIO16WRTTTY[];
    struct { USHORT hvio; USHORT cb; ULONG pch; } __attribute__((packed)) a = { hvio, cb, (ULONG)pch };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pch), cb, 0 } };
    return (USHORT)__os2_call16(__os2_stub_VIO16WRTTTY, &a, sizeof a, b, 1);
}
static __inline__ USHORT VioGetMode(PVIOMODEINFO pvioModeInfo, HVIO hvio)
{
    extern const char __os2_stub_VIO16GETMODE[];
    struct { USHORT hvio; ULONG pvioModeInfo; } __attribute__((packed)) a = { hvio, (ULONG)pvioModeInfo };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pvioModeInfo), sizeof(VIOMODEINFO), 1 } };
    return (USHORT)__os2_call16(__os2_stub_VIO16GETMODE, &a, sizeof a, b, 1);
}
static __inline__ USHORT VioSetMode(PVIOMODEINFO pvioModeInfo, HVIO hvio)
{
    extern const char __os2_stub_VIO16SETMODE[];
    struct { USHORT hvio; ULONG pvioModeInfo; } __attribute__((packed)) a = { hvio, (ULONG)pvioModeInfo };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pvioModeInfo), sizeof(VIOMODEINFO), 0 } };
    return (USHORT)__os2_call16(__os2_stub_VIO16SETMODE, &a, sizeof a, b, 1);
}
static __inline__ USHORT VioGetCurPos(PUSHORT pusRow, PUSHORT pusColumn, HVIO hvio)
{
    extern const char __os2_stub_VIO16GETCURPOS[];
    struct { USHORT hvio; ULONG pusColumn; ULONG pusRow; } __attribute__((packed)) a = { hvio, (ULONG)pusColumn, (ULONG)pusRow };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pusColumn), 2, 1 }, { __builtin_offsetof(__typeof__(a), pusRow), 2, 1 } };
    return (USHORT)__os2_call16(__os2_stub_VIO16GETCURPOS, &a, sizeof a, b, 2);
}
static __inline__ USHORT VioSetCurPos(USHORT usRow, USHORT usColumn, HVIO hvio)
{
    extern const char __os2_stub_VIO16SETCURPOS[];
    struct { USHORT hvio; USHORT usColumn; USHORT usRow; } __attribute__((packed)) a = { hvio, usColumn, usRow };
    return (USHORT)__os2_call16(__os2_stub_VIO16SETCURPOS, &a, sizeof a, 0, 0);
}
static __inline__ USHORT VioGetCurType(PVIOCURSORINFO pvioCursorInfo, HVIO hvio)
{
    extern const char __os2_stub_VIO16GETCURTYPE[];
    struct { USHORT hvio; ULONG pvioCursorInfo; } __attribute__((packed)) a = { hvio, (ULONG)pvioCursorInfo };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pvioCursorInfo), sizeof(VIOCURSORINFO), 1 } };
    return (USHORT)__os2_call16(__os2_stub_VIO16GETCURTYPE, &a, sizeof a, b, 1);
}
static __inline__ USHORT VioSetCurType(PVIOCURSORINFO pvioCursorInfo, HVIO hvio)
{
    extern const char __os2_stub_VIO16SETCURTYPE[];
    struct { USHORT hvio; ULONG pvioCursorInfo; } __attribute__((packed)) a = { hvio, (ULONG)pvioCursorInfo };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pvioCursorInfo), sizeof(VIOCURSORINFO), 0 } };
    return (USHORT)__os2_call16(__os2_stub_VIO16SETCURTYPE, &a, sizeof a, b, 1);
}
static __inline__ USHORT VioScrollUp(USHORT usTopRow, USHORT usLeftCol, USHORT usBotRow, USHORT usRightCol, USHORT cbLines, PBYTE pCell, HVIO hvio)
{
    extern const char __os2_stub_VIO16SCROLLUP[];
    struct { USHORT hvio; ULONG pCell; USHORT cbLines; USHORT usRightCol; USHORT usBotRow; USHORT usLeftCol; USHORT usTopRow; } __attribute__((packed)) a = { hvio, (ULONG)pCell, cbLines, usRightCol, usBotRow, usLeftCol, usTopRow };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pCell), 2, 0 } };
    return (USHORT)__os2_call16(__os2_stub_VIO16SCROLLUP, &a, sizeof a, b, 1);
}
static __inline__ USHORT VioScrollDn(USHORT usTopRow, USHORT usLeftCol, USHORT usBotRow, USHORT usRightCol, USHORT cbLines, PBYTE pCell, HVIO hvio)
{
    extern const char __os2_stub_VIO16SCROLLDN[];
    struct { USHORT hvio; ULONG pCell; USHORT cbLines; USHORT usRightCol; USHORT usBotRow; USHORT usLeftCol; USHORT usTopRow; } __attribute__((packed)) a = { hvio, (ULONG)pCell, cbLines, usRightCol, usBotRow, usLeftCol, usTopRow };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pCell), 2, 0 } };
    return (USHORT)__os2_call16(__os2_stub_VIO16SCROLLDN, &a, sizeof a, b, 1);
}
static __inline__ USHORT VioScrollLf(USHORT usTopRow, USHORT usLeftCol, USHORT usBotRow, USHORT usRightCol, USHORT cbLines, PBYTE pCell, HVIO hvio)
{
    extern const char __os2_stub_VIO16SCROLLLF[];
    struct { USHORT hvio; ULONG pCell; USHORT cbLines; USHORT usRightCol; USHORT usBotRow; USHORT usLeftCol; USHORT usTopRow; } __attribute__((packed)) a = { hvio, (ULONG)pCell, cbLines, usRightCol, usBotRow, usLeftCol, usTopRow };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pCell), 2, 0 } };
    return (USHORT)__os2_call16(__os2_stub_VIO16SCROLLLF, &a, sizeof a, b, 1);
}
static __inline__ USHORT VioScrollRt(USHORT usTopRow, USHORT usLeftCol, USHORT usBotRow, USHORT usRightCol, USHORT cbLines, PBYTE pCell, HVIO hvio)
{
    extern const char __os2_stub_VIO16SCROLLRT[];
    struct { USHORT hvio; ULONG pCell; USHORT cbLines; USHORT usRightCol; USHORT usBotRow; USHORT usLeftCol; USHORT usTopRow; } __attribute__((packed)) a = { hvio, (ULONG)pCell, cbLines, usRightCol, usBotRow, usLeftCol, usTopRow };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pCell), 2, 0 } };
    return (USHORT)__os2_call16(__os2_stub_VIO16SCROLLRT, &a, sizeof a, b, 1);
}
static __inline__ USHORT VioWrtCharStr(PCCH pchStr, USHORT cbStr, USHORT usRow, USHORT usColumn, HVIO hvio)
{
    extern const char __os2_stub_VIO16WRTCHARSTR[];
    struct { USHORT hvio; USHORT usColumn; USHORT usRow; USHORT cbStr; ULONG pchStr; } __attribute__((packed)) a = { hvio, usColumn, usRow, cbStr, (ULONG)pchStr };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pchStr), cbStr, 0 } };
    return (USHORT)__os2_call16(__os2_stub_VIO16WRTCHARSTR, &a, sizeof a, b, 1);
}
static __inline__ USHORT VioWrtCharStrAtt(PCCH pch, USHORT cb, USHORT usRow, USHORT usColumn, PBYTE pAttr, HVIO hvio)
{
    extern const char __os2_stub_VIO16WRTCHARSTRATT[];
    struct { USHORT hvio; ULONG pAttr; USHORT usColumn; USHORT usRow; USHORT cb; ULONG pch; } __attribute__((packed)) a = { hvio, (ULONG)pAttr, usColumn, usRow, cb, (ULONG)pch };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pAttr), 1, 0 }, { __builtin_offsetof(__typeof__(a), pch), cb, 0 } };
    return (USHORT)__os2_call16(__os2_stub_VIO16WRTCHARSTRATT, &a, sizeof a, b, 2);
}
static __inline__ USHORT VioWrtCellStr(PCCH pchCellStr, USHORT cb, USHORT usRow, USHORT usColumn, HVIO hvio)
{
    extern const char __os2_stub_VIO16WRTCELLSTR[];
    struct { USHORT hvio; USHORT usColumn; USHORT usRow; USHORT cb; ULONG pchCellStr; } __attribute__((packed)) a = { hvio, usColumn, usRow, cb, (ULONG)pchCellStr };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pchCellStr), cb, 0 } };
    return (USHORT)__os2_call16(__os2_stub_VIO16WRTCELLSTR, &a, sizeof a, b, 1);
}
static __inline__ USHORT VioWrtNCell(PBYTE pCell, USHORT usTimes, USHORT usRow, USHORT usColumn, HVIO hvio)
{
    extern const char __os2_stub_VIO16WRTNCELL[];
    struct { USHORT hvio; USHORT usColumn; USHORT usRow; USHORT usTimes; ULONG pCell; } __attribute__((packed)) a = { hvio, usColumn, usRow, usTimes, (ULONG)pCell };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pCell), 2, 0 } };
    return (USHORT)__os2_call16(__os2_stub_VIO16WRTNCELL, &a, sizeof a, b, 1);
}
static __inline__ USHORT VioWrtNAttr(PBYTE pAttr, USHORT usTimes, USHORT usRow, USHORT usColumn, HVIO hvio)
{
    extern const char __os2_stub_VIO16WRTNATTR[];
    struct { USHORT hvio; USHORT usColumn; USHORT usRow; USHORT usTimes; ULONG pAttr; } __attribute__((packed)) a = { hvio, usColumn, usRow, usTimes, (ULONG)pAttr };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pAttr), 1, 0 } };
    return (USHORT)__os2_call16(__os2_stub_VIO16WRTNATTR, &a, sizeof a, b, 1);
}
static __inline__ USHORT VioWrtNChar(PCCH pchChar, USHORT usTimes, USHORT usRow, USHORT usColumn, HVIO hvio)
{
    extern const char __os2_stub_VIO16WRTNCHAR[];
    struct { USHORT hvio; USHORT usColumn; USHORT usRow; USHORT usTimes; ULONG pchChar; } __attribute__((packed)) a = { hvio, usColumn, usRow, usTimes, (ULONG)pchChar };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pchChar), 1, 0 } };
    return (USHORT)__os2_call16(__os2_stub_VIO16WRTNCHAR, &a, sizeof a, b, 1);
}
static __inline__ USHORT VioReadCellStr(PCH pchCellStr, PUSHORT pcb, USHORT usRow, USHORT usColumn, HVIO hvio)
{
    extern const char __os2_stub_VIO16READCELLSTR[];
    struct { USHORT hvio; USHORT usColumn; USHORT usRow; ULONG pcb; ULONG pchCellStr; } __attribute__((packed)) a = { hvio, usColumn, usRow, (ULONG)pcb, (ULONG)pchCellStr };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pcb), 2, 1 }, { __builtin_offsetof(__typeof__(a), pchCellStr), (pcb ? *pcb : 0), 1 } };
    return (USHORT)__os2_call16(__os2_stub_VIO16READCELLSTR, &a, sizeof a, b, 2);
}
static __inline__ USHORT VioReadCharStr(PCH pch, PUSHORT pcb, USHORT usRow, USHORT usColumn, HVIO hvio)
{
    extern const char __os2_stub_VIO16READCHARSTR[];
    struct { USHORT hvio; USHORT usColumn; USHORT usRow; ULONG pcb; ULONG pch; } __attribute__((packed)) a = { hvio, usColumn, usRow, (ULONG)pcb, (ULONG)pch };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pcb), 2, 1 }, { __builtin_offsetof(__typeof__(a), pch), (pcb ? *pcb : 0), 1 } };
    return (USHORT)__os2_call16(__os2_stub_VIO16READCHARSTR, &a, sizeof a, b, 2);
}
static __inline__ USHORT VioGetCp(USHORT usReserved, PUSHORT pIdCodePage, HVIO hvio)
{
    extern const char __os2_stub_VIO16GETCP[];
    struct { USHORT hvio; ULONG pIdCodePage; USHORT usReserved; } __attribute__((packed)) a = { hvio, (ULONG)pIdCodePage, usReserved };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pIdCodePage), 2, 1 } };
    return (USHORT)__os2_call16(__os2_stub_VIO16GETCP, &a, sizeof a, b, 1);
}
static __inline__ USHORT VioSetCp(USHORT usReserved, USHORT idCodePage, HVIO hvio)
{
    extern const char __os2_stub_VIO16SETCP[];
    struct { USHORT hvio; USHORT idCodePage; USHORT usReserved; } __attribute__((packed)) a = { hvio, idCodePage, usReserved };
    return (USHORT)__os2_call16(__os2_stub_VIO16SETCP, &a, sizeof a, 0, 0);
}
static __inline__ USHORT VioGetAnsi(PUSHORT pfAnsi, HVIO hvio)
{
    extern const char __os2_stub_VIO16GETANSI[];
    struct { USHORT hvio; ULONG pfAnsi; } __attribute__((packed)) a = { hvio, (ULONG)pfAnsi };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pfAnsi), 2, 1 } };
    return (USHORT)__os2_call16(__os2_stub_VIO16GETANSI, &a, sizeof a, b, 1);
}
static __inline__ USHORT VioSetAnsi(USHORT fAnsi, HVIO hvio)
{
    extern const char __os2_stub_VIO16SETANSI[];
    struct { USHORT hvio; USHORT fAnsi; } __attribute__((packed)) a = { hvio, fAnsi };
    return (USHORT)__os2_call16(__os2_stub_VIO16SETANSI, &a, sizeof a, 0, 0);
}

/* Keyboard */
static __inline__ USHORT KbdCharIn(PKBDKEYINFO pkbci, USHORT fWait, HKBD hkbd)
{
    extern const char __os2_stub_KBD16CHARIN[];
    struct { USHORT hkbd; USHORT fWait; ULONG pkbci; } __attribute__((packed)) a = { hkbd, fWait, (ULONG)pkbci };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pkbci), sizeof(KBDKEYINFO), 1 } };
    return (USHORT)__os2_call16(__os2_stub_KBD16CHARIN, &a, sizeof a, b, 1);
}
static __inline__ USHORT KbdPeek(PKBDKEYINFO pkbci, HKBD hkbd)
{
    extern const char __os2_stub_KBD16PEEK[];
    struct { USHORT hkbd; ULONG pkbci; } __attribute__((packed)) a = { hkbd, (ULONG)pkbci };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pkbci), sizeof(KBDKEYINFO), 1 } };
    return (USHORT)__os2_call16(__os2_stub_KBD16PEEK, &a, sizeof a, b, 1);
}
static __inline__ USHORT KbdStringIn(PCH pch, PSTRINGINBUF pchIn, USHORT fsWait, HKBD hkbd)
{
    extern const char __os2_stub_KBD16STRINGIN[];
    struct { USHORT hkbd; USHORT fsWait; ULONG pchIn; ULONG pch; } __attribute__((packed)) a = { hkbd, fsWait, (ULONG)pchIn, (ULONG)pch };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pchIn), sizeof(STRINGINBUF), 1 }, { __builtin_offsetof(__typeof__(a), pch), (pchIn ? pchIn->cb : 0), 1 } };
    return (USHORT)__os2_call16(__os2_stub_KBD16STRINGIN, &a, sizeof a, b, 2);
}
static __inline__ USHORT KbdFlushBuffer(HKBD hkbd)
{
    extern const char __os2_stub_KBD16FLUSHBUFFER[];
    struct { USHORT hkbd; } __attribute__((packed)) a = { hkbd };
    return (USHORT)__os2_call16(__os2_stub_KBD16FLUSHBUFFER, &a, sizeof a, 0, 0);
}
static __inline__ USHORT KbdGetStatus(PKBDINFO pkbdinfo, HKBD hkbd)
{
    extern const char __os2_stub_KBD16GETSTATUS[];
    struct { USHORT hkbd; ULONG pkbdinfo; } __attribute__((packed)) a = { hkbd, (ULONG)pkbdinfo };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pkbdinfo), sizeof(KBDINFO), 1 } };
    return (USHORT)__os2_call16(__os2_stub_KBD16GETSTATUS, &a, sizeof a, b, 1);
}
static __inline__ USHORT KbdSetStatus(PKBDINFO pkbdinfo, HKBD hkbd)
{
    extern const char __os2_stub_KBD16SETSTATUS[];
    struct { USHORT hkbd; ULONG pkbdinfo; } __attribute__((packed)) a = { hkbd, (ULONG)pkbdinfo };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pkbdinfo), sizeof(KBDINFO), 0 } };
    return (USHORT)__os2_call16(__os2_stub_KBD16SETSTATUS, &a, sizeof a, b, 1);
}

/* Mouse */
static __inline__ USHORT MouOpen(PCSZ pszDvrName, PHMOU phmou)
{
    extern const char __os2_stub_MOU16OPEN[];
    struct { ULONG phmou; ULONG pszDvrName; } __attribute__((packed)) a = { (ULONG)phmou, (ULONG)pszDvrName };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), phmou), 2, 1 }, { __builtin_offsetof(__typeof__(a), pszDvrName), (pszDvrName ? __os2_strsize(pszDvrName) : 0), 0 } };
    return (USHORT)__os2_call16(__os2_stub_MOU16OPEN, &a, sizeof a, b, 2);
}
static __inline__ USHORT MouClose(HMOU hmou)
{
    extern const char __os2_stub_MOU16CLOSE[];
    struct { USHORT hmou; } __attribute__((packed)) a = { hmou };
    return (USHORT)__os2_call16(__os2_stub_MOU16CLOSE, &a, sizeof a, 0, 0);
}
static __inline__ USHORT MouDrawPtr(HMOU hmou)
{
    extern const char __os2_stub_MOU16DRAWPTR[];
    struct { USHORT hmou; } __attribute__((packed)) a = { hmou };
    return (USHORT)__os2_call16(__os2_stub_MOU16DRAWPTR, &a, sizeof a, 0, 0);
}
static __inline__ USHORT MouRemovePtr(PNOPTRRECT pmourtRect, HMOU hmou)
{
    extern const char __os2_stub_MOU16REMOVEPTR[];
    struct { USHORT hmou; ULONG pmourtRect; } __attribute__((packed)) a = { hmou, (ULONG)pmourtRect };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pmourtRect), sizeof(NOPTRRECT), 0 } };
    return (USHORT)__os2_call16(__os2_stub_MOU16REMOVEPTR, &a, sizeof a, b, 1);
}
static __inline__ USHORT MouReadEventQue(PMOUEVENTINFO pmouevEvent, PUSHORT pfWait, HMOU hmou)
{
    extern const char __os2_stub_MOU16READEVENTQUE[];
    struct { USHORT hmou; ULONG pfWait; ULONG pmouevEvent; } __attribute__((packed)) a = { hmou, (ULONG)pfWait, (ULONG)pmouevEvent };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pfWait), 2, 0 }, { __builtin_offsetof(__typeof__(a), pmouevEvent), sizeof(MOUEVENTINFO), 1 } };
    return (USHORT)__os2_call16(__os2_stub_MOU16READEVENTQUE, &a, sizeof a, b, 2);
}
static __inline__ USHORT MouGetNumQueEl(PMOUQUEINFO pmouqi, HMOU hmou)
{
    extern const char __os2_stub_MOU16GETNUMQUEEL[];
    struct { USHORT hmou; ULONG pmouqi; } __attribute__((packed)) a = { hmou, (ULONG)pmouqi };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pmouqi), sizeof(MOUQUEINFO), 1 } };
    return (USHORT)__os2_call16(__os2_stub_MOU16GETNUMQUEEL, &a, sizeof a, b, 1);
}
static __inline__ USHORT MouGetEventMask(PUSHORT pfsEvents, HMOU hmou)
{
    extern const char __os2_stub_MOU16GETEVENTMASK[];
    struct { USHORT hmou; ULONG pfsEvents; } __attribute__((packed)) a = { hmou, (ULONG)pfsEvents };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pfsEvents), 2, 1 } };
    return (USHORT)__os2_call16(__os2_stub_MOU16GETEVENTMASK, &a, sizeof a, b, 1);
}
static __inline__ USHORT MouSetEventMask(PUSHORT pfsEvents, HMOU hmou)
{
    extern const char __os2_stub_MOU16SETEVENTMASK[];
    struct { USHORT hmou; ULONG pfsEvents; } __attribute__((packed)) a = { hmou, (ULONG)pfsEvents };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pfsEvents), 2, 0 } };
    return (USHORT)__os2_call16(__os2_stub_MOU16SETEVENTMASK, &a, sizeof a, b, 1);
}
static __inline__ USHORT MouGetPtrPos(PPTRLOC pmouLoc, HMOU hmou)
{
    extern const char __os2_stub_MOU16GETPTRPOS[];
    struct { USHORT hmou; ULONG pmouLoc; } __attribute__((packed)) a = { hmou, (ULONG)pmouLoc };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pmouLoc), sizeof(PTRLOC), 1 } };
    return (USHORT)__os2_call16(__os2_stub_MOU16GETPTRPOS, &a, sizeof a, b, 1);
}
static __inline__ USHORT MouSetPtrPos(PPTRLOC pmouLoc, HMOU hmou)
{
    extern const char __os2_stub_MOU16SETPTRPOS[];
    struct { USHORT hmou; ULONG pmouLoc; } __attribute__((packed)) a = { hmou, (ULONG)pmouLoc };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pmouLoc), sizeof(PTRLOC), 0 } };
    return (USHORT)__os2_call16(__os2_stub_MOU16SETPTRPOS, &a, sizeof a, b, 1);
}
static __inline__ USHORT MouGetNumButtons(PUSHORT pcButtons, HMOU hmou)
{
    extern const char __os2_stub_MOU16GETNUMBUTTONS[];
    struct { USHORT hmou; ULONG pcButtons; } __attribute__((packed)) a = { hmou, (ULONG)pcButtons };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pcButtons), 2, 1 } };
    return (USHORT)__os2_call16(__os2_stub_MOU16GETNUMBUTTONS, &a, sizeof a, b, 1);
}
static __inline__ USHORT MouFlushQue(HMOU hmou)
{
    extern const char __os2_stub_MOU16FLUSHQUE[];
    struct { USHORT hmou; } __attribute__((packed)) a = { hmou };
    return (USHORT)__os2_call16(__os2_stub_MOU16FLUSHQUE, &a, sizeof a, 0, 0);
}
static __inline__ USHORT MouGetDevStatus(PUSHORT pfsDevStatus, HMOU hmou)
{
    extern const char __os2_stub_MOU16GETDEVSTATUS[];
    struct { USHORT hmou; ULONG pfsDevStatus; } __attribute__((packed)) a = { hmou, (ULONG)pfsDevStatus };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pfsDevStatus), 2, 1 } };
    return (USHORT)__os2_call16(__os2_stub_MOU16GETDEVSTATUS, &a, sizeof a, b, 1);
}
static __inline__ USHORT MouSetDevStatus(PUSHORT pfsDevStatus, HMOU hmou)
{
    extern const char __os2_stub_MOU16SETDEVSTATUS[];
    struct { USHORT hmou; ULONG pfsDevStatus; } __attribute__((packed)) a = { hmou, (ULONG)pfsDevStatus };
    const struct __os2_buf16 b[] = { { __builtin_offsetof(__typeof__(a), pfsDevStatus), 2, 0 } };
    return (USHORT)__os2_call16(__os2_stub_MOU16SETDEVSTATUS, &a, sizeof a, b, 1);
}

#ifdef __cplusplus
}
#endif

#endif /* __BSESUB_H */
