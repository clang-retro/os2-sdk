/*
 * bsesub.h - the OS/2 1.x video (VIOCALLS), keyboard (KBDCALLS) and mouse
 * (MOUCALLS) API, for clang-retro (i8086-pc-os2). OS/2 2.x keeps these as
 * 16-bit calls.
 */
#ifndef __BSESUB_H
#define __BSESUB_H

#include <os2def.h>

#ifdef __cplusplus
extern "C" {
#endif

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
} VIOMODEINFO, FAR *PVIOMODEINFO;
typedef struct _VIOCURSORINFO {
    USHORT yStart;
    USHORT cEnd;
    USHORT cx;
    USHORT attr;
} VIOCURSORINFO, FAR *PVIOCURSORINFO;

USHORT APIENTRY VioWrtTTY(PCH pch, USHORT cb, HVIO hvio);
USHORT APIENTRY VioGetMode(PVIOMODEINFO pvioModeInfo, HVIO hvio);
USHORT APIENTRY VioSetMode(PVIOMODEINFO pvioModeInfo, HVIO hvio);
USHORT APIENTRY VioGetCurPos(PUSHORT pusRow, PUSHORT pusColumn, HVIO hvio);
USHORT APIENTRY VioSetCurPos(USHORT usRow, USHORT usColumn, HVIO hvio);
USHORT APIENTRY VioGetCurType(PVIOCURSORINFO pvioCursorInfo, HVIO hvio);
USHORT APIENTRY VioSetCurType(PVIOCURSORINFO pvioCursorInfo, HVIO hvio);
USHORT APIENTRY VioScrollUp(USHORT usTopRow, USHORT usLeftCol, USHORT usBotRow,
                            USHORT usRightCol, USHORT cbLines, PBYTE pCell, HVIO hvio);
USHORT APIENTRY VioScrollDn(USHORT usTopRow, USHORT usLeftCol, USHORT usBotRow,
                            USHORT usRightCol, USHORT cbLines, PBYTE pCell, HVIO hvio);
USHORT APIENTRY VioWrtCharStr(PCH pchStr, USHORT cbStr, USHORT usRow, USHORT usColumn,
                              HVIO hvio);
USHORT APIENTRY VioWrtCharStrAtt(PCH pch, USHORT cb, USHORT usRow, USHORT usColumn,
                                 PBYTE pAttr, HVIO hvio);
USHORT APIENTRY VioWrtCellStr(PCH pchCellStr, USHORT cb, USHORT usRow, USHORT usColumn,
                              HVIO hvio);
USHORT APIENTRY VioWrtNCell(PBYTE pCell, USHORT usTimes, USHORT usRow, USHORT usColumn,
                            HVIO hvio);
USHORT APIENTRY VioWrtNAttr(PBYTE pAttr, USHORT usTimes, USHORT usRow, USHORT usColumn,
                            HVIO hvio);
USHORT APIENTRY VioWrtNChar(PCH pchChar, USHORT usTimes, USHORT usRow, USHORT usColumn,
                            HVIO hvio);
USHORT APIENTRY VioReadCellStr(PCH pchCellStr, PUSHORT pcb, USHORT usRow, USHORT usColumn,
                               HVIO hvio);
USHORT APIENTRY VioReadCharStr(PCH pch, PUSHORT pcb, USHORT usRow, USHORT usColumn,
                               HVIO hvio);

/* Keyboard */
typedef struct _KBDKEYINFO {
    UCHAR  chChar;
    UCHAR  chScan;
    UCHAR  fbStatus;
    UCHAR  bNlsShift;
    USHORT fsState;
    ULONG  time;
} KBDKEYINFO, FAR *PKBDKEYINFO;
typedef struct _STRINGINBUF {
    USHORT cb;
    USHORT cchIn;
} STRINGINBUF, FAR *PSTRINGINBUF;
typedef struct _KBDINFO {
    USHORT cb;
    USHORT fsMask;
    USHORT chTurnAround;
    USHORT fsInterim;
    USHORT fsState;
} KBDINFO, FAR *PKBDINFO;
#define IO_WAIT   0
#define IO_NOWAIT 1
#define KEYBOARD_ECHO_ON     0x0001
#define KEYBOARD_ECHO_OFF    0x0002
#define KEYBOARD_BINARY_MODE 0x0004
#define KEYBOARD_ASCII_MODE  0x0008
USHORT APIENTRY KbdCharIn(PKBDKEYINFO pkbci, USHORT fWait, HKBD hkbd);
USHORT APIENTRY KbdPeek(PKBDKEYINFO pkbci, HKBD hkbd);
USHORT APIENTRY KbdStringIn(PCH pch, PSTRINGINBUF pchIn, USHORT fsWait, HKBD hkbd);
USHORT APIENTRY KbdFlushBuffer(HKBD hkbd);
USHORT APIENTRY KbdGetStatus(PKBDINFO pkbdinfo, HKBD hkbd);
USHORT APIENTRY KbdSetStatus(PKBDINFO pkbdinfo, HKBD hkbd);

/* Mouse */
typedef struct _MOUEVENTINFO {
    USHORT fs;
    ULONG  time;
    SHORT  row;
    SHORT  col;
} MOUEVENTINFO, FAR *PMOUEVENTINFO;
typedef struct _NOPTRRECT {
    USHORT row, col, cRow, cCol;
} NOPTRRECT, FAR *PNOPTRRECT;
#define MOU_NOWAIT 0
#define MOU_WAIT   1
USHORT APIENTRY MouOpen(PSZ pszDvrName, PHMOU phmou);
USHORT APIENTRY MouClose(HMOU hmou);
USHORT APIENTRY MouDrawPtr(HMOU hmou);
USHORT APIENTRY MouRemovePtr(PNOPTRRECT pmourtRect, HMOU hmou);
USHORT APIENTRY MouReadEventQue(PMOUEVENTINFO pmouevEvent, PUSHORT pfWait, HMOU hmou);
USHORT APIENTRY MouGetNumQueEl(PVOID pmouqi, HMOU hmou);
USHORT APIENTRY MouSetEventMask(PUSHORT pfsEvents, HMOU hmou);
USHORT APIENTRY MouGetEventMask(PUSHORT pfsEvents, HMOU hmou);

#ifdef __cplusplus
}
#endif

#endif /* __BSESUB_H */
