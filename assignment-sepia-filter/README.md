# Description
A program to perform a sepia filter on an image. A sepia filter makes an image with vivid colors look like an old, aged photograph. Most graphical editors include a sepia filter.

# Run

* Generate default `output.bmp`

    `make input.bmp`

* Generate to target output file

    `make input.bmp target.bmp`

# Note

This program only supports:
* Image file in bmp format.
* 24-bit color depth.
* All headers with size not less than 40 bytes.
* bmp file must be uncompressed