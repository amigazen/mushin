#!/usr/bin/env python3
"""Generate the package's original, classic two-plane Workbench icons."""
import struct
from pathlib import Path

DIST = Path(__file__).resolve().parents[1] / 'dist'


def icon(drawer=False):
    width, height = 32, 24
    pixels = [[0] * width for _ in range(height)]
    for y in range(7, 22):
        for x in range(2, 30):
            pixels[y][x] = 2 if y == 7 or x == 2 else 1
    for y in range(4, 7):
        for x in range(3, 14):
            pixels[y][x] = 2
    if not drawer:
        for y in range(1, 13):
            for x in range(14, 19):
                pixels[y][x] = 3
        for y in range(11, 18):
            for x in range(9 + y - 11, 24 - (y - 11)):
                pixels[y][x] = 3
    planes = bytearray()
    for plane in range(2):
        for row in pixels:
            bits = sum(((value >> plane) & 1) << (31 - x)
                       for x, value in enumerate(row))
            planes.extend(struct.pack('>I', bits))

    # DiskObject and Gadget fields use the Amiga's two-byte alignment.
    header = bytearray(78)
    struct.pack_into('>HH', header, 0, 0xe310, 1)
    struct.pack_into('>HHH', header, 12, width, height, 4)
    struct.pack_into('>I', header, 22, 1)  # GadgetRender is present
    header[48] = 2 if drawer else 4       # WBDRAWER / WBPROJECT
    struct.pack_into('>II', header, 50, 0 if drawer else 1,
                     0 if drawer else 1)  # DefaultTool / ToolTypes
    struct.pack_into('>ii', header, 58, -2147483648, -2147483648)
    struct.pack_into('>I', header, 66, 1 if drawer else 0)
    struct.pack_into('>I', header, 74, 65536)
    data = bytearray(header)
    if drawer:
        old_drawer = bytearray(56)  # NewWindow plus dd_CurrentX/Y
        struct.pack_into('>hhhhBBII', old_drawer, 0,
                         50, 50, 450, 180, 0, 1, 0, 0x02000f)
        struct.pack_into('>HHHHH', old_drawer, 38, 100, 60, 65535, 65535, 1)
        data.extend(old_drawer)
    data.extend(struct.pack('>hhhhhIBBI', 0, 0, width, height, 2, 1, 3, 0, 0))
    data.extend(planes)

    def string(value):
        encoded = value.encode('ascii') + b'\0'
        return struct.pack('>I', len(encoded)) + encoded

    if not drawer:
        data.extend(string('SYS:System/Installer'))
        tooltypes = ['APPNAME=Mushin', 'MINUSER=AVERAGE', 'DEFUSER=AVERAGE']
        data.extend(struct.pack('>I', 4 * (len(tooltypes) + 1)))
        for value in tooltypes:
            data.extend(string(value))
    return data


if __name__ == '__main__':
    (DIST / 'Install.info').write_bytes(icon())
    (DIST / 'Mushin.info').write_bytes(icon(drawer=True))
