import os
import subprocess
import signal
import sys
import socket

if __name__ == "__main__":
    print("Killing all simulate processes...")
    subprocess.call("killall -9 qemu-riscv32", shell=True)
    print("Done.")

