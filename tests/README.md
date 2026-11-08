# Testing on OS/2 Warp 3 in QEMU

No install is needed. Warp 3's two boot diskettes boot to a CMD.EXE prompt
once diskette 1's CONFIG.SYS starts `cmd.exe /k b:\run.cmd` instead of the
installer. The programs under test go on a diskette in drive B:, each one's
output goes to B:\NAME.out, and mtools reads it back afterwards.

    ./mkwarp3.sh Disk00.img Disk01.img work   # Blue Spine floppy set
    ./runtests.sh work 60 hello.exe sysinfo.exe

Files given that are not programs are copied to B: too; `dosapi.exe` reads
XDF.MSG, which is on diskette 0:

    mcopy -i Disk00.img ::/XDF.MSG .
    ./runtests.sh work 60 dosapi.exe XDF.MSG

The programs run with B:\ as the current directory, where OS/2 finds
DLLs (LIBPATH starts with `.`). The 16-bit DLL test, in each memory model:

    clang --target=i8086-pc-os2 -mcmodel=small -shared testdll.c -o TESTDLL.DLL
    python3 ../tools/os2-imports.py TESTDLL.DLL > testdll.lnk
    clang --target=i8086-pc-os2 -mcmodel=small dllapp.c -Wl,@testdll.lnk -o DLLAPP.EXE
    ./runtests.sh work 60 DLLAPP.EXE TESTDLL.DLL

Needs qemu-system-i386, mtools and python3. The last screen is saved as
work/screen.ppm.

OS/2 2.11 SMP hangs at "Loading, please wait..." in QEMU and OS/2 1.0 loops
through CPU resets. Warp 3 runs both 16-bit (NE) and 32-bit (LX) programs.
