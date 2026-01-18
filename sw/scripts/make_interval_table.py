#!/usr/bin/env python
from argparse import ArgumentParser
import math


def chunks(lst, n):
    for i in range(0, len(lst), n):
        yield lst[i:i + n]


def generate_table(args):
    A = 1000
    B = 0.001
    C = 10
    N = int(args.number_of_steps)
    T = int(args.total_time_msecs)
    return [int(1000 * A * math.exp(-B * i * (T / N)) + C) for i in range(N)]


def generate_file(args):
    with open(vars(args)['output_file'], 'w+') as f:
        lut = generate_table(args)
        f.write('#pragma once\n#include <avr/pgmspace.h>\nstatic const long LUT[] PROGMEM = {\n')
        f.write(',\n'.join([','.join([f'{a:8d}' for a in chunk]) for chunk in chunks(lut, 8)]))
        f.write('\n};\n')


if __name__ == "__main__":
    parser = ArgumentParser()
    parser.add_argument('-T', '--total-time-msecs', default=10000)
    parser.add_argument('-N', '--number-of-steps', default=100)
    parser.add_argument('-o', '--output-file', default='include/lut.h')
    args = parser.parse_args()
    generate_file(args)
