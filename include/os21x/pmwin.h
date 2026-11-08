/*
 * pmwin.h - the window manager of the 16-bit Presentation Manager (PMWIN,
 * OS/2 1.1 and later), for clang-retro (i8086-pc-os2): message queues,
 * windows and frames, messages, painting, message boxes, menus, dialogs,
 * the standard controls (buttons, static text, entry fields, list boxes,
 * scroll bars, and the combination boxes and multi-line entry fields of
 * OS/2 1.2), accelerator tables, pointers, the cursor and the clipboard.
 * Not yet: hooks, atoms and help.
 *
 * Every function is __far __pascal and every pointer far. Window
 * procedures are MRESULT EXPENTRY Proc(HWND, USHORT, MPARAM, MPARAM): the
 * message is 16 bits, and EXPENTRY loads the program's data segment.
 * Coordinates and sizes in window calls are SHORT, as in OS/2 1.x.
 *
 * Copyright (C) clang-retro contributors.
 * SPDX-License-Identifier: AGPL-3.0-only
 */
#ifndef __PMWIN_H
#define __PMWIN_H

#include <os2def.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef MRESULT (APIENTRY *PFNWP)(HWND, USHORT, MPARAM, MPARAM);

typedef struct _QMSG {
    HWND   hwnd;
    USHORT msg;
    MPARAM mp1;
    MPARAM mp2;
    ULONG  time;
    POINTL ptl;
} QMSG, FAR *PQMSG;

/* Anchor blocks and message queues */
HAB     APIENTRY WinInitialize(USHORT flOptions);
BOOL    APIENTRY WinTerminate(HAB hab);
HMQ     APIENTRY WinCreateMsgQueue(HAB hab, SHORT cmsg);
BOOL    APIENTRY WinDestroyMsgQueue(HMQ hmq);
BOOL    APIENTRY WinGetMsg(HAB hab, PQMSG pqmsg, HWND hwndFilter, USHORT msgFilterFirst,
                           USHORT msgFilterLast);
BOOL    APIENTRY WinPeekMsg(HAB hab, PQMSG pqmsg, HWND hwndFilter, USHORT msgFilterFirst,
                            USHORT msgFilterLast, USHORT fl);
MRESULT APIENTRY WinDispatchMsg(HAB hab, PQMSG pqmsg);
BOOL    APIENTRY WinPostMsg(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);
MRESULT APIENTRY WinSendMsg(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);
BOOL    APIENTRY WinPostQueueMsg(HMQ hmq, USHORT msg, MPARAM mp1, MPARAM mp2);
ERRORID APIENTRY WinGetLastError(HAB hab);
#define PM_REMOVE   0x0001
#define PM_NOREMOVE 0x0000

/* Windows */
#define HWND_DESKTOP ((HWND)1)
#define HWND_OBJECT  ((HWND)2)
#define HWND_TOP     ((HWND)3)
#define HWND_BOTTOM  ((HWND)4)

#define CS_MOVENOTIFY   0x00000001L
#define CS_SIZEREDRAW   0x00000004L
#define CS_HITTEST      0x00000008L
#define CS_PUBLIC       0x00000010L
#define CS_FRAME        0x00000020L
#define CS_CLIPCHILDREN 0x20000000L
#define CS_CLIPSIBLINGS 0x10000000L
#define CS_PARENTCLIP   0x08000000L
#define CS_SAVEBITS     0x04000000L
#define CS_SYNCPAINT    0x02000000L

#define WS_VISIBLE      0x80000000L
#define WS_DISABLED     0x40000000L
#define WS_CLIPCHILDREN 0x20000000L
#define WS_CLIPSIBLINGS 0x10000000L
#define WS_PARENTCLIP   0x08000000L
#define WS_SAVEBITS     0x04000000L
#define WS_SYNCPAINT    0x02000000L
#define WS_MINIMIZED    0x01000000L
#define WS_MAXIMIZED    0x00800000L
#define WS_GROUP        0x00010000L
#define WS_TABSTOP      0x00020000L

#define FCF_TITLEBAR      0x00000001L
#define FCF_SYSMENU       0x00000002L
#define FCF_MENU          0x00000004L
#define FCF_SIZEBORDER    0x00000008L
#define FCF_MINBUTTON     0x00000010L
#define FCF_MAXBUTTON     0x00000020L
#define FCF_MINMAX        0x00000030L
#define FCF_VERTSCROLL    0x00000040L
#define FCF_HORZSCROLL    0x00000080L
#define FCF_DLGBORDER     0x00000100L
#define FCF_BORDER        0x00000200L
#define FCF_SHELLPOSITION 0x00000400L
#define FCF_TASKLIST      0x00000800L
#define FCF_NOBYTEALIGN   0x00001000L
#define FCF_NOMOVEWITHOWNER 0x00002000L
#define FCF_ICON          0x00004000L
#define FCF_ACCELTABLE    0x00008000L
#define FCF_SYSMODAL      0x00010000L
#define FCF_SCREENALIGN   0x00020000L
#define FCF_MOUSEALIGN    0x00040000L
#define FCF_STANDARD      0x0008CC3FL

#define FID_SYSMENU    0x8002
#define FID_TITLEBAR   0x8003
#define FID_MINMAX     0x8004
#define FID_MENU       0x8005
#define FID_VERTSCROLL 0x8006
#define FID_HORZSCROLL 0x8007
#define FID_CLIENT     0x8008

BOOL    APIENTRY WinRegisterClass(HAB hab, PCSZ pszClassName, PFNWP pfnWndProc, ULONG flStyle,
                                  USHORT cbWindowData);
HWND    APIENTRY WinCreateStdWindow(HWND hwndParent, ULONG flStyle, PULONG pflCreateFlags,
                                    PCSZ pszClientClass, PCSZ pszTitle, ULONG styleClient,
                                    HMODULE hmod, USHORT idResources, PHWND phwndClient);
HWND    APIENTRY WinCreateWindow(HWND hwndParent, PCSZ pszClass, PCSZ pszName, ULONG flStyle,
                                 SHORT x, SHORT y, SHORT cx, SHORT cy, HWND hwndOwner,
                                 HWND hwndInsertBehind, USHORT id, PVOID pCtlData,
                                 PVOID pPresParams);
BOOL    APIENTRY WinDestroyWindow(HWND hwnd);
MRESULT APIENTRY WinDefWindowProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);
BOOL    APIENTRY WinShowWindow(HWND hwnd, BOOL fShow);
BOOL    APIENTRY WinSetWindowText(HWND hwnd, PCSZ pszText);
SHORT   APIENTRY WinQueryWindowText(HWND hwnd, SHORT cchBufferMax, PCH pchBuffer);
HWND    APIENTRY WinQueryWindow(HWND hwnd, SHORT cmd, BOOL fLock);
HWND    APIENTRY WinWindowFromID(HWND hwndParent, USHORT id);
BOOL    APIENTRY WinQueryWindowRect(HWND hwnd, PRECTL prclDest);
BOOL    APIENTRY WinInvalidateRect(HWND hwnd, PRECTL pwrc, BOOL fIncludeChildren);
BOOL    APIENTRY WinUpdateWindow(HWND hwnd);
BOOL    APIENTRY WinSetWindowPos(HWND hwnd, HWND hwndInsertBehind, SHORT x, SHORT y, SHORT cx,
                                 SHORT cy, USHORT fl);
BOOL    APIENTRY WinSetWindowULong(HWND hwnd, SHORT index, ULONG ul);
ULONG   APIENTRY WinQueryWindowULong(HWND hwnd, SHORT index);
BOOL    APIENTRY WinSetWindowPtr(HWND hwnd, SHORT index, PVOID p);
PVOID   APIENTRY WinQueryWindowPtr(HWND hwnd, SHORT index);
LONG    APIENTRY WinQuerySysValue(HWND hwndDeskTop, SHORT iSysValue);
BOOL    APIENTRY WinAlarm(HWND hwndDesktop, USHORT rgfType);
USHORT  APIENTRY WinStartTimer(HAB hab, HWND hwnd, USHORT idTimer, USHORT dtTimeout);
BOOL    APIENTRY WinStopTimer(HAB hab, HWND hwnd, USHORT idTimer);
#define QW_NEXT   0
#define QW_PREV   1
#define QW_TOP    2
#define QW_BOTTOM 3
#define QW_OWNER  4
#define QW_PARENT 5
#define SWP_SIZE     0x0001
#define SWP_MOVE     0x0002
#define SWP_ZORDER   0x0004
#define SWP_SHOW     0x0008
#define SWP_HIDE     0x0010
#define SWP_ACTIVATE 0x0080
#define SV_CXSCREEN 20
#define SV_CYSCREEN 21

