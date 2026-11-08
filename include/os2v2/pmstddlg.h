/*
 * pmstddlg.h - the standard file dialog of the 32-bit Presentation
 * Manager (PMCTLS), for clang-retro (i386-pc-os2).
 *
 * Copyright (C) clang-retro contributors.
 * SPDX-License-Identifier: AGPL-3.0-only
 */
#ifndef __PMSTDDLG_H
#define __PMSTDDLG_H

#include <os2def.h>
#include <pmwin.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CCHMAXPATH
#define CCHMAXPATH     260
#define CCHMAXPATHCOMP 256
#endif

typedef PSZ APSZ[1];
typedef APSZ *PAPSZ;

#define FDS_CENTER          0x00000001L
#define FDS_CUSTOM          0x00000002L
#define FDS_FILTERUNION     0x00000004L
#define FDS_HELPBUTTON      0x00000008L
#define FDS_APPLYBUTTON     0x00000010L
#define FDS_PRELOAD_VOLINFO 0x00000020L
#define FDS_MODELESS        0x00000040L
#define FDS_INCLUDE_EAS     0x00000080L
#define FDS_OPEN_DIALOG     0x00000100L
#define FDS_SAVEAS_DIALOG   0x00000200L
#define FDS_MULTIPLESEL     0x00000400L
#define FDS_ENABLEFILELB    0x00000800L

#define FDS_EFSELECTION 0
#define FDS_LBSELECTION 1

#define FDS_SUCCESSFUL              0
#define FDS_ERR_DEALLOCATE_MEMORY   1
#define FDS_ERR_FILTER_TRUNC        2
#define FDS_ERR_INVALID_DIALOG      3
#define FDS_ERR_INVALID_DRIVE       4
#define FDS_ERR_INVALID_FILTER      5
#define FDS_ERR_INVALID_PATHFILE    6
#define FDS_ERR_OUT_OF_MEMORY       7
#define FDS_ERR_PATH_TOO_LONG       8
#define FDS_ERR_TOO_MANY_FILE_TYPES 9
#define FDS_ERR_INVALID_VERSION     10
#define FDS_ERR_INVALID_CUSTOM_HANDLE 11
#define FDS_ERR_DIALOG_LOAD_ERROR   12
#define FDS_ERR_DRIVE_ERROR         13

typedef struct _FILEDLG {
    ULONG   cbSize;
    ULONG   fl;
    ULONG   ulUser;
    LONG    lReturn;
    LONG    lSRC;
    PSZ     pszTitle;
    PSZ     pszOKButton;
    PFNWP   pfnDlgProc;
    PSZ     pszIType;
    PAPSZ   papszITypeList;
    PSZ     pszIDrive;
    PAPSZ   papszIDriveList;
    HMODULE hMod;
    CHAR    szFullFile[CCHMAXPATH];
    PAPSZ   papszFQFilename;
    ULONG   ulFQFCount;
    USHORT  usDlgId;
    SHORT   x;
    SHORT   y;
    SHORT   sEAType;
} FILEDLG, *PFILEDLG;

HWND    APIENTRY WinFileDlg(HWND hwndP, HWND hwndO, PFILEDLG pfild);
MRESULT APIENTRY WinDefFileDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
BOOL    APIENTRY WinFreeFileDlgList(PAPSZ papszFQFilename);

#ifdef __cplusplus
}
#endif

#endif /* __PMSTDDLG_H */
