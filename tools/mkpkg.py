#!/usr/bin/env python3
"""Stage the NoClick release drawer.

    m68k-amigaos-gcc -O2 -noixemul -Wall -fomit-frame-pointer -s \
        -o out/NoClick src/noclick.c
    python3 tools/mkpkg.py

Produces out/release/NoClick/ (the program + README.md; a CLI tool, so no
program icon) and out/release/NoClick.info (the drawer icon) beside it, so
the drawer shows on Workbench. Archive it with LhA to keep the protection
bits.
"""
import os
import shutil
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
REL = os.path.join(ROOT, "out", "release")

# (source relative to the repo root, path inside out/release)
FILES = [
    ("out/NoClick",               "NoClick/NoClick"),
    ("README.md",                 "NoClick/README.md"),
    ("icons/NoClick-drawer.info", "NoClick.info"),
]


def main():
    missing = [s for s, _ in FILES if not os.path.isfile(os.path.join(ROOT, s))]
    if missing:
        sys.exit("missing (build out/NoClick first?): " + ", ".join(missing))
    if os.path.isdir(REL):
        shutil.rmtree(REL)
    for src, dst in FILES:
        d = os.path.join(REL, dst)
        os.makedirs(os.path.dirname(d), exist_ok=True)
        shutil.copyfile(os.path.join(ROOT, src), d)
    for dirpath, _, names in os.walk(REL):
        for n in sorted(names):
            p = os.path.join(dirpath, n)
            print("%7d  %s" % (os.path.getsize(p), os.path.relpath(p, REL)))


if __name__ == "__main__":
    main()