/* Messages */
#define WM_NULL           0x0000
#define WM_CREATE         0x0001
#define WM_DESTROY        0x0002
#define WM_ENABLE         0x0004
#define WM_SHOW           0x0005
#define WM_MOVE           0x0006
#define WM_SIZE           0x0007
#define WM_ADJUSTWINDOWPOS 0x0008
#define WM_CALCVALIDRECTS 0x0009
#define WM_SETWINDOWPARAMS 0x000a
#define WM_QUERYWINDOWPARAMS 0x000b
#define WM_HITTEST        0x000c
#define WM_ACTIVATE       0x000d
#define WM_SETFOCUS       0x000f
#define WM_SETSELECTION   0x0010
#define WM_COMMAND        0x0020
#define WM_SYSCOMMAND     0x0021
#define WM_HELP           0x0022
#define WM_PAINT          0x0023
#define WM_TIMER          0x0024
#define WM_CLOSE          0x0029
#define WM_QUIT           0x002a
#define WM_CONTROL        0x0030
#define WM_VSCROLL        0x0031
#define WM_HSCROLL        0x0032
#define WM_INITMENU       0x0033
#define WM_MENUSELECT     0x0034
#define WM_MENUEND        0x0035
#define WM_INITDLG        0x003b
#define WM_ERASEBACKGROUND 0x004f
#define WM_MOUSEMOVE      0x0070
#define WM_BUTTON1DOWN    0x0071
#define WM_BUTTON1UP      0x0072
#define WM_BUTTON1DBLCLK  0x0073
#define WM_BUTTON2DOWN    0x0074
#define WM_BUTTON2UP      0x0075
#define WM_BUTTON2DBLCLK  0x0076
#define WM_CHAR           0x007a
#define WM_USER           0x1000

#define KC_CHAR      0x0001
#define KC_VIRTUALKEY 0x0002
#define KC_SCANCODE  0x0004
#define KC_SHIFT     0x0008
#define KC_CTRL      0x0010
#define KC_ALT       0x0020
#define KC_KEYUP     0x0040

/* Message boxes */
#define MB_OK              0x0000
#define MB_OKCANCEL        0x0001
#define MB_RETRYCANCEL     0x0002
#define MB_ABORTRETRYIGNORE 0x0003
#define MB_YESNO           0x0004
#define MB_YESNOCANCEL     0x0005
#define MB_CANCEL          0x0006
#define MB_ENTER           0x0007
#define MB_ENTERCANCEL     0x0008
#define MB_NOICON          0x0000
#define MB_CUANOTIFICATION 0x0000
#define MB_ICONQUESTION    0x0010
#define MB_ICONEXCLAMATION 0x0020
#define MB_CUAWARNING      0x0020
#define MB_ICONASTERISK    0x0030
#define MB_ICONHAND        0x0040
#define MB_CUACRITICAL     0x0040
#define MB_QUERY           MB_ICONQUESTION
#define MB_WARNING         MB_CUAWARNING
#define MB_INFORMATION     MB_ICONASTERISK
#define MB_CRITICAL        MB_CUACRITICAL
#define MB_ERROR           MB_CRITICAL
#define MB_DEFBUTTON1      0x0000
#define MB_DEFBUTTON2      0x0100
#define MB_DEFBUTTON3      0x0200
#define MB_APPLMODAL       0x0000
#define MB_SYSTEMMODAL     0x1000
#define MB_HELP            0x2000
#define MB_MOVEABLE        0x4000
#define MBID_OK     1
#define MBID_CANCEL 2
#define MBID_ABORT  3
#define MBID_RETRY  4
#define MBID_IGNORE 5
#define MBID_YES    6
#define MBID_NO     7
#define MBID_HELP   8
#define MBID_ENTER  9
#define MBID_ERROR  0xffff
USHORT  APIENTRY WinMessageBox(HWND hwndParent, HWND hwndOwner, PCSZ pszText, PCSZ pszCaption,
                               USHORT idWindow, USHORT flStyle);

/* Painting */
HPS   APIENTRY WinBeginPaint(HWND hwnd, HPS hps, PRECTL prclPaint);
BOOL  APIENTRY WinEndPaint(HPS hps);
HPS   APIENTRY WinGetPS(HWND hwnd);
BOOL  APIENTRY WinReleasePS(HPS hps);
BOOL  APIENTRY WinFillRect(HPS hps, PRECTL prcl, LONG lColor);
SHORT   APIENTRY WinDrawText(HPS hps, SHORT cchText, PCH lpchText, PRECTL prcl, LONG clrFore,
                             LONG clrBack, USHORT flCmd);
#define DT_LEFT       0x0000
#define DT_QUERYEXTENT 0x0002
#define DT_TEXTATTRS  0x0040
#define DT_EXTERNALLEADING 0x0080
#define DT_CENTER     0x0100
#define DT_RIGHT      0x0200
#define DT_TOP        0x0000
#define DT_VCENTER    0x0400
#define DT_BOTTOM     0x0800
#define DT_HALFTONE   0x1000
#define DT_MNEMONIC   0x2000
#define DT_WORDBREAK  0x4000
#define DT_ERASERECT  0x8000

/* More window calls */
#define QW_NEXTTOP    6
#define QW_PREVTOP    7
#define QW_FRAMEOWNER 8
#define QWL_USER     0
#define QWS_ID       (-1)
#define QWL_STYLE    (-2)
#define QWP_PFNWP    (-3)
#define QWL_HMQ      (-4)

#define SWP_NOREDRAW        0x0020
#define SWP_NOADJUST        0x0040
#define SWP_DEACTIVATE      0x0100
#define SWP_EXTSTATECHANGE  0x0200
#define SWP_MINIMIZE        0x0400
#define SWP_MAXIMIZE        0x0800
#define SWP_RESTORE         0x1000
#define SWP_FOCUSACTIVATE   0x2000
#define SWP_FOCUSDEACTIVATE 0x4000

typedef struct _SWP {
    USHORT fs;
    SHORT  cy;
    SHORT  cx;
    SHORT  y;
    SHORT  x;
    HWND   hwndInsertBehind;
    HWND   hwnd;
} SWP, FAR *PSWP;

typedef struct _CLASSINFO {
    ULONG  flClassStyle;
    PFNWP  pfnWindowProc;
    USHORT cbWindowData;
} CLASSINFO, FAR *PCLASSINFO;

typedef struct _CREATESTRUCT {
    PVOID  pPresParams;
    PVOID  pCtlData;
    USHORT id;
    HWND   hwndInsertBehind;
    HWND   hwndOwner;
    SHORT  cy;
    SHORT  cx;
    SHORT  y;
    SHORT  x;
    ULONG  flStyle;
    PSZ    pszText;
    PSZ    pszClass;
    HWND   hwndParent;
} CREATESTRUCT, FAR *PCREATESTRUCT;

typedef LHANDLE HENUM;

