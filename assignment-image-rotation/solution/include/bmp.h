#ifndef __BMP_H__
#define __BMP_H__
#include  <stdint.h>
#include "image.h"
#define BITMAPINFORHEADER_SIZE 40
#define DEFAULT_RESOLUTION 2800  // pixels per meter
                                 // which is 28 pixels per cm
                                 // which is 72 pixels per inch
struct __attribute__((packed)) bmp_header {
    uint16_t bfType;
    uint32_t bfileSize;
    uint32_t bfReserved;
    uint32_t bOffBits;
    uint32_t biSize;
    
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    uint32_t biXPelsPerMeter;
    uint32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};
// this is included just for the sake of the correctness.
// these values is from https://en.wikipedia.org/wiki/BMP_file_format
enum bmp_compression_method {
    BI_COMPRESS_RGB 	        = 0,
    BI_COMPRESS_RLE8 	        = 1,
    BI_COMPRESS_RLE4 	        = 2,
    BI_COMPRESS_BITFIELDS 	    = 3,
    BI_COMPRESS_JPEG 	        = 4,
    BI_COMPRESS_PNG 		    = 5,
    BI_COMPRESS_ALPHABITFIELDS 	= 6,
    BI_COMPRESS_CMYK 	        = 11,
    BI_COMPRESS_CMYKRLE8        = 12,
    BI_COMPRESS_CMYKRLE4        = 13
};
enum bmp_read_status  {
    BMP_READ_OK = 0,
    BMP_READ_ERROR,
    BMP_READ_INVALID_SIGNATURE,
    BMP_READ_INVALID_HEADER,
    BMP_READ_CORRUPTED,
    BMP_READ_UNSUPPORTED_HEADER,
    BMP_READ_UNSUPPORTED_COLOR_DEPTH,
    BMP_READ_UNSUPPORTED_COMPRESSION_METHOD,
    BMP_READ_MEMORY_ERROR
};
enum bmp_read_status image_from_bmp(FILE* in, struct image* const img);
enum bmp_write_status  {
    BMP_WRITE_OK = 0,
    BMP_WRITE_ERROR
};
enum bmp_write_status image_to_bmp(FILE* out, struct image const img); 

#endif // __BMP_H__
