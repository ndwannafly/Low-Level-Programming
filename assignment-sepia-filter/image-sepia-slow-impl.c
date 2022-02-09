#define SEPIA_SLOW_IMPL
#include "image-sepia.h"

#include "image.h"
#include "pixel.h"

void slow_sepia_inplace( struct image* img){
    uint32_t x, y;
    for( y = 0; y < img->height; y++)
	for( x = 0; x < img->width; x++)
	    sepia_one( pixel_of( *img, x, y));
}
