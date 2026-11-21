#!/usr/bin/env python3
"""Tokenize ASCII Commodore BASIC V2 source into a loadable PRG."""

from pathlib import Path
import argparse

KEYWORDS = [
    "END", "FOR", "NEXT", "DATA", "INPUT#", "INPUT", "DIM", "READ", "LET",
    "GOTO", "RUN", "IF", "RESTORE", "GOSUB", "RETURN", "REM", "STOP", "ON",
    "WAIT", "LOAD", "SAVE", "VERIFY", "DEF", "POKE", "PRINT#", "PRINT", "CONT",
    "LIST", "CLR", "CMD", "SYS", "OPEN", "CLOSE", "GET", "NEW", "TAB(", "TO",
    "FN", "SPC(", "THEN", "NOT", "STEP", "+", "-", "*", "/", "^", "AND",
    "OR", ">", "=", "<", "SGN", "INT", "ABS", "USR", "FRE", "POS", "SQR",
    "RND", "LOG", "EXP", "COS", "SIN", "TAN", "ATN", "PEEK", "LEN", "STR$",
    "VAL", "ASC", "CHR$", "LEFT$", "RIGHT$", "MID$", "GO",
]
TOKENS = {word: 0x80 + i for i, word in enumerate(KEYWORDS)}
ORDERED = sorted(TOKENS, key=len, reverse=True)

def petscii_byte(ch: str) -> int:
    code = ord(ch)
    if 0 <= code <= 255:
        return code
    raise ValueError(f"character outside single-byte PETSCII range: {ch!r}")

def tokenize_body(body: str) -> bytes:
    result = bytearray(); i = 0; quoted = False; data_mode = False
    while i < len(body):
        ch = body[i]
        if quoted:
            # The C64 starts in upper/graphics mode.  ASCII lowercase bytes
            # render as graphics characters there, so source text must be
            # converted to uppercase PETSCII just like program identifiers.
            result.append(petscii_byte(ch.upper())); quoted = ch != '"'; i += 1; continue
        if ch == '"':
            quoted = True; result.append(ord(ch)); i += 1; continue
        if data_mode:
            result.append(petscii_byte(ch)); i += 1
            if ch == ":": data_mode = False
            continue
        # BASIC source is conventionally written in either case.  Match the
        # language tokens case-insensitively while preserving quoted PETSCII
        # text and non-keyword source bytes verbatim.
        match = next((word for word in ORDERED if body[i:i + len(word)].upper() == word), None)
        if match is None:
            # Program text (variable names, operators, and punctuation) is
            # entered in the C64's normal upper/graphics character set.
            # Preserve case only inside quoted strings, handled above.
            result.append(petscii_byte(ch.upper())); i += 1; continue
        result.append(TOKENS[match]); i += len(match)
        if match == "REM":
            result.extend(petscii_byte(c) for c in body[i:]); break
        if match == "DATA": data_mode = True
    return bytes(result)

def tokenize(source: str) -> bytes:
    records = []; previous = -1
    for raw in source.splitlines():
        line = raw.rstrip("\r")
        if not line: continue
        number_text, separator, body = line.partition(" ")
        if not separator or not number_text.isdigit(): raise ValueError(f"invalid BASIC source line: {raw!r}")
        number = int(number_text)
        if number <= previous or number > 63999: raise ValueError(f"line numbers must increase and be <=63999: {number}")
        previous = number; records.append((number, tokenize_body(body)))
    address = 0x0801; output = bytearray((0x01, 0x08))
    for number, body in records:
        next_address = address + 2 + 2 + len(body) + 1
        output.extend((next_address & 0xFF, next_address >> 8)); output.extend((number & 0xFF, number >> 8)); output.extend(body); output.append(0); address = next_address
    output.extend((0, 0)); return bytes(output)

def main() -> None:
    parser = argparse.ArgumentParser(); parser.add_argument("source", type=Path); parser.add_argument("output", type=Path); args = parser.parse_args()
    args.output.parent.mkdir(parents=True, exist_ok=True); args.output.write_bytes(tokenize(args.source.read_text(encoding="ascii")))

if __name__ == "__main__": main()
