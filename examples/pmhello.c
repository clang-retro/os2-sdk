/* A Presentation Manager window for 32-bit OS/2:
 *   clang --target=i386-pc-os2 -mwindows -Os pmhello.c -o pmhello.exe
 */
#include <os2.h>

static MRESULT EXPENTRY
client_proc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg) {
    case WM_PAINT: {
        RECTL rc;
        HPS   hps = WinBeginPaint(hwnd, NULLHANDLE, NULL);
        WinQueryWindowRect(hwnd, &rc);
        WinFillRect(hps, &rc, CLR_WHITE);
        WinDrawText(hps, -1, "Hello from clang on OS/2!", &rc, CLR_DARKBLUE, CLR_WHITE,
                    DT_CENTER | DT_VCENTER);
        WinEndPaint(hps);
        return 0;
    }
    case WM_BUTTON1DOWN:
        WinMessageBox(HWND_DESKTOP, hwnd, "Built with clang-retro.", "pmhello", 0,
                      MB_OK | MB_INFORMATION | MB_MOVEABLE);
        return (MRESULT)TRUE;
    }
    return WinDefWindowProc(hwnd, msg, mp1, mp2);
}

int
main(void)
{
    HAB   hab = WinInitialize(0);
    HMQ   hmq = WinCreateMsgQueue(hab, 0);
    ULONG flags = FCF_TITLEBAR | FCF_SYSMENU | FCF_SIZEBORDER | FCF_MINMAX
        | FCF_SHELLPOSITION | FCF_TASKLIST;
    HWND  client;
    QMSG  qmsg;

    WinRegisterClass(hab, "PmHello", client_proc, CS_SIZEREDRAW, 0);
    HWND frame = WinCreateStdWindow(HWND_DESKTOP, WS_VISIBLE, &flags, "PmHello",
                                    "Hello from clang", 0, NULLHANDLE, 0, &client);
    while (WinGetMsg(hab, &qmsg, NULLHANDLE, 0, 0))
        WinDispatchMsg(hab, &qmsg);
    WinDestroyWindow(frame);
    WinDestroyMsgQueue(hmq);
    WinTerminate(hab);
    return 0;
}
