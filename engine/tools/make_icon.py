#!/usr/bin/env python3
"""Generate a wiki's 16x16 icon.png, shown next to the program in a shell.

    python3 engine/tools/make_icon.py mathwiki

Each wiki gets an open book in its own accent colour, matching that app's
header bar.
"""

import os
import struct
import sys
import zlib

# project -> (border/background, page, spine, text lines)
THEMES = {
    "mathwiki":    ((28, 42, 84), (250, 250, 247), (18, 96, 190), (120, 160, 210)),
    "physicswiki": ((26, 60, 54), (250, 250, 247), (10, 140, 110), (120, 200, 175)),
    "chemwiki":    ((72, 34, 72), (250, 250, 247), (150, 45, 130), (205, 150, 200)),
}
DEFAULT_THEME = THEMES["mathwiki"]

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

    project = sys.argv[1] if len(sys.argv) > 1 else "mathwiki"
    theme = THEMES.get(os.path.basename(os.path.normpath(project)), DEFAULT_THEME)
    palette = dict(zip("BWSa", theme))

    raw = b""
    for row in ART:
        raw += b"\x00"  # filter type: none
        for c in row:
            raw += bytes(palette[c])

    png = (b"\x89PNG\r\n\x1a\n" +
           chunk(b"IHDR", struct.pack(">IIBBBBB", 16, 16, 8, 2, 0, 0, 0)) +
           chunk(b"IDAT", zlib.compress(raw, 9)) +
           chunk(b"IEND", b""))

    repo = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    root = project if os.path.isabs(project) else os.path.join(repo, project)
    out = os.path.join(root, "icon.png")
    with open(out, "wb") as f:
        f.write(png)
    print("wrote %s (%d bytes)" % (out, len(png)))


if __name__ == "__main__":
    main()
