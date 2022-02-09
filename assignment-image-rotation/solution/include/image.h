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
struct image rotate_image(struct image const img, float angle, struct pixel const background_color); 
struct image rotate_image_90(struct image const img, int multiple);

#endif // __IMAGE_H__
