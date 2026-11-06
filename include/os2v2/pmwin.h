/*
 * pmwin.h - a part of the 32-bit Presentation Manager API (PMWIN and
 * PMGPI): enough for a window, its messages, painting text and simple
 * graphics, and message boxes. For clang-retro (i386-pc-os2).
 */
#ifndef __PMWIN_H
#define __PMWIN_H

#include <os2def.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef MRESULT (EXPENTRY FNWP)(HWND, ULONG, MPARAM, MPARAM);
typedef FNWP *PFNWP;

typedef struct _QMSG {
    HWND   hwnd;
    ULONG  msg;
    MPARAM mp1;
    MPARAM mp2;
    ULONG  time;
    POINTL ptl;
    ULONG  reserved;
} QMSG, *PQMSG;

/* Anchor blocks and message queues */
HAB     APIENTRY WinInitialize(ULONG flOptions);
BOOL    APIENTRY WinTerminate(HAB hab);
HMQ     APIENTRY WinCreateMsgQueue(HAB hab, LONG cmsg);
BOOL    APIENTRY WinDestroyMsgQueue(HMQ hmq);
BOOL    APIENTRY WinGetMsg(HAB hab, PQMSG pqmsg, HWND hwndFilter, ULONG msgFilterFirst,
                           ULONG msgFilterLast);
BOOL    APIENTRY WinPeekMsg(HAB hab, PQMSG pqmsg, HWND hwndFilter, ULONG msgFilterFirst,
                            ULONG msgFilterLast, ULONG fl);