BOOL    APIENTRY WinQueryWindowPos(HWND hwnd, PSWP pswp);
BOOL    APIENTRY WinSetMultWindowPos(HAB hab, PSWP pswp, USHORT cswp);
BOOL    APIENTRY WinEnableWindow(HWND hwnd, BOOL fEnable);
BOOL    APIENTRY WinIsWindowEnabled(HWND hwnd);
BOOL    APIENTRY WinIsWindowVisible(HWND hwnd);
BOOL    APIENTRY WinIsWindow(HAB hab, HWND hwnd);
BOOL    APIENTRY WinIsChild(HWND hwnd, HWND hwndParent);
BOOL    APIENTRY WinSetFocus(HWND hwndDesktop, HWND hwndSetFocus);
HWND    APIENTRY WinQueryFocus(HWND hwndDesktop, BOOL fLock);
BOOL    APIENTRY WinSetActiveWindow(HWND hwndDesktop, HWND hwnd);
HWND    APIENTRY WinQueryActiveWindow(HWND hwndDesktop, BOOL fLock);
HWND    APIENTRY WinQueryDesktopWindow(HAB hab, HDC hdc);
HWND    APIENTRY WinQueryObjectWindow(HWND hwndDesktop);
BOOL    APIENTRY WinSetParent(HWND hwnd, HWND hwndNewParent, BOOL fRedraw);
BOOL    APIENTRY WinSetOwner(HWND hwnd, HWND hwndNewOwner);
SHORT   APIENTRY WinQueryWindowTextLength(HWND hwnd);
PFNWP   APIENTRY WinSubclassWindow(HWND hwnd, PFNWP pfnwp);
SHORT   APIENTRY WinQueryClassName(HWND hwnd, SHORT cchMax, PCH pch);
BOOL    APIENTRY WinQueryClassInfo(HAB hab, PCSZ pszClassName, PCLASSINFO pClassInfo);
BOOL    APIENTRY WinMapWindowPoints(HWND hwndFrom, HWND hwndTo, PPOINTL prgptl, SHORT cwpt);
HWND    APIENTRY WinWindowFromPoint(HWND hwnd, PPOINTL pptl, BOOL fChildren, BOOL fLock);
BOOL    APIENTRY WinQueryWindowProcess(HWND hwnd, PPID ppid, PTID ptid);
HAB     APIENTRY WinQueryAnchorBlock(HWND hwnd);
USHORT  APIENTRY WinQueryWindowUShort(HWND hwnd, SHORT index);
BOOL    APIENTRY WinSetWindowUShort(HWND hwnd, SHORT index, USHORT us);
BOOL    APIENTRY WinSetWindowBits(HWND hwnd, SHORT index, ULONG flData, ULONG flMask);
LONG    APIENTRY WinQuerySysColor(HWND hwndDesktop, LONG clr, LONG lReserved);
BOOL    APIENTRY WinSetSysColors(HWND hwndDesktop, ULONG flOptions, ULONG flFormat,
                                 LONG clrFirst, ULONG cclr, PLONG pclr);
BOOL    APIENTRY WinSetSysValue(HWND hwndDesktop, SHORT iSysValue, LONG lValue);
ULONG   APIENTRY WinGetCurrentTime(HAB hab);
HENUM   APIENTRY WinBeginEnumWindows(HWND hwnd);
HWND    APIENTRY WinGetNextWindow(HENUM henum);
BOOL    APIENTRY WinEndEnumWindows(HENUM henum);
BOOL    APIENTRY WinCalcFrameRect(HWND hwndFrame, PRECTL prcl, BOOL fClient);
BOOL    APIENTRY WinLockWindowUpdate(HWND hwndDesktop, HWND hwndLockUpdate);
BOOL    APIENTRY WinValidateRect(HWND hwnd, PRECTL prcl, BOOL fIncludeChildren);
BOOL    APIENTRY WinQueryUpdateRect(HWND hwnd, PRECTL prcl);
BOOL    APIENTRY WinInvalidateRegion(HWND hwnd, HRGN hrgn, BOOL fIncludeChildren);
SHORT   APIENTRY WinScrollWindow(HWND hwnd, SHORT dx, SHORT dy, PRECTL prclScroll,
                                 PRECTL prclClip, HRGN hrgnUpdate, PRECTL prclUpdate,
                                 USHORT rgfsw);
#define SW_SCROLLCHILDREN 0x0001
#define SW_INVALIDATERGN  0x0002

BOOL    APIENTRY WinSetPresParam(HWND hwnd, ULONG id, ULONG cbParam, PVOID pbParam);
ULONG   APIENTRY WinQueryPresParam(HWND hwnd, ULONG id1, ULONG id2, PULONG pulId, ULONG cbBuf,
                                   PVOID pbBuf, USHORT fs);
BOOL    APIENTRY WinRemovePresParam(HWND hwnd, ULONG id);
#define PP_FOREGROUNDCOLOR      1L
#define PP_FOREGROUNDCOLORINDEX 2L
#define PP_BACKGROUNDCOLOR      3L
#define PP_BACKGROUNDCOLORINDEX 4L
#define PP_HILITEFOREGROUNDCOLOR      5L
#define PP_HILITEFOREGROUNDCOLORINDEX 6L
#define PP_HILITEBACKGROUNDCOLOR      7L
#define PP_HILITEBACKGROUNDCOLORINDEX 8L
#define PP_DISABLEDFOREGROUNDCOLOR      9L
#define PP_DISABLEDFOREGROUNDCOLORINDEX 10L
#define PP_DISABLEDBACKGROUNDCOLOR      11L
#define PP_DISABLEDBACKGROUNDCOLORINDEX 12L
#define PP_BORDERCOLOR          13L
#define PP_BORDERCOLORINDEX     14L
#define PP_FONTNAMESIZE         15L
#define PP_FONTHANDLE           16L
#define QPF_NOINHERIT   0x0001
#define QPF_ID1COLORINDEX 0x0002
#define QPF_ID2COLORINDEX 0x0004
#define QPF_PURERGBCOLOR  0x0008
#define QPF_VALIDFLAGS    0x000F

/* Rectangles */
BOOL    APIENTRY WinSetRect(HAB hab, PRECTL prcl, SHORT xLeft, SHORT yBottom, SHORT xRight,
                            SHORT yTop);
BOOL APIENTRY WinSetRectEmpty(HAB hab, PRECTL prcl);
BOOL APIENTRY WinCopyRect(HAB hab, PRECTL prclDst, PRECTL prclSrc);
BOOL APIENTRY WinIsRectEmpty(HAB hab, PRECTL prcl);
BOOL APIENTRY WinEqualRect(HAB hab, PRECTL prcl1, PRECTL prcl2);
BOOL APIENTRY WinPtInRect(HAB hab, PRECTL prcl, PPOINTL pptl);
BOOL    APIENTRY WinOffsetRect(HAB hab, PRECTL prcl, SHORT cx, SHORT cy);
BOOL    APIENTRY WinInflateRect(HAB hab, PRECTL prcl, SHORT cx, SHORT cy);
BOOL APIENTRY WinIntersectRect(HAB hab, PRECTL prclDst, PRECTL prclSrc1,
                               PRECTL prclSrc2);
BOOL APIENTRY WinUnionRect(HAB hab, PRECTL prclDst, PRECTL prclSrc1, PRECTL prclSrc2);
BOOL APIENTRY WinSubtractRect(HAB hab, PRECTL prclDst, PRECTL prclSrc1,
                              PRECTL prclSrc2);

