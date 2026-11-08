/*
 * pmshl.h - the shell of the 32-bit Presentation Manager, for clang-retro
 * (i386-pc-os2): the Window List (the switch list) and the profile calls
 * (OS2.INI and other .INI files).
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
typedef LHANDLE HPROGRAM, *PHPROGRAM;
typedef LHANDLE HINI, *PHINI;

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
#define PROG_VDM              4
#define PROG_WINDOWEDVDM      7
#define PROG_DLL              6
#define PROG_PDD              8
#define PROG_VDD              9
#define PROG_WINDOW_REAL      10
#define PROG_WINDOW_PROT      11
#define PROG_30_STD           11
#define PROG_WINDOW_AUTO      12
#define PROG_SEAMLESSVDM      13
#define PROG_30_STDSEAMLESSVDM 13
#define PROG_SEAMLESSCOMMON   14
#define PROG_30_STDSEAMLESSCOMMON 14
#define PROG_31_STDSEAMLESSVDM 15
#define PROG_31_STDSEAMLESSCOMMON 16
#define PROG_31_ENHSEAMLESSVDM 17
#define PROG_31_ENHSEAMLESSCOMMON 18
#define PROG_31_ENH           19
#define PROG_31_STD           20
#define PROG_RESERVED         255

typedef struct _SWCNTRL {
    HWND     hwnd;
    HWND     hwndIcon;
    HPROGRAM hprog;
    PID      idProcess;
    ULONG    idSession;
    ULONG    uchVisibility;
    ULONG    fbJump;
    CHAR     szSwtitle[MAXNAMEL + 4];
    ULONG    bProgType;
} SWCNTRL, *PSWCNTRL;

typedef struct _SWENTRY {
    HSWITCH hswitch;
    SWCNTRL swctl;
} SWENTRY, *PSWENTRY;

typedef struct _SWBLOCK {
    ULONG   cswentry;
    SWENTRY aswentry[1];
} SWBLOCK, *PSWBLOCK;

HSWITCH APIENTRY WinAddSwitchEntry(PSWCNTRL pswctl);
ULONG   APIENTRY WinRemoveSwitchEntry(HSWITCH hsw);
ULONG   APIENTRY WinChangeSwitchEntry(HSWITCH hswitch, PSWCNTRL pswctl);
HSWITCH APIENTRY WinCreateSwitchEntry(HAB hab, PSWCNTRL pswctl);
ULONG   APIENTRY WinQuerySessionTitle(HAB hab, ULONG usSession, PSZ pszTitle,
                                      ULONG usTitlelen);
ULONG   APIENTRY WinQuerySwitchEntry(HSWITCH hswitch, PSWCNTRL pswctl);
HSWITCH APIENTRY WinQuerySwitchHandle(HWND hwnd, PID pid);
ULONG   APIENTRY WinQuerySwitchList(HAB hab, PSWBLOCK pswblk, ULONG usDataLength);
ULONG   APIENTRY WinQueryTaskSizePos(HAB hab, ULONG usScreenGroup, PSWP pswp);
ULONG   APIENTRY WinQueryTaskTitle(ULONG usSession, PSZ pszTitle, ULONG usTitlelen);
ULONG   APIENTRY WinSwitchToProgram(HSWITCH hswitch);

/* Profiles */
#define HINI_PROFILE        ((HINI)0L)
#define HINI_USERPROFILE    ((HINI)-1L)
#define HINI_SYSTEMPROFILE  ((HINI)-2L)
#define HINI_USER           HINI_USERPROFILE
#define HINI_SYSTEM         HINI_SYSTEMPROFILE
HINI  APIENTRY PrfOpenProfile(HAB hab, PCSZ pszFileName);
BOOL  APIENTRY PrfCloseProfile(HINI hini);
LONG  APIENTRY PrfQueryProfileInt(HINI hini, PCSZ pszApp, PCSZ pszKey, LONG sDefault);
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
