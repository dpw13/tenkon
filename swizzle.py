#!/usr/bin/env python3

import argparse
import functools
import os
import sys

def get_bit(byte, bit) :
    return (byte >> bit) & 1


def set_bit(byte, bit, value) :
    return byte | (value << bit)


def swizzle_byte(byte: int):
    ret_val = byte & 0x07

    ret_val = set_bit(ret_val, 3, get_bit(byte, 7))
    ret_val = set_bit(ret_val, 4, get_bit(byte, 6))
    ret_val = set_bit(ret_val, 5, get_bit(byte, 5))
    ret_val = set_bit(ret_val, 6, get_bit(byte, 4))
    ret_val = set_bit(ret_val, 7, get_bit(byte, 3))

    return ret_val

parser = argparse.ArgumentParser()
parser.add_argument('--rom-size', '-s', help="ROM size", type=int, default=0x80000)
parser.add_argument('--offset', '-o', action='append', type=lambda x: int(x, 0))
parser.add_argument('--binfile', '-f', action='append', type=str)
args = parser.parse_args()

out_bytes = [
    bytearray([255] * args.rom_size),
    bytearray([255] * args.rom_size),
]

# See MC68030UM Fig 7-4
# HI(32:24) LO(23:16)
# B0        B1
# B2        B3

for (addr, path) in zip(args.offset, args.binfile):
    size = os.path.getsize(path)
    print(f"Adding {path} at offset 0x{addr:x}:0x{addr+size:x}")
    with open(path, "rb") as src:
        for chunk in iter(functools.partial(src.read, 4096), b''):
            for b in chunk:
                rom = addr & 0x01
                bank = args.rom_size//2 if (addr & 0x02) else 0
                offset = (addr >> 2) + bank
                #print(f"{addr:02X} -> {rom}.{offset:02X}: {b}")
                out_bytes[rom][offset] = swizzle_byte(b)
                addr += 1

# Yes this indexing is intentional T_T
with open("hi.bin", "wb") as out:
    out.write(out_bytes[0])        
with open("lo.bin", "wb") as out:
    out.write(out_bytes[1])        