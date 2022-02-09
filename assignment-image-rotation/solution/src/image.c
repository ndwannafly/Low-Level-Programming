#include "image.h"
#include "pixel.h"

#include <math.h>
#include <stdlib.h>

bool reset_image(struct image* const img, uint64_t width, uint64_t height) {
    img->width = width;
    img->height = height;
    img->data = malloc(width * height * sizeof(struct pixel));
    if (img->data == NULL) {
        return false;
    }
    return true;
}

struct image rotate_image(struct image const img, float angle, struct pixel const background_color) {
    const long double sin_a = sinf(angle), cos_a = cosf(angle);
    const long double sin_ra = sinf(-angle), cos_ra = cosf(-angle);
    
    long double img_origin_x = (double)img.width * 0.5;
    long double img_origin_y = (double)img.height * 0.5;
    
    // find the new image size by rotating 4 corners
    long double min_x = 0, max_x = 0, min_y = 0, max_y = 0;
    
    uint32_t corner_x[] = {0, img.width - 1, 0, img.width - 1};
    uint32_t corner_y[] = {0, 0, img.height - 1, img.height - 1};
    for (size_t i = 0; i < 4; ++i) {
        double dx = (double)(corner_x[i] - img_origin_x);
        double dy = (double)(corner_y[i] - img_origin_y);
        double new_x = (double)(dx * cos_a - dy * sin_a);
        double new_y = (double)(dx * sin_a + dy * cos_a);
        if (min_x > new_x) min_x = new_x;
        if (max_x < new_x) max_x = new_x;
        if (min_y > new_y) min_y = new_y;
        if (max_y < new_y) max_y = new_y;
    }
    
    struct image res;
    reset_image(&res, (uint32_t) (max_x - min_x + 1), (uint32_t) (max_y - min_y + 1));
    
    long double res_origin_x = (double)res.width * 0.5;
    long double res_origin_y = (double)res.height * 0.5;
    
    for (int y = 0; y < res.height; ++y) {
        for (int x = 0; x < res.width; ++x) {
            long double dx = x - res_origin_x;
            long double dy = y - res_origin_y;
            long old_x = (long)round((double)(dx * cos_ra - dy * sin_ra + img_origin_x));
            long old_y = (long)round((double)(dx * sin_ra + dy * cos_ra + img_origin_y));
            
            struct pixel old_pixel = (old_x >= 0 && old_y >= 0 && old_x < img.width && old_y < img.height)
                ? img.data[old_y * img.width + old_x]
                : background_color;
            res.data[y * res.width + x] = old_pixel;
        }
    }
    return res;
}

static size_t rotate0_trans(struct image const img, size_t x, size_t y) {
    return y * img.width + x;
}
static size_t rotate90_trans(struct image const img, size_t x, size_t y) {
    return x * img.height + (img.height - y - 1);
}
static size_t rotate180_trans(struct image const img, size_t x, size_t y) {
    return rotate0_trans(img, img.width - x - 1, img.height - y - 1);
}
static size_t rotate270_trans(struct image const img, size_t x, size_t y) {
    return (img.width - x - 1) * img.height + y;
}

typedef size_t (*transform_fn) (struct image const, size_t, size_t);
static transform_fn transform_func_list[] = {
    rotate0_trans, rotate90_trans, rotate180_trans, rotate270_trans
};
struct image rotate_image_90(struct image const img, int multiple) {
    struct image new_img;
    transform_fn transforms = transform_func_list[multiple & 0x03];
    if (multiple & 0x01) {
        reset_image(&new_img, img.height, img.width);
    } else {
        reset_image(&new_img, img.width, img.height);
    }
    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {
            new_img.data[transforms(img, x, y)] = img.data[y * img.width + x];
        }
    }
    return new_img;
}
