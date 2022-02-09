#include "image.h"
#include "pixel.h"

#include <stdlib.h>
#include <math.h>

bool reset_image(struct image* const img, uint64_t width, uint64_t height){
    img->width = width;
    img->height = height;
    img->data = malloc(width * height * sizeof(struct pixel));
    if (img->data == NULL){
        return false;
    }

    return true;
}

struct pixel* pixel_of(struct image const image, size_t x, size_t y){
     return image.data + y * image.width + x;
}
