# EGT Raw Image Format

The EGT raw image format (.eraw) is a customized minimal image format that is
optimized solely for decode and embedding into binaries.

## Features

- Raw RGBA 32 bit pixel values.
- Optmized for encode/decode.
- Lossless run length encoding style compression.
- 8 bits per channel ARGB data, top to bottom, left to right.
- Little endian encoding.
- Pre-multiplied alpha.

## Header and Layout

The image format has a 28 byte header followed by a basic run length encoding of
the 32 bit RGBA pixel data.

    [magic]
    [width]
    [height]
    [reserved]
    [reserved]
    [reserved]
    [reserved]
    {block header}[pixel...]...
