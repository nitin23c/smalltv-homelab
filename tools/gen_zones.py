#!/usr/bin/env python3
"""Refresh the time zone table inside web/app.js from the system tzdata.

The device cannot convert zone names (America/Chicago) into POSIX rules
(CST6CDT,M3.2.0,M11.1.0) itself, so the web page carries this table and sends
the matching rule. Re-run after tzdata updates:
    python3 tools/gen_zones.py web/app.js
"""
import json
import os
import re
import sys

ROOT = "/usr/share/zoneinfo"


def posix_rule(name):
    with open(os.path.join(ROOT, name), "rb") as f:
        data = f.read()
    lines = data.rstrip(b"\n").split(b"\n")
    return lines[-1].decode() if lines else ""


def main():
    zones = set()
    with open(os.path.join(ROOT, "zone1970.tab")) as f:
        for line in f:
            if line.startswith("#"):
                continue
            parts = line.rstrip("\n").split("\t")
            if len(parts) >= 3:
                zones.add(parts[2])
    zones.add("UTC")
    out = {}
    for z in sorted(zones):
        rule = posix_rule(z)
        if rule:
            out[z] = rule
    table = json.dumps(out, separators=(",", ":"))
    path = sys.argv[1]
    src = open(path).read()
    new, n = re.subn(r"/\* ZONES-BEGIN \*/.*?/\* ZONES-END \*/",
                     lambda m: f"/* ZONES-BEGIN */const ZONES = {table};/* ZONES-END */",
                     src, flags=re.S)
    if n != 1:
        sys.exit("ZONES markers not found")
    open(path, "w").write(new)
    print(f"{len(out)} zones written to {path}")


if __name__ == "__main__":
    main()