MRESULT APIENTRY WinDispatchMsg(HAB hab, PQMSG pqmsg);
BOOL    APIENTRY WinPostMsg(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT APIENTRY WinSendMsg(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
BOOL    APIENTRY WinPostQueueMsg(HMQ hmq, ULONG msg, MPARAM mp1, MPARAM mp2);
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
#define WS_ANIMATE      0x00400000L
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
#define FCF_AUTOICON      0x40000000L
#define FCF_STANDARD      0x0000CC3FL

#define FID_SYSMENU    0x8002
#define FID_TITLEBAR   0x8003
#define FID_MINMAX     0x8004
#define FID_MENU       0x8005
#define FID_VERTSCROLL 0x8006
#define FID_HORZSCROLL 0x8007
#define FID_CLIENT     0x8008

BOOL    APIENTRY WinRegisterClass(HAB hab, PCSZ pszClassName, PFNWP pfnWndProc,
                                  ULONG flStyle, ULONG cbWindowData);
HWND    APIENTRY WinCreateStdWindow(HWND hwndParent, ULONG flStyle, PULONG pflCreateFlags,
                                    PCSZ pszClientClass, PCSZ pszTitle, ULONG styleClient,
                                    HMODULE hmod, ULONG idResources, PHWND phwndClient);
HWND    APIENTRY WinCreateWindow(HWND hwndParent, PCSZ pszClass, PCSZ pszName, ULONG flStyle,
                                 LONG x, LONG y, LONG cx, LONG cy, HWND hwndOwner,
                                 HWND hwndInsertBehind, ULONG id, PVOID pCtlData,
                                 PVOID pPresParams);
BOOL    APIENTRY WinDestroyWindow(HWND hwnd);
MRESULT APIENTRY WinDefWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
BOOL    APIENTRY WinShowWindow(HWND hwnd, BOOL fShow);
BOOL    APIENTRY WinSetWindowText(HWND hwnd, PCSZ pszText);
LONG    APIENTRY WinQueryWindowText(HWND hwnd, LONG cchBufferMax, PCH pchBuffer);
HWND    APIENTRY WinQueryWindow(HWND hwnd, LONG cmd);
HWND    APIENTRY WinWindowFromID(HWND hwndParent, ULONG id);
BOOL    APIENTRY WinQueryWindowRect(HWND hwnd, PRECTL prclDest);
BOOL    APIENTRY WinInvalidateRect(HWND hwnd, PRECTL pwrc, BOOL fIncludeChildren);
BOOL    APIENTRY WinUpdateWindow(HWND hwnd);
BOOL    APIENTRY WinSetWindowPos(HWND hwnd, HWND hwndInsertBehind, LONG x, LONG y, LONG cx,
                                 LONG cy, ULONG fl);
BOOL    APIENTRY WinSetWindowULong(HWND hwnd, LONG index, ULONG ul);
ULONG   APIENTRY WinQueryWindowULong(HWND hwnd, LONG index);
BOOL    APIENTRY WinSetWindowPtr(HWND hwnd, LONG index, PVOID p);
PVOID   APIENTRY WinQueryWindowPtr(HWND hwnd, LONG index);
LONG    APIENTRY WinQuerySysValue(HWND hwndDeskTop, LONG iSysValue);
BOOL    APIENTRY WinAlarm(HWND hwndDesktop, ULONG rgfType);
ULONG   APIENTRY WinStartTimer(HAB hab, HWND hwnd, ULONG idTimer, ULONG dtTimeout);
BOOL    APIENTRY WinStopTimer(HAB hab, HWND hwnd, ULONG idTimer);
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
ULONG APIENTRY WinMessageBox(HWND hwndParent, HWND hwndOwner, PCSZ pszText,
                             PCSZ pszCaption, ULONG idWindow, ULONG flStyle);

/* Painting */
HPS   APIENTRY WinBeginPaint(HWND hwnd, HPS hps, PRECTL prclPaint);
BOOL  APIENTRY WinEndPaint(HPS hps);
HPS   APIENTRY WinGetPS(HWND hwnd);
BOOL  APIENTRY WinReleasePS(HPS hps);
BOOL  APIENTRY WinFillRect(HPS hps, PRECTL prcl, LONG lColor);
LONG  APIENTRY WinDrawText(HPS hps, LONG cchText, PCSZ lpchText, PRECTL prcl, LONG clrFore,
                           LONG clrBack, ULONG flCmd);
#define DT_LEFT       0x0000
#define DT_QUERYEXTENT 0x0002
#define DT_UNDERSCORE 0x0010
#define DT_STRIKEOUT  0x0020
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

/* Colors (GPI) */
#define CLR_FALSE      (-5L)
#define CLR_TRUE       (-4L)
#define CLR_ERROR      (-255L)
#define CLR_DEFAULT    (-3L)
#define CLR_WHITE      (-2L)
#define CLR_BLACK      (-1L)
#define CLR_BACKGROUND 0L
#define CLR_BLUE       1L
#define CLR_RED        2L
#define CLR_PINK       3L
#define CLR_GREEN      4L
#define CLR_CYAN       5L
#define CLR_YELLOW     6L
#define CLR_NEUTRAL    7L
#define CLR_DARKGRAY   8L
#define CLR_DARKBLUE   9L
#define CLR_DARKRED    10L
#define CLR_DARKPINK   11L
#define CLR_DARKGREEN  12L
#define CLR_DARKCYAN   13L
#define CLR_BROWN      14L
#define CLR_PALEGRAY   15L

/* GPI */
BOOL APIENTRY GpiSetColor(HPS hps, LONG lColor);
BOOL APIENTRY GpiSetBackColor(HPS hps, LONG lColor);
BOOL APIENTRY GpiSetBackMix(HPS hps, LONG lMixMode);
BOOL APIENTRY GpiMove(HPS hps, PPOINTL pptlPoint);
LONG APIENTRY GpiLine(HPS hps, PPOINTL pptlEndPoint);
LONG APIENTRY GpiBox(HPS hps, LONG lControl, PPOINTL pptlPoint, LONG lHRound, LONG lVRound);
LONG APIENTRY GpiCharString(HPS hps, LONG lCount, PCH pchString);
LONG APIENTRY GpiCharStringAt(HPS hps, PPOINTL pptlPoint, LONG lCount, PCH pchString);
BOOL APIENTRY GpiErase(HPS hps);
#define DRO_FILL        1L
#define DRO_OUTLINE     2L
#define DRO_OUTLINEFILL 3L
#define BM_LEAVEALONE   5L
#define BM_OVERPAINT    2L

#ifdef __cplusplus
}
#endif

#endif /* __PMWIN_H */
