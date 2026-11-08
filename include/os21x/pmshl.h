/*
 * pmshl.h - the shell of the 16-bit Presentation Manager (OS/2 1.1 and
 * later), for clang-retro (i8086-pc-os2): the Task List (the switch list)
 * and the profile calls (OS2.INI and other .INI files).
 *
 * Copyright (C) clang-retro contributors.
 * SPDX-License-Identifier: AGPL-3.0-only
 */
#ifndef __PMSHL_H
#define __PMSHL_H

#include <os2def.h>
#include <pmwin.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The switch list */
#define MAXNAMEL 60
typedef LHANDLE HPROGRAM, FAR *PHPROGRAM;
typedef LHANDLE HINI, FAR *PHINI;

#define SWL_INVISIBLE   0x01
#define SWL_GRAYED      0x02
#define SWL_VISIBLE     0x04
#define SWL_NOTJUMPABLE 0x01
#define SWL_JUMPABLE    0x02

#define PROG_DEFAULT          0
#define PROG_FULLSCREEN       1
#define PROG_WINDOWABLEVIO    2
#define PROG_PM               3
#define PROG_GROUP            5
#define PROG_REAL             4
#define PROG_DLL              6
#define PROG_RESERVED         255

typedef struct _SWCNTRL {
    HWND     hwnd;
    HWND     hwndIcon;
    HPROGRAM hprog;
    USHORT   idProcess;
    USHORT   idSession;
    UCHAR    uchVisibility;
    UCHAR    fbJump;
    CHAR     szSwtitle[MAXNAMEL + 1];
    BYTE     fReserved;
} SWCNTRL, FAR *PSWCNTRL;

typedef struct _SWENTRY {
    HSWITCH hswitch;
    SWCNTRL swctl;
} SWENTRY, FAR *PSWENTRY;

typedef struct _SWBLOCK {
    USHORT  cswentry;
    SWENTRY aswentry[1];
} SWBLOCK, FAR *PSWBLOCK;

HSWITCH APIENTRY WinAddSwitchEntry(PSWCNTRL pswctl);
USHORT  APIENTRY WinRemoveSwitchEntry(HSWITCH hsw);
USHORT  APIENTRY WinChangeSwitchEntry(HSWITCH hswitch, PSWCNTRL pswctl);
HSWITCH APIENTRY WinCreateSwitchEntry(HAB hab, PSWCNTRL pswctl);
USHORT  APIENTRY WinQuerySessionTitle(HAB hab, USHORT usSession, PSZ pszTitle,
                                      USHORT usTitlelen);
USHORT  APIENTRY WinQuerySwitchEntry(HSWITCH hswitch, PSWCNTRL pswctl);
HSWITCH APIENTRY WinQuerySwitchHandle(HWND hwnd, PID pid);
USHORT  APIENTRY WinQuerySwitchList(HAB hab, PSWBLOCK pswblk, USHORT usDataLength);
USHORT  APIENTRY WinQueryTaskSizePos(HAB hab, USHORT usScreenGroup, PSWP pswp);
USHORT  APIENTRY WinQueryTaskTitle(USHORT usSession, PSZ pszTitle, USHORT usTitlelen);
USHORT  APIENTRY WinSwitchToProgram(HSWITCH hswitch);

/* Profiles */
#define HINI_PROFILE        ((HINI)0L)
#define HINI_USERPROFILE    ((HINI)-1L)
#define HINI_SYSTEMPROFILE  ((HINI)-2L)
#define HINI_USER           HINI_USERPROFILE
#define HINI_SYSTEM         HINI_SYSTEMPROFILE
HINI  APIENTRY PrfOpenProfile(HAB hab, PCSZ pszFileName);
BOOL  APIENTRY PrfCloseProfile(HINI hini);
SHORT   APIENTRY PrfQueryProfileInt(HINI hini, PCSZ pszApp, PCSZ pszKey, SHORT sDefault);
ULONG APIENTRY PrfQueryProfileString(HINI hini, PCSZ pszApp, PCSZ pszKey,
                                     PCSZ pszDefault, PVOID pBuffer, ULONG cchBufferMax);
BOOL  APIENTRY PrfWriteProfileString(HINI hini, PCSZ pszApp, PCSZ pszKey, PCSZ pszData);
BOOL  APIENTRY PrfQueryProfileSize(HINI hini, PCSZ pszApp, PCSZ pszKey, PULONG pulReqLen);
BOOL  APIENTRY PrfQueryProfileData(HINI hini, PCSZ pszApp, PCSZ pszKey, PVOID pBuffer,
                                   PULONG pulBuffLen);
BOOL  APIENTRY PrfWriteProfileData(HINI hini, PCSZ pszApp, PCSZ pszKey, PVOID pData,
                                   ULONG cchDataLen);

#ifdef __cplusplus
}
#endif

#endif /* __PMSHL_H */
