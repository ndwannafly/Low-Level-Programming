#ifndef __IMAGE_SEPIA_H__
#define __IMAGE_SEPIA_H__


#include <stdint.h>
#include "pixel.h"
#include "image.h"

extern const float SEPIA_COEFFICIENT[3][3];

unsigned char sat(uint64_t x);
void sepia_one(struct pixel* const pixel);

#ifdef SEPIA_SLOW_IMPL
void slow_sepia_inplace(struct image* img);
#   ifndef SEPIA_FAST_IMPL
    static inline void sepia_inplace(struct image* img) {
    	slow_sepia_inplace(img);
    }
#   endif
#endif


#ifdef SEPIA_FAST_IMPL
void fast_sepia_inplace(struct image* img);
#   ifndef SEPIA_SLOW_IMPL
    static inline void sepia_inplace(struct image* img){
        fast_sepia_inplace(img);
    }
#   endif
#endif

#endif // __IMAGE_SEPIA_H__
