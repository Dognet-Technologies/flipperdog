#!/usr/bin/env python3
"""Query Flipper Zero CLI over raw USB-CDC serial (no pyserial dependency).

USB-CDC ignora il baudrate; serve solo mettere la tty in raw mode (no echo,
no canonical) per non corrompere il protocollo testuale della CLI del Flipper.
"""
import os
import sys
import time
import select
import termios

DEV = sys.argv[1] if len(sys.argv) > 1 else "/dev/ttyACM0"


def open_raw(dev):
    fd = os.open(dev, os.O_RDWR | os.O_NOCTTY | os.O_NONBLOCK)
    a = termios.tcgetattr(fd)
    # iflag, oflag, lflag -> 0 = raw; disabilita echo/canonical/flow control
    a[0] = 0
    a[1] = 0
    a[3] = 0
    a[6][termios.VMIN] = 0
    a[6][termios.VTIME] = 0
    termios.tcsetattr(fd, termios.TCSANOW, a)
    return fd


def drain(fd, idle=0.4, hard=2.0):
    """Legge finche' il device tace per `idle` secondi o fino a `hard` totali."""
    buf = b""
    start = time.time()
    last = start
    while time.time() - start < hard:
        r, _, _ = select.select([fd], [], [], 0.1)
        if r:
            try:
                data = os.read(fd, 4096)
            except OSError:
                data = b""
            if data:
                buf += data
                last = time.time()
                continue
        if time.time() - last >= idle:
            break
    return buf


def send(fd, cmd):
    os.write(fd, cmd.encode() + b"\r\n")
    time.sleep(0.2)
    return drain(fd).decode("utf-8", "replace")


def main():
    fd = open_raw(DEV)
    try:
        drain(fd, hard=1.0)          # scarta banner iniziale
        send(fd, "")                  # forza prompt pulito
        for cmd in ("info device", "power info"):
            print(f"\n===== >: {cmd} =====")
            print(send(fd, cmd).strip())
    finally:
        os.close(fd)


if __name__ == "__main__":
    main()