/* More messages */
#define WM_MEASUREITEM       0x0037
#define WM_DRAWITEM          0x0036
#define WM_CONTROLPOINTER    0x0038
#define WM_QUERYDLGCODE      0x003a
#define WM_SUBSTITUTESTRING  0x003c
#define WM_MATCHMNEMONIC     0x003d
#define WM_SAVEAPPLICATION   0x003e
#define WM_FLASHWINDOW       0x0040
#define WM_FORMATFRAME       0x0041
#define WM_UPDATEFRAME       0x0042
#define WM_FOCUSCHANGE       0x0043
#define WM_SETBORDERSIZE     0x0044
#define WM_TRACKFRAME        0x0045
#define WM_MINMAXFRAME       0x0046
#define WM_SETICON           0x0047
#define WM_QUERYICON         0x0048
#define WM_SETACCELTABLE     0x0049
#define WM_QUERYACCELTABLE   0x004a
#define WM_TRANSLATEACCEL    0x004b
#define WM_QUERYTRACKINFO    0x004c
#define WM_QUERYBORDERSIZE   0x004d
#define WM_NEXTMENU          0x004e
#define WM_QUERYFRAMEINFO    0x0050
#define WM_QUERYFOCUSCHAIN   0x0051
#define WM_CALCFRAMERECT     0x0053
#define WM_WINDOWPOSCHANGED  0x0055
#define WM_QUERYFRAMECTLCOUNT 0x0059
#define WM_QUERYHELPINFO     0x005b
#define WM_SETHELPINFO       0x005c
#define WM_ERROR             0x005d
#define WM_RENDERFMT         0x0060
#define WM_RENDERALLFMTS     0x0061
#define WM_DESTROYCLIPBOARD  0x0062
#define WM_PAINTCLIPBOARD    0x0063
#define WM_SIZECLIPBOARD     0x0064
#define WM_HSCROLLCLIPBOARD  0x0065
#define WM_VSCROLLCLIPBOARD  0x0066
#define WM_DRAWCLIPBOARD     0x0067
#define WM_BUTTON3DOWN       0x0077
#define WM_BUTTON3UP         0x0078
#define WM_BUTTON3DBLCLK     0x0079
#define WM_VIOCHAR           0x007b
#define WM_JOURNALNOTIFY     0x007c
#define WM_PRESPARAMCHANGED  0x002f
#define WM_SYSCOLORCHANGE    0x002b
#define WM_PSETFOCUS         0x0012
#define WM_PSYSCOLORCHANGE   0x0013
#define WM_PSIZE             0x0014
#define WM_PACTIVATE         0x0015
#define WM_PCONTROL          0x0016
#define WM_SYSVALUECHANGED   0x002d
#define WM_APPTERMINATENOTIFY 0x002e
#define WM_SEM1              0x0025

#define BMSG_POST          0x0000
#define BMSG_SEND          0x0001
#define BMSG_POSTQUEUE     0x0002
#define BMSG_DESCENDANTS   0x0004
#define BMSG_FRAMEONLY     0x0008
BOOL    APIENTRY WinBroadcastMsg(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2, USHORT rgf);
BOOL    APIENTRY WinInSendMsg(HAB hab);
BOOL    APIENTRY WinWaitMsg(HAB hab, USHORT msgFirst, USHORT msgLast);
BOOL    APIENTRY WinQueryMsgPos(HAB hab, PPOINTL pptl);
ULONG   APIENTRY WinQueryMsgTime(HAB hab);
ULONG   APIENTRY WinQueryQueueStatus(HWND hwndDesktop);
BOOL    APIENTRY WinCancelShutdown(HMQ hmq, BOOL fCancelAlways);

/* WM_COMMAND: where it came from (SHORT1FROMMP(mp2)) */
#define CMDSRC_OTHER       0
#define CMDSRC_PUSHBUTTON  1
#define CMDSRC_MENU        2
#define CMDSRC_ACCELERATOR 3

/* WM_SYSCOMMAND */
#define SC_SIZE          0x8000
#define SC_MOVE          0x8001
#define SC_MINIMIZE      0x8002
#define SC_MAXIMIZE      0x8003
#define SC_CLOSE         0x8004
#define SC_NEXT          0x8005
#define SC_APPMENU       0x8006
#define SC_SYSMENU       0x8007
#define SC_RESTORE       0x8008
#define SC_NEXTFRAME     0x8009
#define SC_NEXTWINDOW    0x8010
#define SC_TASKMANAGER   0x8011
#define SC_HELPKEYS      0x8012
#define SC_HELPINDEX     0x8013
#define SC_HELPEXTENDED  0x8014
#define SC_SWITCHPANELIDS 0x8015
#define SC_DBE_FIRST     0x8018
#define SC_DBE_LAST      0x801F

/* Frames */
#define FS_ICON           0x00000001L
#define FS_ACCELTABLE     0x00000002L
#define FS_SHELLPOSITION  0x00000004L
#define FS_TASKLIST       0x00000008L
#define FS_NOBYTEALIGN    0x00000010L
#define FS_NOMOVEWITHOWNER 0x00000020L
#define FS_SYSMODAL       0x00000040L
#define FS_DLGBORDER      0x00000080L
#define FS_BORDER         0x00000100L
#define FS_SCREENALIGN    0x00000200L
#define FS_MOUSEALIGN     0x00000400L
#define FS_SIZEBORDER     0x00000800L
#define FS_STANDARD       0x0000000FL
#define FCF_DBE_APPSTAT   0x80000000L
#define FF_FLASHWINDOW    0x0001
#define FF_ACTIVE         0x0002
#define FF_FLASHHILITE    0x0004
#define FF_OWNERHIDDEN    0x0008
#define FF_DLGDISMISSED   0x0010
#define FF_OWNERDISABLED  0x0020
#define FF_SELECTED       0x0040
#define FF_NOACTIVATESWP  0x0080
BOOL    APIENTRY WinFlashWindow(HWND hwndFrame, BOOL fFlash);
#pragma pack(push, 2)
typedef struct _FRAMECDATA {
    USHORT cb;
    ULONG  flCreateFlags;
    USHORT hmodResources;
    USHORT idResources;
} FRAMECDATA, FAR *PFRAMECDATA;
#pragma pack(pop)
BOOL    APIENTRY WinCreateFrameControls(HWND hwndFrame, PFRAMECDATA pfcdata,
                                        PCSZ pszTitle);

/* System values (WinQuerySysValue) */
#define SV_SWAPBUTTON       0
#define SV_DBLCLKTIME       1
#define SV_CXDBLCLK         2
#define SV_CYDBLCLK         3
#define SV_CXSIZEBORDER     4
#define SV_CYSIZEBORDER     5
#define SV_ALARM            6
#define SV_CURSORRATE       9
#define SV_FIRSTSCROLLRATE  10
#define SV_SCROLLRATE       11
#define SV_NUMBEREDLISTS    12
#define SV_WARNINGFREQ      13
#define SV_NOTEFREQ         14
#define SV_ERRORFREQ        15
#define SV_WARNINGDURATION  16
#define SV_NOTEDURATION     17
#define SV_ERRORDURATION    18
#define SV_CXVSCROLL        22
#define SV_CYHSCROLL        23
#define SV_CYVSCROLLARROW   24
#define SV_CXHSCROLLARROW   25
#define SV_CXBORDER         26
#define SV_CYBORDER         27
#define SV_CXDLGFRAME       28
#define SV_CYDLGFRAME       29
#define SV_CYTITLEBAR       30
#define SV_CYVSLIDER        31
#define SV_CXHSLIDER        32
#define SV_CXMINMAXBUTTON   33
#define SV_CYMINMAXBUTTON   34
#define SV_CYMENU           35
#define SV_CXFULLSCREEN     36
#define SV_CYFULLSCREEN     37
#define SV_CXICON           38
#define SV_CYICON           39
#define SV_CXPOINTER        40
#define SV_CYPOINTER        41
#define SV_DEBUG            42
#define SV_CMOUSEBUTTONS    43
#define SV_CPOINTERBUTTONS  69
#define SV_POINTERLEVEL     44
#define SV_CURSORLEVEL      45
#define SV_TRACKRECTLEVEL   46
#define SV_CTIMERS          47
#define SV_MOUSEPRESENT     48
#define SV_CXBYTEALIGN      49
#define SV_CYBYTEALIGN      50

/* System colors (WinQuerySysColor, negative indexes) */
#define SYSCLR_BUTTONLIGHT        (-41L)
#define SYSCLR_BUTTONMIDDLE       (-40L)
#define SYSCLR_BUTTONDARK         (-39L)
#define SYSCLR_BUTTONDEFAULT      (-38L)
#define SYSCLR_TITLEBOTTOM        (-37L)
#define SYSCLR_SHADOW             (-36L)
#define SYSCLR_ICONTEXT           (-35L)
#define SYSCLR_DIALOGBACKGROUND   (-34L)
#define SYSCLR_HILITEFOREGROUND   (-33L)
#define SYSCLR_HILITEBACKGROUND   (-32L)
#define SYSCLR_INACTIVETITLETEXTBGND (-31L)
#define SYSCLR_ACTIVETITLETEXTBGND (-30L)
#define SYSCLR_INACTIVETITLETEXT  (-29L)
#define SYSCLR_ACTIVETITLETEXT    (-28L)
#define SYSCLR_OUTPUTTEXT         (-27L)
#define SYSCLR_WINDOWSTATICTEXT   (-26L)
#define SYSCLR_SCROLLBAR          (-25L)
#define SYSCLR_BACKGROUND         (-24L)
#define SYSCLR_ACTIVETITLE        (-23L)
#define SYSCLR_INACTIVETITLE      (-22L)
#define SYSCLR_MENU               (-21L)
#define SYSCLR_WINDOW             (-20L)
#define SYSCLR_WINDOWFRAME        (-19L)
#define SYSCLR_MENUTEXT           (-18L)
#define SYSCLR_WINDOWTEXT         (-17L)
#define SYSCLR_TITLETEXT          (-16L)
#define SYSCLR_ACTIVEBORDER       (-15L)
#define SYSCLR_INACTIVEBORDER     (-14L)
#define SYSCLR_APPWORKSPACE       (-13L)
#define SYSCLR_HELPBACKGROUND     (-12L)
#define SYSCLR_HELPTEXT           (-11L)
#define SYSCLR_HELPHILITE         (-10L)

