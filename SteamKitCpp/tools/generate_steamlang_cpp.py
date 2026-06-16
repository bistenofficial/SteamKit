#!/usr/bin/env python3
"""Generate C++ enum headers from SteamLanguage .steamd files.

This is the first native codegen entrypoint for SteamKitCpp. It intentionally
starts with enums because they are the stable dependency of the core SDK types;
structured message class generation will build on the same parser boundary.
"""

from __future__ import annotations

import argparse
import re
from pathlib import Path


ENUM_START_RE = re.compile(r"^\s*enum\s+(?P<name>[A-Za-z_][A-Za-z0-9_]*)\s*$")
VALUE_RE = re.compile(r"^\s*(?P<name>[A-Za-z_][A-Za-z0-9_]*)\s*=\s*(?P<value>[^;]+);")
NAMESPACE_RE = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*(::[A-Za-z_][A-Za-z0-9_]*)*$")


def parse_enums(path: Path) -> list[tuple[str, list[tuple[str, str]]]]:
    enums: list[tuple[str, list[tuple[str, str]]]] = []
    current_name: str | None = None
    current_values: list[tuple[str, str]] = []

    for raw_line in path.read_text(encoding="utf-8").splitlines():
        line = raw_line.strip()
        if not line or line.startswith("#import"):
            continue

        if current_name is None:
            match = ENUM_START_RE.match(line)
            if match:
                current_name = match.group("name")
                current_values = []
            continue

        if line == "{":
            continue

        if line.startswith("}"):
            enums.append((current_name, current_values))
            current_name = None
            current_values = []
            continue

        match = VALUE_RE.match(line)
        if match:
            current_values.append((match.group("name"), match.group("value").strip()))

    return enums


def emit_header(enums: list[tuple[str, list[tuple[str, str]]]], namespace: str) -> str:
    if not NAMESPACE_RE.match(namespace):
        raise ValueError(f"invalid C++ namespace: {namespace}")

    lines = [
        "#pragma once",
        "",
        "#include <cstdint>",
        "",
        f"namespace {namespace}",
        "{",
    ]

    for enum_name, values in enums:
        lines.append(f"enum class {enum_name} : std::uint32_t")
        lines.append("{")
        for name, value in values:
            lines.append(f"    {name} = {value},")
        lines.append("};")
        lines.append("")

    lines.append(f"}} // namespace {namespace}")
    lines.append("")
    return "\n".join(lines)


def main() -> int:
    parser = argparse.ArgumentParser(description="Generate SteamKitCpp enum headers from .steamd files.")
    parser.add_argument("inputs", nargs="+", type=Path, help="Input .steamd files")
    parser.add_argument("--output", required=True, type=Path, help="Output C++ header")
    parser.add_argument(
        "--namespace",
        default="steamkit::generated",
        help="C++ namespace for generated declarations, default: steamkit::generated",
    )
    args = parser.parse_args()

    enums: list[tuple[str, list[tuple[str, str]]]] = []
    for input_path in args.inputs:
        enums.extend(parse_enums(input_path))

    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(emit_header(enums, args.namespace), encoding="utf-8", newline="\n")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
