#!/usr/bin/env python3
"""Convert a C uint32_t pixel array (0xAARRGGBB) to a uint16_t RGB565 array.

Usage:
  python convert_uint32_pixels_to_uint16.py \
      --input list_for_conversion_to_uint16.c \
      --output valvano_rgb565.c

Notes:
- Transparent pixels (alpha == 0) are emitted as 0xF81F (bright magenta key).
- Non-transparent pixels ignore alpha and convert RGB to RGB565.
"""

from __future__ import annotations

import argparse
import re
from pathlib import Path
from typing import Dict, List, Sequence

HEX_LITERAL_RE = re.compile(r"0x[0-9a-fA-F]+")
DEFINE_RE = re.compile(r"^\s*#define\s+([A-Za-z_][A-Za-z0-9_]*)\s+(.+?)\s*$", re.MULTILINE)
ARRAY_RE = re.compile(
    r"static\s+const\s+uint32_t\s+([A-Za-z_][A-Za-z0-9_]*)\s*((?:\[[^\]]+\])+?)\s*=\s*\{(.*?)\};",
    re.DOTALL,
)
DIM_RE = re.compile(r"\[([^\]]+)\]")


class ParseError(RuntimeError):
    pass


def parse_defines(source: str) -> Dict[str, int]:
    defines: Dict[str, int] = {}
    for name, value_text in DEFINE_RE.findall(source):
        value_text = value_text.split("//", 1)[0].strip()
        if not value_text:
            continue
        try:
            # Evaluate only integer literals/expressions using already-known defines.
            value = eval(value_text, {"__builtins__": {}}, defines)
        except Exception:
            continue
        if isinstance(value, int):
            defines[name] = value
    return defines


def resolve_dim(expr: str, defines: Dict[str, int]) -> int:
    expr = expr.strip()
    try:
        value = eval(expr, {"__builtins__": {}}, defines)
    except Exception as exc:
        raise ParseError(f"Could not evaluate dimension expression '{expr}': {exc}") from exc
    if not isinstance(value, int) or value <= 0:
        raise ParseError(f"Invalid dimension value for '{expr}': {value!r}")
    return value


def rgb888_to_rgb565(rgb32: int) -> int:
    alpha = (rgb32 >> 24) & 0xFF
    if alpha == 0:
        # Use a visible chroma key so transparency is distinguishable from black.
        return 0xF81F

    r = (rgb32 >> 16) & 0xFF
    g = (rgb32 >> 8) & 0xFF
    b = rgb32 & 0xFF
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)


def flatten_to_nested(values: Sequence[int], dims: Sequence[int]) -> List:
    if len(dims) == 1:
        return list(values)

    stride = 1
    for d in dims[1:]:
        stride *= d

    nested = []
    for i in range(dims[0]):
        start = i * stride
        end = start + stride
        nested.append(flatten_to_nested(values[start:end], dims[1:]))
    return nested


def format_c_array(data, indent: int = 0, per_line: int = 12) -> str:
    space = " " * indent

    if isinstance(data, list) and data and isinstance(data[0], int):
        lines = []
        for i in range(0, len(data), per_line):
            chunk = data[i : i + per_line]
            items = ", ".join(f"0x{v:04X}" for v in chunk)
            suffix = "," if i + per_line < len(data) else ""
            lines.append(f"{space}  {items}{suffix}")
        return "{\n" + "\n".join(lines) + f"\n{space}}}"

    if isinstance(data, list):
        items = [format_c_array(item, indent + 2, per_line) for item in data]
        joined = (",\n").join(f"{space}  {item}" for item in items)
        return "{\n" + joined + f"\n{space}}}"

    raise TypeError("Unsupported data type for C array formatting")


def detect_frame_shape(in_name: str, defines: Dict[str, int], dims: Sequence[int]) -> tuple[int, int] | None:
    if len(dims) != 2:
        return None

    prefix = in_name
    if prefix.endswith("_data"):
        prefix = prefix[:-5]
    macro_base = prefix.upper()

    width = defines.get(f"{macro_base}_FRAME_WIDTH")
    height = defines.get(f"{macro_base}_FRAME_HEIGHT")
    if isinstance(width, int) and isinstance(height, int) and width > 0 and height > 0:
        return width, height

    return None


def flip_frames_vertically(values: Sequence[int], dims: Sequence[int], frame_width: int, frame_height: int) -> List[int]:
    frame_size = frame_width * frame_height
    if frame_size <= 0:
        raise ParseError("Invalid frame size for vertical flip.")

    if len(dims) != 2:
        raise ParseError("Vertical flip currently supports 2D arrays [frames][pixels].")

    if dims[1] != frame_size:
        raise ParseError(
            f"Frame dimensions mismatch: width*height={frame_size}, but pixels per frame={dims[1]}."
        )

    flipped: List[int] = []
    for frame in range(dims[0]):
        base = frame * frame_size
        frame_pixels = list(values[base : base + frame_size])

        # ST7735_DrawBitmap expects rows in bottom-to-top order.
        for row in range(frame_height - 1, -1, -1):
            row_start = row * frame_width
            row_end = row_start + frame_width
            flipped.extend(frame_pixels[row_start:row_end])

    return flipped