/* Keys: WM_CHAR is fs = SHORT1FROMMP(mp1), the repeat count
   CHAR3FROMMP(mp1), the scan code CHAR4FROMMP(mp1), the character
   SHORT1FROMMP(mp2) and the virtual key SHORT2FROMMP(mp2). */
#define KC_PREVDOWN   0x0080
#define KC_LONEKEY    0x0100
#define KC_DEADKEY    0x0200
#define KC_COMPOSITE  0x0400
#define KC_INVALIDCOMP 0x0800
#define KC_TOGGLE     0x1000
#define KC_INVALIDCHAR 0x2000
#define KC_DBCSRSRVD1 0x4000
#define KC_DBCSRSRVD2 0x8000
#define VK_BUTTON1    0x01
#define VK_BUTTON2    0x02
#define VK_BUTTON3    0x03
#define VK_BREAK      0x04
#define VK_BACKSPACE  0x05
#define VK_TAB        0x06
#define VK_BACKTAB    0x07
#define VK_NEWLINE    0x08
#define VK_SHIFT      0x09
#define VK_CTRL       0x0A
#define VK_ALT        0x0B
#define VK_ALTGRAF    0x0C
#define VK_PAUSE      0x0D
#define VK_CAPSLOCK   0x0E
#define VK_ESC        0x0F
#define VK_SPACE      0x10
#define VK_PAGEUP     0x11
#define VK_PAGEDOWN   0x12
#define VK_END        0x13
#define VK_HOME       0x14
#define VK_LEFT       0x15
#define VK_UP         0x16
#define VK_RIGHT      0x17
#define VK_DOWN       0x18
#define VK_PRINTSCRN  0x19
#define VK_INSERT     0x1A
#define VK_DELETE     0x1B
#define VK_SCRLLOCK   0x1C
#define VK_NUMLOCK    0x1D
#define VK_ENTER      0x1E
#define VK_SYSRQ      0x1F
#define VK_F1         0x20
#define VK_F2         0x21
#define VK_F3         0x22
#define VK_F4         0x23
#define VK_F5         0x24
#define VK_F6         0x25
#define VK_F7         0x26
#define VK_F8         0x27
#define VK_F9         0x28
#define VK_F10        0x29
#define VK_F11        0x2A
#define VK_F12        0x2B
#define VK_F13        0x2C
#define VK_F14        0x2D
#define VK_F15        0x2E
#define VK_F16        0x2F
#define VK_F17        0x30
#define VK_F18        0x31
#define VK_F19        0x32
#define VK_F20        0x33
#define VK_F21        0x34
#define VK_F22        0x35
#define VK_F23        0x36
#define VK_F24        0x37
#define VK_MENU       VK_F10
SHORT   APIENTRY WinGetKeyState(HWND hwndDesktop, SHORT vkey);
SHORT   APIENTRY WinGetPhysKeyState(HWND hwndDesktop, SHORT sc);

/* Mouse messages: the position is SHORT1FROMMP(mp1), SHORT2FROMMP(mp1). */
BOOL    APIENTRY WinSetCapture(HWND hwndDesktop, HWND hwnd);
HWND    APIENTRY WinQueryCapture(HWND hwndDesktop, BOOL fLock);

/* Strings and messages from resources */
SHORT   APIENTRY WinLoadString(HAB hab, HMODULE hmod, USHORT id, SHORT cchMax, PSZ pchBuffer);
SHORT   APIENTRY WinLoadMessage(HAB hab, HMODULE hmod, USHORT id, SHORT cchMax, PSZ pchBuffer);

/* Pointers (mouse cursors) and icons */
#define SPTR_ARROW       1
#define SPTR_TEXT        2
#define SPTR_WAIT        3
#define SPTR_SIZE        4
#define SPTR_MOVE        5
#define SPTR_SIZENWSE    6
#define SPTR_SIZENESW    7
#define SPTR_SIZEWE      8
#define SPTR_SIZENS      9
#define SPTR_APPICON     10
#define SPTR_ICONINFORMATION 11
#define SPTR_ICONQUESTION 12
#define SPTR_ICONERROR   13
#define SPTR_ICONWARNING 14
#define SPTR_ILLEGAL     18
#define SPTR_FILE        19
#define SPTR_FOLDER      20
#define SPTR_MULTFILE    21
#define SPTR_PROGRAM     22
#define DP_NORMAL   0x0000
#define DP_HALFTONED 0x0001
#define DP_INVERTED 0x0002
HPOINTER APIENTRY WinLoadPointer(HWND hwndDesktop, HMODULE hmod, USHORT idres);
BOOL     APIENTRY WinDestroyPointer(HPOINTER hptr);
HPOINTER APIENTRY WinQuerySysPointer(HWND hwndDesktop, SHORT iptr, BOOL fLoad);
BOOL     APIENTRY WinSetPointer(HWND hwndDesktop, HPOINTER hptrNew);
HPOINTER APIENTRY WinQueryPointer(HWND hwndDesktop);
BOOL     APIENTRY WinShowPointer(HWND hwndDesktop, BOOL fShow);
BOOL     APIENTRY WinQueryPointerPos(HWND hwndDesktop, PPOINTL pptl);
BOOL    APIENTRY WinSetPointerPos(HWND hwndDesktop, SHORT x, SHORT y);
BOOL    APIENTRY WinDrawPointer(HPS hps, SHORT x, SHORT y, HPOINTER hptr, USHORT fs);

/* The text cursor */
BOOL APIENTRY WinCreateCursor(HWND hwnd, SHORT x, SHORT y, SHORT cx, SHORT cy, USHORT fs,
                              PRECTL prclClip);
BOOL APIENTRY WinDestroyCursor(HWND hwnd);
BOOL APIENTRY WinShowCursor(HWND hwnd, BOOL fShow);

/* Drawing helpers */
#define DB_PATCOPY      0x0000
#define DB_PATINVERT    0x0001
#define DB_DESTINVERT   0x0002
#define DB_AREAMIXMODE  0x0003
#define DB_ROP          0x0007
#define DB_INTERIOR     0x0008
#define DB_AREAATTRS    0x0010
#define DB_STANDARD     0x0100
#define DB_DLGBORDER    0x0200
#define DBM_NORMAL      0x0000
#define DBM_INVERT      0x0001
#define DBM_HALFTONE    0x0002
#define DBM_STRETCH     0x0004
#define DBM_IMAGEATTRS  0x0008
BOOL    APIENTRY WinDrawBorder(HPS hps, PRECTL prcl, SHORT cx, SHORT cy, LONG clrFore,
                               LONG clrBack, USHORT flCmd);
BOOL    APIENTRY WinDrawBitmap(HPS hpsDst, HBITMAP hbm, PRECTL pwrcSrc, PPOINTL pptlDst,
                               LONG clrFore, LONG clrBack, USHORT fl);
BOOL APIENTRY WinInvertRect(HPS hps, PRECTL prcl);

