/* A Presentation Manager program whose menu, accelerators, dialogs and
 * strings are resources (pmnotes.rc):
 *   clang --target=i386-pc-os2 -mwindows -Os pmnotes.c -o pmnotes.exe
 *   python3 ../tools/os2rc.py -I ../include/os2v2 pmnotes.rc pmnotes.exe
 */
#include <os2.h>
#include <stdio.h>
#include "pmnotes.h"

static char name[64];

static MRESULT EXPENTRY
name_proc(HWND dlg, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg) {
    case WM_INITDLG:
        WinSetDlgItemText(dlg, IDC_NAME, name);
        return (MRESULT)FALSE;
    case WM_COMMAND:
        if (SHORT1FROMMP(mp1) == DID_OK)
            WinQueryDlgItemText(dlg, IDC_NAME, sizeof name, name);
        WinDismissDlg(dlg, SHORT1FROMMP(mp1));
        return 0;
    }
    return WinDefDlgProc(dlg, msg, mp1, mp2);
}

static MRESULT EXPENTRY
client_proc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    HAB hab = WinQueryAnchorBlock(hwnd);

    switch (msg) {
    case WM_PAINT: {
        char  fmt[64], text[128];
        RECTL rc;
        HPS   hps = WinBeginPaint(hwnd, NULLHANDLE, NULL);
        if (name[0]) {
            WinLoadString(hab, NULLHANDLE, IDS_GREETING, sizeof fmt, fmt);
            snprintf(text, sizeof text, fmt, name);
        } else {
            WinLoadString(hab, NULLHANDLE, IDS_NONAME, sizeof text, text);
        }
        WinQueryWindowRect(hwnd, &rc);
        WinFillRect(hps, &rc, CLR_WHITE);
        WinDrawText(hps, -1, text, &rc, CLR_DARKBLUE, CLR_WHITE,
                    DT_CENTER | DT_VCENTER);
        WinEndPaint(hps);
        return 0;
    }
    case WM_COMMAND:
        switch (SHORT1FROMMP(mp1)) {
        case IDM_NAME:
            if (WinDlgBox(HWND_DESKTOP, hwnd, name_proc, NULLHANDLE, IDD_NAME,
                          NULL) == DID_OK)
                WinInvalidateRect(hwnd, NULL, FALSE);
            break;
        case IDM_ABOUT:
            WinDlgBox(HWND_DESKTOP, hwnd, WinDefDlgProc, NULLHANDLE, IDD_ABOUT,
                      NULL);
            break;
        case IDM_EXIT:
            WinPostMsg(hwnd, WM_CLOSE, 0, 0);
            break;
        }
        return 0;
    }
    return WinDefWindowProc(hwnd, msg, mp1, mp2);
}

int
main(void)
{
    HAB   hab = WinInitialize(0);
    HMQ   hmq = WinCreateMsgQueue(hab, 0);
    ULONG flags = FCF_STANDARD & ~FCF_ICON;
    HWND  client;
    QMSG  qmsg;

    WinRegisterClass(hab, "PmNotes", client_proc, CS_SIZEREDRAW, 0);
    HWND frame = WinCreateStdWindow(HWND_DESKTOP, WS_VISIBLE, &flags, "PmNotes",
                                    "pmnotes", 0, NULLHANDLE, ID_MAIN, &client);
    while (WinGetMsg(hab, &qmsg, NULLHANDLE, 0, 0))
        WinDispatchMsg(hab, &qmsg);
    WinDestroyWindow(frame);
    WinDestroyMsgQueue(hmq);
    WinTerminate(hab);
    return 0;
}
