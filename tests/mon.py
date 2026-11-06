# mon.py SOCKET COMMAND... : send commands to a QEMU monitor socket
import socket, sys, time
s = socket.socket(socket.AF_UNIX)
s.connect(sys.argv[1])
time.sleep(0.3)
s.recv(65536)
for c in sys.argv[2:]:
    s.sendall((c + '\n').encode())
    time.sleep(0.5)
    try:
        s.recv(65536)
    except Exception:
        pass