/* Accelerator tables */
#define AF_CHAR       0x0001
#define AF_VIRTUALKEY 0x0002
#define AF_SCANCODE   0x0004
#define AF_SHIFT      0x0008
#define AF_CONTROL    0x0010
#define AF_ALT        0x0020
#define AF_LONEKEY    0x0040
#define AF_SYSCOMMAND 0x0100
#define AF_HELP       0x0200
typedef struct _ACCEL {
    USHORT fs;
    USHORT key;
    USHORT cmd;
} ACCEL, FAR *PACCEL;
typedef struct _ACCELTABLE {
    USHORT cAccel;
    USHORT codepage;
    ACCEL  aaccel[1];
} ACCELTABLE, FAR *PACCELTABLE;
HACCEL  APIENTRY WinLoadAccelTable(HAB hab, HMODULE hmod, USHORT idAccelTable);
HACCEL APIENTRY WinCreateAccelTable(HAB hab, PACCELTABLE pAccelTable);
BOOL   APIENTRY WinDestroyAccelTable(HACCEL haccel);
BOOL   APIENTRY WinSetAccelTable(HAB hab, HACCEL haccel, HWND hwndFrame);
HACCEL APIENTRY WinQueryAccelTable(HAB hab, HWND hwndFrame);
BOOL   APIENTRY WinTranslateAccel(HAB hab, HWND hwnd, HACCEL haccel, PQMSG pqmsg);

/* Menus */
#define MIT_END      (-1)
#define MIT_NONE     (-1)
#define MIT_MEMERROR (-1)
#define MIT_ERROR    (-1)
#define MID_NONE     MIT_NONE
#define MID_ERROR    (-1)
#define MIS_TEXT        0x0001
#define MIS_BITMAP      0x0002
#define MIS_SEPARATOR   0x0004
#define MIS_OWNERDRAW   0x0008
#define MIS_SUBMENU     0x0010
#define MIS_MULTMENU    0x0020
#define MIS_SYSCOMMAND  0x0040
#define MIS_HELP        0x0080
#define MIS_STATIC      0x0100
#define MIS_BUTTONSEPARATOR 0x0200
#define MIS_BREAK       0x0400
#define MIS_BREAKSEPARATOR 0x0800
#define MIS_GROUP       0x1000
#define MIS_SINGLE      0x2000
#define MIA_NODISMISS   0x0020
#define MIA_FRAMED      0x1000
#define MIA_CHECKED     0x2000
#define MIA_DISABLED    0x4000
#define MIA_HILITED     0x8000
#define MS_ACTIONBAR    0x00000001L
#define MS_TITLEBUTTON  0x00000002L
#define MS_VERTICALFLIP 0x00000004L
typedef struct _MENUITEM {
    SHORT  iPosition;
    USHORT afStyle;
    USHORT afAttribute;
    USHORT id;
    HWND   hwndSubMenu;
    ULONG  hItem;
} MENUITEM, FAR *PMENUITEM;
#define MM_INSERTITEM          0x0180
#define MM_DELETEITEM          0x0181
#define MM_QUERYITEM           0x0182
#define MM_SETITEM             0x0183
#define MM_QUERYITEMCOUNT      0x0184
#define MM_STARTMENUMODE       0x0185
#define MM_ENDMENUMODE         0x0186
#define MM_REMOVEITEM          0x0188
#define MM_SELECTITEM          0x0189
#define MM_QUERYSELITEMID      0x018a
#define MM_QUERYITEMTEXT       0x018b
#define MM_QUERYITEMTEXTLENGTH 0x018c
#define MM_SETITEMHANDLE       0x018d
#define MM_SETITEMTEXT         0x018e
#define MM_ITEMPOSITIONFROMID  0x018f
#define MM_ITEMIDFROMPOSITION  0x0190
#define MM_QUERYITEMATTR       0x0191
#define MM_SETITEMATTR         0x0192
#define MM_ISITEMVALID         0x0193
HWND    APIENTRY WinLoadMenu(HWND hwndFrame, HMODULE hmod, USHORT idMenu);
HWND APIENTRY WinCreateMenu(HWND hwndParent, PVOID lpmt);
#define WinCheckMenuItem(hwndMenu, id, fcheck) \
    ((BOOL)WinSendMsg(hwndMenu, MM_SETITEMATTR, MPFROM2SHORT(id, TRUE), \
                      MPFROM2SHORT(MIA_CHECKED, (fcheck) ? MIA_CHECKED : 0)))
#define WinIsMenuItemChecked(hwndMenu, id) \
    ((BOOL)WinSendMsg(hwndMenu, MM_QUERYITEMATTR, MPFROM2SHORT(id, TRUE), \
                      MPFROMLONG(MIA_CHECKED)))
#define WinEnableMenuItem(hwndMenu, id, fEnable) \
    ((BOOL)WinSendMsg(hwndMenu, MM_SETITEMATTR, MPFROM2SHORT(id, TRUE), \
                      MPFROM2SHORT(MIA_DISABLED, (fEnable) ? 0 : MIA_DISABLED)))
#define WinIsMenuItemEnabled(hwndMenu, id) \
    (!(BOOL)WinSendMsg(hwndMenu, MM_QUERYITEMATTR, MPFROM2SHORT(id, TRUE), \
                       MPFROMLONG(MIA_DISABLED)))
#define WinSetMenuItemText(hwndMenu, id, psz) \
    ((BOOL)WinSendMsg(hwndMenu, MM_SETITEMTEXT, MPFROMLONG(id), MPFROMP(psz)))
#define WinIsMenuItemValid(hwndMenu, id) \
    ((BOOL)WinSendMsg(hwndMenu, MM_ISITEMVALID, MPFROM2SHORT(id, TRUE), \
                      MPFROMLONG(FALSE)))

/* Dialogs */
#define DID_OK     1
#define DID_CANCEL 2
#define DID_ERROR  0xffff
#define EDI_FIRSTTABITEM   0
#define EDI_LASTTABITEM    1
#define EDI_NEXTTABITEM    2
#define EDI_PREVTABITEM    3
#define EDI_FIRSTGROUPITEM 4
#define EDI_LASTGROUPITEM  5
#define EDI_NEXTGROUPITEM  6
#define EDI_PREVGROUPITEM  7
#define DLGC_ENTRYFIELD  0x0001
#define DLGC_BUTTON      0x0002
#define DLGC_RADIOBUTTON 0x0004
#define DLGC_STATIC      0x0008
#define DLGC_DEFAULT     0x0010
#define DLGC_PUSHBUTTON  0x0020
#define DLGC_CHECKBOX    0x0040
#define DLGC_SCROLLBAR   0x0080
#define DLGC_MENU        0x0100
#define DLGC_TABONCLICK  0x0200
#define DLGC_MLE         0x0400

/* A dialog template, as a DLGTEMPLATE resource holds it */
#pragma pack(push, 2)
typedef struct _DLGTITEM {
    USHORT fsItemStatus;
    USHORT cChildren;
    USHORT cchClassName;
    USHORT offClassName;
    USHORT cchText;
    USHORT offText;
    ULONG  flStyle;
    SHORT  x;
    SHORT  y;
    SHORT  cx;
    SHORT  cy;
    USHORT id;
    USHORT offPresParams;
    USHORT offCtlData;
} DLGTITEM, FAR *PDLGTITEM;
typedef struct _DLGTEMPLATE {
    USHORT   cbTemplate;
    USHORT   type;
    USHORT   codepage;
    USHORT   offadlgti;
    USHORT   fsTemplateStatus;
    USHORT   iItemFocus;
    USHORT   coffPresParams;
    DLGTITEM adlgti[1];
} DLGTEMPLATE, FAR *PDLGTEMPLATE;
#pragma pack(pop)

USHORT  APIENTRY WinDlgBox(HWND hwndParent, HWND hwndOwner, PFNWP pfnDlgProc, HMODULE hmod,
                           USHORT idDlg, PVOID pCreateParams);
HWND    APIENTRY WinLoadDlg(HWND hwndParent, HWND hwndOwner, PFNWP pfnDlgProc, HMODULE hmod,
                            USHORT idDlg, PVOID pCreateParams);
HWND    APIENTRY WinCreateDlg(HWND hwndParent, HWND hwndOwner, PFNWP pfnDlgProc,
                              PDLGTEMPLATE pdlgt, PVOID pCreateParams);
