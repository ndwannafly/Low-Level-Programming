#include "bmp.h"

#include <stdio.h>
#include <stdlib.h>

enum bmp_read_status image_from_bmp(FILE* in, struct image* const img) {
    struct bmp_header header;
    if (!fread(&header, sizeof(header.bfType), 1, in)) {
        return BMP_READ_ERROR;
    }
    
    if (header.bfType != ('B' | ((uint16_t)'M' << 8))) {
        return BMP_READ_INVALID_SIGNATURE;
    }
    
    if (fseek(in, 0, SEEK_END)) {
        return BMP_READ_ERROR;
    }
    uint32_t file_size = ftell(in);
    if (fseek(in, sizeof(header.bfType), SEEK_SET)) {
        return BMP_READ_ERROR;
    }
    
    if (file_size < sizeof(header)) {
        return BMP_READ_CORRUPTED;
    }
    
    if (!fread(&header.bfileSize, (char*)&header.biWidth - (char*)&header.bfileSize, 1, in)) {
        return BMP_READ_ERROR;
    }
    if (header.bfileSize != file_size) {
        return BMP_READ_CORRUPTED;
    }
    if (header.biSize < BITMAPINFORHEADER_SIZE) {
        return BMP_READ_UNSUPPORTED_HEADER;
    }
    if (!fread(&header.biWidth, BITMAPINFORHEADER_SIZE - sizeof(header.biSize), 1, in)) {
        return BMP_READ_ERROR;
    }
    
    if (file_size < header.bOffBits + header.biSizeImage) {
        return BMP_READ_CORRUPTED;
    }
    
    uint32_t round_up_row_size = (header.biBitCount * header.biWidth + 31) >> 5 <<2;
    
    if (header.biPlanes != 1) {
        return BMP_READ_INVALID_HEADER;
    }
    
    switch (header.biBitCount) {
        case 1: case 4: case 8: case 16: case 24: case 32:
            break;
        default:
            return BMP_READ_INVALID_HEADER;
    }
    
    switch (header.biCompression) {
        case BI_COMPRESS_RGB:
        case BI_COMPRESS_RLE8:
        case BI_COMPRESS_RLE4:
        case BI_COMPRESS_BITFIELDS:
        case BI_COMPRESS_JPEG:
        case BI_COMPRESS_PNG:
        case BI_COMPRESS_ALPHABITFIELDS:
        case BI_COMPRESS_CMYK:
        case BI_COMPRESS_CMYKRLE8:
        case BI_COMPRESS_CMYKRLE4:
            break;
        default:
            return BMP_READ_INVALID_HEADER;
    }
    
    // don't need to check for the rest of the fields
    
    if (header.biWidth <= 0 || header.biHeight <= 0 || header.bfileSize == 0 || header.biBitCount != 24) {
        return BMP_READ_UNSUPPORTED_COLOR_DEPTH;
    }
    if (header.biCompression) {
        return BMP_READ_UNSUPPORTED_COMPRESSION_METHOD;
    }
    
    if (!reset_image(img, header.biWidth, header.biHeight)) {
        return BMP_READ_MEMORY_ERROR;
    }
    if (fseek(in, header.bOffBits, SEEK_SET)) {
        return BMP_READ_ERROR;
    }
    size_t padding = round_up_row_size - ((header.biBitCount * header.biWidth) >> 3);
    for (size_t row = 0; row < header.biHeight; ++row) {
        if (fread(img->data + row * header.biWidth, sizeof(struct pixel), header.biWidth, in) < header.biWidth) {
            return BMP_READ_ERROR;
        }
        if (fseek(in, (long)padding, SEEK_CUR)) {
            return BMP_READ_ERROR;
        }
    }
    return BMP_READ_OK;
}

enum bmp_write_status image_to_bmp(FILE* out, struct image const img) {
    uint32_t round_up_row_size = (sizeof(struct pixel) * 8 * img.width + 31) >> 5 << 2;
    size_t padding = round_up_row_size - sizeof(struct pixel) * img.width;
    
    uint32_t data_size = round_up_row_size * img.height;
    
    struct bmp_header header;
    header.bfType = 'B' | ((uint16_t)'M' << 8);
    header.bfileSize = sizeof(header) + data_size;
    header.bfReserved = 0;
    header.bOffBits = sizeof(header);
    header.biSize = BITMAPINFORHEADER_SIZE;
    
    header.biWidth = img.width;
    header.biHeight = img.height;
    header.biPlanes = 1;
    header.biBitCount = 24;
    header.biCompression = BI_COMPRESS_RGB;
    header.biSizeImage = data_size;
    header.biXPelsPerMeter = DEFAULT_RESOLUTION;
    header.biYPelsPerMeter = DEFAULT_RESOLUTION;
    header.biClrUsed = 0;
    header.biClrImportant = 0;
    
    if (!fwrite(&header, sizeof(header), 1, out)) {
        return BMP_WRITE_ERROR;
    }
    
    uint32_t temp_value = 0;  // used to fill the padding
    for (size_t row = 0; row < img.height; ++row) {
        if (fwrite(img.data + row * img.width, sizeof(struct pixel), img.width, out) < img.width) {
            return BMP_WRITE_ERROR;
        }
        if (fwrite(&temp_value, 1, padding, out) != padding) {
            return BMP_WRITE_ERROR;
        }
    }
    
    return BMP_WRITE_OK;
}
