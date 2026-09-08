#!/usr/bin/env python3
"""Capture a PNG screenshot from an M5Stack over a serial connection.

The sketch must respond to the request with:

    PNG1
    uint32 little-endian payload length
    PNG payload

Human-readable serial output before the PNG1 marker is ignored.
"""

from __future__ import annotations

import argparse
import struct
import sys
import time
from pathlib import Path

try:
    import serial
    from serial import SerialException
except ImportError:
    print("pyserial is required: python3 -m pip install pyserial", file=sys.stderr)
    raise SystemExit(2)


FRAME_MAGIC = b"PNG1"
PNG_SIGNATURE = b"\x89PNG\r\n\x1a\n"
DEFAULT_BAUD = 115200
DEFAULT_TIMEOUT = 15.0
DEFAULT_MAX_BYTES = 8 * 1024 * 1024


def read_exact(port: serial.Serial, size: int, deadline: float) -> bytes:
    """Read exactly size bytes before the deadline expires."""
    data = bytearray()
    while len(data) < size:
        if time.monotonic() >= deadline:
            raise TimeoutError(
                f"timed out while reading {size} bytes "
                f"(received {len(data)})"
            )
        chunk = port.read(size - len(data))
        if chunk:
            data.extend(chunk)
    return bytes(data)


def find_frame_magic(port: serial.Serial, deadline: float) -> None:
    """Discard serial text until the PNG frame marker is found."""
    window = bytearray()
    while time.monotonic() < deadline:
        byte = port.read(1)
        if not byte:
            continue
        window.extend(byte)
        if len(window) > len(FRAME_MAGIC):
            del window[:-len(FRAME_MAGIC)]
        if bytes(window) == FRAME_MAGIC:
            return
    raise TimeoutError("timed out waiting for PNG1 frame marker")


def capture(args: argparse.Namespace) -> Path:
    """Request one screenshot and write it to the requested output path."""
    output = Path(args.output).expanduser()
    temporary = output.with_name(output.name + ".part")
    deadline = time.monotonic() + args.timeout

    output.parent.mkdir(parents=True, exist_ok=True)
    try:
        with serial.Serial(
            port=args.port,
            baudrate=args.baud,
            timeout=0.1,
            write_timeout=2.0,
        ) as port:
            if args.boot_delay > 0:
                time.sleep(args.boot_delay)
            port.reset_input_buffer()
            port.write(args.request.encode("ascii"))
            port.flush()

            find_frame_magic(port, deadline)
            length_bytes = read_exact(port, 4, deadline)
            payload_length = struct.unpack("<I", length_bytes)[0]
            if payload_length < len(PNG_SIGNATURE):
                raise ValueError(f"invalid PNG payload length: {payload_length}")
            if payload_length > args.max_bytes:
                raise ValueError(
                    f"PNG payload is {payload_length} bytes, exceeding "
                    f"the {args.max_bytes}-byte limit"
                )

            signature = read_exact(port, len(PNG_SIGNATURE), deadline)
            if signature != PNG_SIGNATURE:
                raise ValueError("frame payload is not a PNG image")

            remaining = payload_length - len(PNG_SIGNATURE)
            with temporary.open("wb") as image:
                image.write(signature)
                while remaining:
                    chunk_size = min(4096, remaining)
                    image.write(read_exact(port, chunk_size, deadline))
                    remaining -= chunk_size
            temporary.replace(output)
    except Exception:
        temporary.unlink(missing_ok=True)
        raise

    return output


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Request and save a PNG screenshot over serial."
    )
    parser.add_argument("--port", required=True, help="Serial device, e.g. /dev/ttyACM0")
    parser.add_argument("--baud", type=int, default=DEFAULT_BAUD)
    parser.add_argument("--output", default="screenshot.png", help="Output PNG path")
    parser.add_argument(
        "--request",
        default="s\n",
        help="Request sent to the sketch (default: 's' followed by newline)",
    )
    parser.add_argument("--timeout", type=float, default=DEFAULT_TIMEOUT)
    parser.add_argument(
        "--boot-delay",
        type=float,
        default=2.0,
        help="Seconds to wait after opening the port for board reset (default: 2)",
    )
    parser.add_argument(
        "--max-bytes",
        type=int,
        default=DEFAULT_MAX_BYTES,
        help="Reject images larger than this many bytes",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        output = capture(args)
    except (OSError, SerialException, TimeoutError, ValueError) as error:
        print(f"capture failed: {error}", file=sys.stderr)
        return 1
    print(f"saved {output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