USHORT  APIENTRY WinProcessDlg(HWND hwndDlg);
BOOL    APIENTRY WinDismissDlg(HWND hwndDlg, USHORT usResult);
MRESULT APIENTRY WinDefDlgProc(HWND hwndDlg, USHORT msg, MPARAM mp1, MPARAM mp2);
MRESULT APIENTRY WinSendDlgItemMsg(HWND hwndDlg, USHORT idItem, USHORT msg, MPARAM mp1,
                                   MPARAM mp2);
BOOL    APIENTRY WinSetDlgItemText(HWND hwndDlg, USHORT idItem, PCSZ pszText);
USHORT  APIENTRY WinQueryDlgItemText(HWND hwndDlg, USHORT idItem, SHORT lMaxText, PSZ pszText);
SHORT   APIENTRY WinQueryDlgItemTextLength(HWND hwndDlg, USHORT idItem);
BOOL    APIENTRY WinSetDlgItemShort(HWND hwndDlg, USHORT idItem, USHORT usValue, BOOL fSigned);
BOOL    APIENTRY WinQueryDlgItemShort(HWND hwndDlg, USHORT idItem, PSHORT pResult,
                                      BOOL fSigned);
HWND    APIENTRY WinEnumDlgItem(HWND hwndDlg, HWND hwnd, USHORT code, BOOL fLock);
BOOL    APIENTRY WinMapDlgPoints(HWND hwndDlg, PPOINTL prgwptl, USHORT cwpt,
                                 BOOL fCalcWindowCoords);
SHORT   APIENTRY WinSubstituteStrings(HWND hwnd, PCSZ pszSrc, SHORT cchDstMax, PSZ pszDst);
#define WinCheckButton(hwndDlg, id, usCheckState) \
    ((ULONG)WinSendDlgItemMsg(hwndDlg, id, BM_SETCHECK, \
                              MPFROMSHORT(usCheckState), (MPARAM)NULL))
#define WinQueryButtonCheckstate(hwndDlg, id) \
    ((ULONG)WinSendDlgItemMsg(hwndDlg, id, BM_QUERYCHECK, (MPARAM)NULL, \
                              (MPARAM)NULL))
#define WinEnableControl(hwndDlg, id, fEnable) \
    WinEnableWindow(WinWindowFromID(hwndDlg, id), fEnable)
#define WinIsControlEnabled(hwndDlg, id) \
    ((BOOL)WinIsWindowEnabled(WinWindowFromID(hwndDlg, id)))

/* The standard window classes */
#define WC_FRAME       ((PSZ)0xffff0001L)
#define WC_COMBOBOX    ((PSZ)0xffff0002L)
#define WC_BUTTON      ((PSZ)0xffff0003L)
#define WC_MENU        ((PSZ)0xffff0004L)
#define WC_STATIC      ((PSZ)0xffff0005L)
#define WC_ENTRYFIELD  ((PSZ)0xffff0006L)
#define WC_LISTBOX     ((PSZ)0xffff0007L)
#define WC_SCROLLBAR   ((PSZ)0xffff0008L)
#define WC_TITLEBAR    ((PSZ)0xffff0009L)
#define WC_MLE         ((PSZ)0xffff000AL)

/* Buttons */
#define BS_PUSHBUTTON     0L
#define BS_CHECKBOX       1L
#define BS_AUTOCHECKBOX   2L
#define BS_RADIOBUTTON    3L
#define BS_AUTORADIOBUTTON 4L
#define BS_3STATE         5L
#define BS_AUTO3STATE     6L
#define BS_USERBUTTON     7L
#define BS_HELP           0x0100L
#define BS_SYSCOMMAND     0x0200L
#define BS_DEFAULT        0x0400L
#define BS_NOPOINTERFOCUS 0x0800L
#define BS_NOBORDER       0x1000L
#define BS_NOCURSORSELECT 0x2000L
#define BM_CLICK          0x0120
#define BM_QUERYHILITE    0x0122
#define BM_SETHILITE      0x0123
#define BM_QUERYCHECKINDEX 0x0121
#define BM_QUERYCHECK     0x0124
#define BM_SETCHECK       0x0125
#define BM_SETDEFAULT     0x0126
#define BN_CLICKED        1
#define BN_DBLCLICKED     2
#define BN_PAINT          3

/* Static controls */
#define SS_TEXT          0x0001L
#define SS_GROUPBOX      0x0002L
#define SS_ICON          0x0003L
#define SS_BITMAP        0x0004L
#define SS_FGNDRECT      0x0005L
#define SS_HALFTONERECT  0x0006L
#define SS_BKGNDRECT     0x0007L
#define SS_FGNDFRAME     0x0008L
#define SS_HALFTONEFRAME 0x0009L
#define SS_BKGNDFRAME    0x000aL
#define SS_SYSICON       0x000bL
#define SM_SETHANDLE     0x0100
#define SM_QUERYHANDLE   0x0101

/* Entry fields */
#define ES_LEFT       0x00000000L
#define ES_CENTER     0x00000001L
#define ES_RIGHT      0x00000002L
#define ES_AUTOSCROLL 0x00000004L
#define ES_MARGIN     0x00000008L
#define ES_AUTOTAB    0x00000010L
#define ES_READONLY   0x00000020L
#define ES_COMMAND    0x00000040L
#define ES_UNREADABLE 0x00000080L
#define ES_ANY        0x00000000L
#define ES_SBCS       0x00001000L
#define ES_DBCS       0x00002000L
#define ES_MIXED      0x00003000L
#define EM_QUERYCHANGED    0x0140
#define EM_QUERYSEL        0x0141
#define EM_SETSEL          0x0142
#define EM_SETTEXTLIMIT    0x0143
#define EM_CUT             0x0144
#define EM_COPY            0x0145
#define EM_CLEAR           0x0146
#define EM_PASTE           0x0147
#define EM_QUERYFIRSTCHAR  0x0148
#define EM_SETFIRSTCHAR    0x0149
#define EM_QUERYREADONLY   0x014a
#define EM_SETREADONLY     0x014b
#define EM_SETINSERTMODE   0x014c
#define EN_SETFOCUS        0x0001
#define EN_KILLFOCUS       0x0002
#define EN_CHANGE          0x0004
#define EN_SCROLL          0x0008
#define EN_MEMERROR        0x0010
#define EN_OVERFLOW        0x0020
#define EN_INSERTMODETOGGLE 0x0040

/* List boxes */
#define LS_MULTIPLESEL  0x00000001L
#define LS_OWNERDRAW    0x00000002L
#define LS_NOADJUSTPOS  0x00000004L
#define LS_HORZSCROLL   0x00000008L
#define LIT_ERROR         (-3)
#define LIT_MEMERROR      (-2)
#define LIT_NONE          (-1)
#define LIT_FIRST         (-1)
#define LIT_END           (-1)
#define LIT_SORTASCENDING (-2)
#define LIT_SORTDESCENDING (-3)
#define LSS_SUBSTRING     0x0001
#define LSS_PREFIX        0x0002
#define LSS_CASESENSITIVE 0x0004
#define LM_QUERYITEMCOUNT      0x0160
#define LM_INSERTITEM          0x0161
#define LM_SETTOPINDEX         0x0162
#define LM_DELETEITEM          0x0163
#define LM_SELECTITEM          0x0164
#define LM_QUERYSELECTION      0x0165
#define LM_SETITEMTEXT         0x0166
#define LM_QUERYITEMTEXTLENGTH 0x0167
#define LM_QUERYITEMTEXT       0x0168
#define LM_SETITEMHANDLE       0x0169
#define LM_QUERYITEMHANDLE     0x016a
#define LM_SEARCHSTRING        0x016b
#define LM_SETITEMHEIGHT       0x016c
#define LM_QUERYTOPINDEX       0x016d
#define LM_DELETEALL           0x016e
#define LN_SELECT     1
#define LN_SETFOCUS   2
#define LN_KILLFOCUS  3
#define LN_SCROLL     4
#define LN_ENTER      5
#define WinQueryLboxCount(hwndLbox) \
    ((LONG)WinSendMsg(hwndLbox, LM_QUERYITEMCOUNT, (MPARAM)NULL, (MPARAM)NULL))
