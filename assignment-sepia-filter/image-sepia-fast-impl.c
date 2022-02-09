#define SEPIA_FAST_IMPL
#include "image-sepia.h"
#include "image.h"
#include "pixel.h"

extern void fast_sepia_inplace_helper(struct pixel* const data, size_t n_pixel, const float coefficient_t[3][3]);

void fast_sepia_inplace(struct image* img){
    const float SEPIA_COEFFICIENT_FOR_HELPER[3][3] = {
        { SEPIA_COEFFICIENT[2][2], SEPIA_COEFFICIENT[1][2], SEPIA_COEFFICIENT[0][2]},
	{ SEPIA_COEFFICIENT[2][1], SEPIA_COEFFICIENT[1][1], SEPIA_COEFFICIENT[0][1]},
	{ SEPIA_COEFFICIENT[2][0], SEPIA_COEFFICIENT[1][0], SEPIA_COEFFICIENT[0][0]},
    };

    const size_t total_pixel = img->width * img->height;
    const size_t process_by_hand = total_pixel % 4;
    const size_t process_with_sse = total_pixel - process_by_hand;

    fast_sepia_inplace_helper(img->data, process_with_sse, SEPIA_COEFFICIENT_FOR_HELPER);
    for (size_t i = process_with_sse; i < total_pixel; ++i){
    	sepia_one(img->data + i);
    }

}
