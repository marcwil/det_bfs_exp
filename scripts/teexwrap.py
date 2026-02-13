#!/bin/python3

import subprocess
from argparse import ArgumentParser
from os.path import basename

def find_line(messages, key):
    res = [l for l in messages if key in l]
    return res[-1]

parser = ArgumentParser()
parser.add_argument("exe_path", help="Path to the teexgraph executable")
parser.add_argument("graph_path", help="Path to the graph")
parser.add_argument("-s", "--samples", help="Number of samples", type=int, default=1)
argv = parser.parse_args()

for i in range(argv.samples):
    proc = subprocess.run([argv.exe_path, argv.graph_path], capture_output=True)

    messages = proc.stderr.decode().splitlines()

    diam = int(find_line(messages, "Diameter value").split()[-1])
    iter = int(find_line(messages, "Iterations").split()[-1])
    time = float(find_line(messages, "Time passed").split()[-2])*1000

    print(f"{basename(argv.graph_path)},{diam},{iter},{time}")
