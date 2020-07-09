# EGT Raw Image Format

The EGT raw image format (.eraw) is a customized minimal image format that is
optimized solely for decode and embedding into binaries.

## Features

- Raw RGBA 32 bit pixel values.
- Optimized for encode/decode speed over compression.
- Lossless run length encoding style compression.
- 8 bits per channel RGBA data, top to bottom, left to right.
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

Notes
- [32 bit unsigned]
- {16 bit unsigned]
- Magic is defined as 0x50502AA2.
- Width and height are specified in pixels.
- Reserved words should always be zero.

Each block is prefixed with a 16bit header followed by pixel data.  A block
represents an as-is length of pixel data or repeated pixel data using high
order bit flags of the block header.  The maxiumum number of pixels in a block
is 0x7fff.  A block header masking with 0x8000 indicates repeated pixel data for
the number specified.
