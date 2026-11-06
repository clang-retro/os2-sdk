#!/bin/sh
# mkwarp3.sh DISK00 DISK01 WORKDIR: prepare Warp 3's boot diskettes to boot
# to a CMD.EXE prompt that runs B:\RUN.CMD.
set -e
work=$3
mkdir -p "$work"
cp "$1" "$work/disk0.img"
cp "$2" "$work/disk1.img"
chmod u+w "$work"/disk*.img
mtype -i "$work/disk1.img" ::/CONFIG.SYS |
  sed 's/^protshell=sysinst1.exe/protshell=cmd.exe \/k b:\\run.cmd/
       s/^set os2_shell=sysinst2.exe/set comspec=a:\\cmd.exe/' > "$work/config.sys"
mcopy -o -i "$work/disk1.img" "$work/config.sys" ::/CONFIG.SYS
echo "boot diskettes ready in $work"
