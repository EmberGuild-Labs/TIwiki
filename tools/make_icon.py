#!/usr/bin/env python3
"""Generate the 16x16 icon.png shown next to the program in a shell.

The icon is an open book: white pages, blue-grey spine, accent-blue text
lines, on the same dark navy used by the app's header bar.
"""

import os
import struct
import zlib

PALETTE = {
    "B": (28, 42, 84),     # navy border / background
    "W": (250, 250, 247),  # page
    "S": (18, 96, 190),    # spine
    "a": (120, 160, 210),  # lines of "text" on the page
}

ART = [
    "BBBBBBBBBBBBBBBB",
    "BWWWWWWSSWWWWWWB",
    "BWaaaaWSSWaaaaWB",
    "BWWWWWWSSWWWWWWB",
    "BWaaaaWSSWaaaaWB",
    "BWWWWWWSSWWWWWWB",
    "BWaaaaWSSWaaaaWB",
    "BWWWWWWSSWWWWWWB",
    "BWaaaaWSSWaaaaWB",
    "BWWWWWWSSWWWWWWB",
    "BWaaaaWSSWaaaaWB",
    "BWWWWWWSSWWWWWWB",
    "BWaaaWWSSWWaaaWB",
    "BWWWWWWSSWWWWWWB",
    "BWWWWWWSSWWWWWWB",
    "BBBBBBBBBBBBBBBB",
]


def chunk(tag, data):
    return (struct.pack(">I", len(data)) + tag + data +
            struct.pack(">I", zlib.crc32(tag + data) & 0xFFFFFFFF))


def main():
    assert len(ART) == 16 and all(len(r) == 16 for r in ART), "icon must be 16x16"

    raw = b""
    for row in ART:
        raw += b"\x00"  # filter type: none
        for c in row:
            raw += bytes(PALETTE[c])

    png = (b"\x89PNG\r\n\x1a\n" +
           chunk(b"IHDR", struct.pack(">IIBBBBB", 16, 16, 8, 2, 0, 0, 0)) +
           chunk(b"IDAT", zlib.compress(raw, 9)) +
           chunk(b"IEND", b""))

    out = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
                       "icon.png")
    with open(out, "wb") as f:
        f.write(png)
    print("wrote %s (%d bytes)" % (out, len(png)))


if __name__ == "__main__":
    main()