def convert_file(
    input_path: Path,
    output_path: Path,
    array_name: str | None,
    force_width: int | None,
    force_height: int | None,
    flip_y: bool,
) -> None:
    source = input_path.read_text(encoding="utf-8")
    defines = parse_defines(source)

    match = ARRAY_RE.search(source)
    if not match:
        raise ParseError("No 'static const uint32_t ... = {...};' array found.")

    in_name, dims_text, initializer = match.groups()
    dim_exprs = DIM_RE.findall(dims_text)
    if not dim_exprs:
        raise ParseError("Array dimensions were not found.")

    dims = [resolve_dim(expr, defines) for expr in dim_exprs]

    hex_values = HEX_LITERAL_RE.findall(initializer)
    if not hex_values:
        raise ParseError("No hexadecimal pixel literals found in the array initializer.")

    pixels32 = [int(x, 16) for x in hex_values]

    expected = 1
    for d in dims:
        expected *= d

    if len(pixels32) != expected:
        raise ParseError(
            f"Pixel count mismatch: found {len(pixels32)} values, expected {expected} from dimensions {dims}."
        )

    pixels16 = [rgb888_to_rgb565(px) for px in pixels32]
    frame_width: int | None = None
    frame_height: int | None = None

    if flip_y:
        if force_width is not None or force_height is not None:
            if force_width is None or force_height is None:
                raise ParseError("Both --frame-width and --frame-height must be provided together.")
            frame_width, frame_height = force_width, force_height
        else:
            shape = detect_frame_shape(in_name, defines, dims)
            if shape is None:
                raise ParseError(
                    "Could not infer frame width/height for vertical flip. "
                    "Use --frame-width and --frame-height, or run with --no-flip-y."
                )
            frame_width, frame_height = shape

        pixels16 = flip_frames_vertically(pixels16, dims, frame_width, frame_height)

    nested = flatten_to_nested(pixels16, dims)

    out_name = array_name if array_name else f"{in_name}_rgb565"
    macro_base = out_name.upper()
    dims_decl = "".join(f"[{d}]" for d in dims)

    # Try to provide metadata even if vertical flip is disabled.
    if frame_width is None or frame_height is None:
        shape = detect_frame_shape(in_name, defines, dims)
        if shape is not None:
            frame_width, frame_height = shape

    meta_lines = [
        f"#define {macro_base}_FRAME_COUNT {dims[0]}",
    ]
    if len(dims) >= 2:
        meta_lines.append(f"#define {macro_base}_PIXELS_PER_FRAME {dims[1]}")
    if frame_width is not None and frame_height is not None:
        meta_lines.append(f"#define {macro_base}_FRAME_WIDTH {frame_width}")
        meta_lines.append(f"#define {macro_base}_FRAME_HEIGHT {frame_height}")
    metadata = "\n".join(meta_lines)

    header_comment = (
        f"/* Auto-generated from {input_path.name} by {Path(__file__).name}. */\n"
        "/* Source pixels: 0xAARRGGBB, output pixels: RGB565 (uint16_t). */\n"
    )

    c_text = (
        "#include <stdint.h>\n\n"
        + header_comment
        + "\n"
        + metadata
        + "\n\n"
        + f"static const uint16_t {out_name}{dims_decl} = "
        + format_c_array(nested)
        + ";\n"
    )

    output_path.write_text(c_text, encoding="utf-8")


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Convert a C uint32_t pixel array (0xAARRGGBB) to a C uint16_t RGB565 array."
    )
    parser.add_argument(
        "--input",
        "-i",
        type=Path,
        default=Path("list_for_conversion_to_uint16.c"),
        help="Input C file containing a static const uint32_t pixel array.",
    )
    parser.add_argument(
        "--output",
        "-o",
        type=Path,
        default=Path("converted_pixels_rgb565.c"),
        help="Output C file for the converted uint16_t array.",
    )
    parser.add_argument(
        "--array-name",
        "-n",
        type=str,
        default=None,
        help="Optional output array name (default: <input_array_name>_rgb565).",
    )
    parser.add_argument(
        "--frame-width",
        type=int,
        default=None,
        help="Optional frame width (required with --frame-height when overriding auto-detection).",
    )
    parser.add_argument(
        "--frame-height",
        type=int,
        default=None,
        help="Optional frame height (required with --frame-width when overriding auto-detection).",
    )
    parser.add_argument(
        "--no-flip-y",
        action="store_true",
        help="Disable vertical row flip (enabled by default for ST7735_DrawBitmap compatibility).",
    )
    return parser


def main() -> int:
    parser = build_parser()
    args = parser.parse_args()

    try:
        convert_file(
            args.input,
            args.output,
            args.array_name,
            args.frame_width,
            args.frame_height,
            flip_y=not args.no_flip_y,
        )
    except (OSError, ParseError, TypeError) as exc:
        print(f"Error: {exc}")
        return 1

    print(f"Wrote RGB565 array to: {args.output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
