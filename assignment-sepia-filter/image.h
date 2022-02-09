#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "pixel.h"

struct image {
    uint64_t width, height;
    struct pixel * data;
};

bool reset_image(struct image* const img, uint64_t width, uint64_t height);
struct pixel* pixel_of(struct image const image, size_t x, size_t y);

#endif // __IMAGE_H__