#define WinInsertLboxItem(hwndLbox, index, psz) \
    ((LONG)WinSendMsg(hwndLbox, LM_INSERTITEM, MPFROMLONG(index), MPFROMP(psz)))
#define WinDeleteLboxItem(hwndLbox, index) \
    ((LONG)WinSendMsg(hwndLbox, LM_DELETEITEM, MPFROMLONG(index), (MPARAM)NULL))
#define WinQueryLboxSelectedItem(hwndLbox) \
    ((LONG)WinSendMsg(hwndLbox, LM_QUERYSELECTION, MPFROMLONG(LIT_FIRST), \
                      (MPARAM)NULL))
#define WinSetLboxItemText(hwndLbox, index, psz) \
    ((BOOL)WinSendMsg(hwndLbox, LM_SETITEMTEXT, MPFROMLONG(index), MPFROMP(psz)))
#define WinQueryLboxItemText(hwndLbox, index, psz, cchMax) \
    ((LONG)WinSendMsg(hwndLbox, LM_QUERYITEMTEXT, MPFROM2SHORT(index, cchMax), \
                      MPFROMP(psz)))
#define WinQueryLboxItemTextLength(hwndLbox, index) \
    ((SHORT)WinSendMsg(hwndLbox, LM_QUERYITEMTEXTLENGTH, MPFROMSHORT(index), \
                       (MPARAM)NULL))

/* Combination boxes */
#define CBS_SIMPLE       0x0001L
#define CBS_DROPDOWN     0x0002L
#define CBS_DROPDOWNLIST 0x0004L
#define CBM_SHOWLIST      0x0170
#define CBM_HILITE        0x0171
#define CBM_ISLISTSHOWING 0x0172
#define CBN_EFCHANGE  1
#define CBN_EFSCROLL  2
#define CBN_MEMERROR  3
#define CBN_LBSELECT  4
#define CBN_LBSCROLL  5
#define CBN_SHOWLIST  6
#define CBN_ENTER     7

/* Scroll bars */
#define SBS_HORZ       0L
#define SBS_VERT       1L
#define SBS_THUMBSIZE  2L
#define SBS_AUTOTRACK  4L
#define SBM_SETSCROLLBAR 0x01a0
#define SBM_SETPOS       0x01a1
#define SBM_QUERYPOS     0x01a2
#define SBM_QUERYRANGE   0x01a3
#define SBM_SETTHUMBSIZE 0x01a6
#define SB_LINEUP        1
#define SB_LINEDOWN      2
#define SB_LINELEFT      1
#define SB_LINERIGHT     2
#define SB_PAGEUP        3
#define SB_PAGEDOWN      4
#define SB_PAGELEFT      3
#define SB_PAGERIGHT     4
#define SB_SLIDERTRACK   5
#define SB_SLIDERPOSITION 6
#define SB_ENDSCROLL     7

/* Multi-line entry fields */
typedef LONG IPT, FAR *PIPT;
#define MLS_WORDWRAP     0x00000001L
#define MLS_BORDER       0x00000002L
#define MLS_VSCROLL      0x00000004L
#define MLS_HSCROLL      0x00000008L
#define MLS_READONLY     0x00000010L
#define MLS_IGNORETAB    0x00000020L
#define MLFIE_CFTEXT     0
#define MLFIE_NOTRANS    1
#define MLFIE_WINFMT     2
#define MLM_SETTEXTLIMIT     0x01b0
#define MLM_QUERYTEXTLIMIT   0x01b1
#define MLM_SETFORMATRECT    0x01b2
#define MLM_QUERYFORMATRECT  0x01b3
#define MLM_SETWRAP          0x01b4
#define MLM_QUERYWRAP        0x01b5
#define MLM_SETTABSTOP       0x01b6
#define MLM_QUERYTABSTOP     0x01b7
#define MLM_SETREADONLY      0x01b8
#define MLM_QUERYREADONLY    0x01b9
#define MLM_QUERYCHANGED     0x01ba
#define MLM_SETCHANGED       0x01bb
#define MLM_QUERYLINECOUNT   0x01bc
#define MLM_CHARFROMLINE     0x01bd
#define MLM_LINEFROMCHAR     0x01be
#define MLM_QUERYLINELENGTH  0x01bf
#define MLM_QUERYTEXTLENGTH  0x01c0
#define MLM_FORMAT           0x01c1
#define MLM_SETIMPORTEXPORT  0x01c2
#define MLM_IMPORT           0x01c3
#define MLM_EXPORT           0x01c4
#define MLM_DELETE           0x01c6
#define MLM_QUERYFORMATLINELENGTH 0x01c7
#define MLM_QUERYFORMATTEXTLENGTH 0x01c8
#define MLM_INSERT           0x01c9
#define MLM_SETSEL           0x01ca
#define MLM_QUERYSEL         0x01cb
#define MLM_QUERYSELTEXT     0x01cc
#define MLM_QUERYUNDO        0x01cd
#define MLM_UNDO             0x01ce
#define MLM_RESETUNDO        0x01cf
#define MLM_QUERYFONT        0x01d0
#define MLM_SETFONT          0x01d1
#define MLM_SETTEXTCOLOR     0x01d2
#define MLM_QUERYTEXTCOLOR   0x01d3
#define MLM_SETBACKCOLOR     0x01d4
#define MLM_QUERYBACKCOLOR   0x01d5
#define MLM_QUERYFIRSTCHAR   0x01d6
#define MLM_SETFIRSTCHAR     0x01d7
#define MLM_CUT              0x01d8
#define MLM_COPY             0x01d9
#define MLM_PASTE            0x01da
#define MLM_CLEAR            0x01db
#define MLM_ENABLEREFRESH    0x01dc
#define MLM_DISABLEREFRESH   0x01dd
#define MLM_SEARCH           0x01de
#define MLM_QUERYIMPORTEXPORT 0x01df
#define MLN_OVERFLOW         0x0001
#define MLN_PIXHORZOVERFLOW  0x0002
#define MLN_PIXVERTOVERFLOW  0x0003
#define MLN_TEXTOVERFLOW     0x0004
#define MLN_VSCROLL          0x0005
#define MLN_HSCROLL          0x0006
#define MLN_CHANGE           0x0007
#define MLN_SETFOCUS         0x0008
#define MLN_KILLFOCUS        0x0009
#define MLN_MARGIN           0x000a
#define MLN_SEARCHPAUSE      0x000b
#define MLN_MEMERROR         0x000c
#define MLN_UNDOOVERFLOW     0x000d
#define MLN_CLPBDFAIL        0x000f


/* The clipboard */
#define CF_TEXT        1
#define CF_BITMAP      2
#define CF_DSPTEXT     3
#define CF_DSPBITMAP   4
#define CF_METAFILE    5
#define CF_DSPMETAFILE 6
#define CFI_OWNERFREE    0x0001
#define CFI_OWNERDISPLAY 0x0002
#define CFI_HANDLE       0x0200
BOOL  APIENTRY WinOpenClipbrd(HAB hab);
BOOL  APIENTRY WinCloseClipbrd(HAB hab);
BOOL  APIENTRY WinEmptyClipbrd(HAB hab);
BOOL    APIENTRY WinSetClipbrdData(HAB hab, ULONG ulData, USHORT fmt, USHORT rgfFmtInfo);
ULONG   APIENTRY WinQueryClipbrdData(HAB hab, USHORT fmt);
BOOL    APIENTRY WinQueryClipbrdFmtInfo(HAB hab, USHORT fmt, PUSHORT prgfFmtInfo);
USHORT  APIENTRY WinEnumClipbrdFmts(HAB hab, USHORT fmt);
BOOL  APIENTRY WinSetClipbrdOwner(HAB hab, HWND hwnd);
HWND    APIENTRY WinQueryClipbrdOwner(HAB hab, BOOL fLock);
BOOL  APIENTRY WinSetClipbrdViewer(HAB hab, HWND hwndNewClipViewer);
HWND    APIENTRY WinQueryClipbrdViewer(HAB hab, BOOL fLock);

#ifdef __cplusplus
}
#endif

#endif /* __PMWIN_H */
