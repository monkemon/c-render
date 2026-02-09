import sys
import tty
import termios

fd = sys.stdin.fileno()
old = termios.tcgetattr(fd)
try:
    tty.setraw(fd)
    while True:
        ch = sys.stdin.read(1)
        if ord(ch) == 27:
            exit()
        print("char: [%s]\t\tint: [%d]\t\tbin: [%s]" % (str(ch), ord(ch), bin(ord(ch))), end="\r\n", flush=True)
except KeyboardInterrupt:
    pass
finally:
    termios.tcsetattr(fd, termios.TCSADRAIN, old)