#!/usr/bin/env python

from __future__ import print_function
import argparse
import os
import subprocess
import signal
import sys
import socket



#make sure that they have a netid...

base_command = "QEMU_SET_ENV=NETID={key} QEMU_STACK_SIZE=200000000000 qemu-riscv32 {debug} {target} {offset}"

def signal_handler(sig, frame):
    return
signal.signal(signal.SIGINT, signal_handler)

def get_ports(filename):
    with open(filename, 'r') as f:
        return map(lambda x: int(x.strip()), f.readlines())


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("target", help="path to the RISC-V ELF binary to simulate")
    parser.add_argument("-d", "--debug", action="store_true", help="enable remote debugging")
    parser.add_argument("-k", "--key", help="key to use for stack randomization (defaults to netid)")
    parser.add_argument("-o", "--offset", type=int, help="number of bytes to offset stack by (applies after stack randomization)", default=0)

    args = parser.parse_args()    
    
    netid = os.getenv("NETID")
    if not netid:
        print("\033[91mThe NETID environment variable is not set, please make sure setup.py ran correctly and you restarted your terminal\033[0m")
        exit(1)
    if args.key != None:
        random_key = args.key
    else:
        random_key = netid

    debug_str = ""
    if args.debug:
        ports = get_ports(os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(__file__))), netid + "_ports.txt", netid+"_ports.txt"))
        for port in ports:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            try:
                s.bind(("localhost", port))
                s.close()
                debug_str = "-g {}".format(port)
                break
            except socket.error as e:
                print("Port \033[91m{}\033[0m already in use, trying another".format(port), file=sys.stderr)

        else: # we did not find a port
            print("\033[91mAll ports are in use, please clean up any other running processes and try again\033[0m", file=sys.stderr)
            exit(1)
                
    if args.debug:
        print("Listening for GDB connection on port \033[92m{}\033[0m".format(port), file=sys.stderr)
    command = base_command.format(key=random_key, debug=debug_str, target=args.target, offset='a' * args.offset)

    #try:
    p = subprocess.Popen(command, shell=True)
    p.wait()
    #except KeyboardInterrupt as e:
        #os.kill(p.pid, signal.SIGINT)
        #p.wait()
