#include "image-sepia.h"

#include <inttypes.h>
#include "image.h"
#include "pixel.h"

const float SEPIA_COEFFICIENT[3][3] = {
	{ .393f, .769f, .189f },
	{ .349f, .686f, .168f },
	{ .272f, .543f, .131f }
};

unsigned char sat( uint64_t x) {
    if (x < 256) return x;
    return 255;
}

void sepia_one( struct pixel* const pixel) {
    struct pixel const old = *pixel;

    pixel->r = sat(
		old.r * SEPIA_COEFFICIENT[0][0] +
		old.g * SEPIA_COEFFICIENT[0][1] +
	       	old.b * SEPIA_COEFFICIENT[0][2] 
	);
    pixel->g = sat(
	    old.r * SEPIA_COEFFICIENT[1][0] +
	    old.g * SEPIA_COEFFICIENT[1][1] +
	    old.b * SEPIA_COEFFICIENT[1][2]
        );
    pixel->b = sat(
	    old.r * SEPIA_COEFFICIENT[2][0] +
	    old.g * SEPIA_COEFFICIENT[2][1] +
	    old.b * SEPIA_COEFFICIENT[2][2]
	);
}
