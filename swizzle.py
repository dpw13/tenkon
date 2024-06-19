#!/usr/bin/env python3

import functools
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

ROM_SIZE = 0x80000
#ROM_SIZE = 0x100

out_bytes = [
    bytearray([255] * ROM_SIZE),
    bytearray([255] * ROM_SIZE),
]

# See MC68030UM Fig 7-4
# HI(32:24) LO(23:16)
# B0        B1
# B2        B3

if len(sys.argv) < 2:
    print(f"Usage: {sys.argv[0]} <romfile.bin>")
    exit(1)

addr = 0
with open(sys.argv[1], "rb") as src:
    for chunk in iter(functools.partial(src.read, 4096), b''):
        for b in chunk:
            rom = addr & 0x01
            bank = ROM_SIZE//2 if (addr & 0x02) else 0
            offset = (addr >> 2) + bank
            #print(f"{addr:02X} -> {rom}.{offset:02X}: {b}")
            out_bytes[rom][offset] = swizzle_byte(b)
            addr += 1

# Yes this indexing is intentional T_T
with open("hi.bin", "wb") as out:
    out.write(out_bytes[0])        
with open("lo.bin", "wb") as out:
    out.write(out_bytes[1])        