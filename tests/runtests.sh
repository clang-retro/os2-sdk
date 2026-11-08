#!/bin/sh
# runtests.sh WORKDIR SECONDS PROGRAM.exe... : run each program under OS/2
# Warp 3 in QEMU and print what it wrote. WORKDIR comes from mkwarp3.sh.
# Other files given (not .exe) are copied to B: for the programs to use,
# such as DLLs.
set -e
here=$(cd "$(dirname "$0")" && pwd)
work=$(cd "$1" && pwd)
secs=$2
shift 2
img=$work/tests.img
rm -f "$img" "$work/mon.sock"
mformat -C -i "$img" -f 1440 ::
names=
for f in "$@"; do
  mcopy -i "$img" "$f" ::/
  case $f in
  *.exe) names="$names $(basename "$f" .exe)" ;;
  *.EXE) names="$names $(basename "$f" .EXE)" ;;
  esac
done
# B:\ is the current directory, so that programs find DLLs there (LIBPATH
# starts with ".").
printf '@echo off\r\nb:\r\ncd \\\r\nfor %%%%f in (%s) do call b:\\one %%%%f\r\n' "$names" > "$work/run.cmd"
printf '@echo off\r\nb:\\%%1 > b:\\%%1.out 2>&1\r\n' > "$work/one.cmd"
mcopy -i "$img" "$work/run.cmd" "$work/one.cmd" ::/
qemu-system-i386 -m 32 -cpu pentium -machine pc \
  -drive file="$work/disk0.img",if=floppy,index=0,format=raw \
  -drive file="$img",if=floppy,index=1,format=raw \
  -boot a -display none -vga std \
  -monitor unix:"$work/mon.sock",server,nowait &
pid=$!
# Diskette 0 asks for diskette 1: swap it in and press Enter.
sleep 25
python3 "$here/mon.py" "$work/mon.sock" "change floppy0 $work/disk1.img raw" "sendkey ret"
sleep "$secs"
python3 "$here/mon.py" "$work/mon.sock" "screendump $work/screen.ppm" quit
wait $pid || true
for n in $names; do
  echo "--- $n"
  mtype -i "$img" "::/$n.out" 2>&1 || true
done
