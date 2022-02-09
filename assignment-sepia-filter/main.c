#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"
#include "image.h"
#include "pixel.h"

#define REQUIRE_SEPIA_INPLACE_FUNCTION
#include "image-sepia.h"

static const char* read_status_msg[] = {
    [BMP_READ_OK] = "",
    [BMP_READ_ERROR] = "Unable to read from the input file.",
    [BMP_READ_INVALID_SIGNATURE] = "The file is not be a bmp image.",
    [BMP_READ_INVALID_HEADER] = "Invalid bmp header.",
    [BMP_READ_CORRUPTED] = "The input file is corrupted",
    [BMP_READ_UNSUPPORTED_HEADER] = "This program only supports all headers with size not less than 40 bytes.",
    [BMP_READ_UNSUPPORTED_COLOR_DEPTH] = "This program only supports 24-bit color depth.",
    [BMP_READ_UNSUPPORTED_COMPRESSION_METHOD] = "This program only supports uncompressed bmp file.",
    [BMP_READ_MEMORY_ERROR] = "If you see this, then your computer's suck! :)"
};

int main(int argc, char** argv){
    if(argc < 2){
       fprintf(stderr, "Usage: %s \"input_file_name\" [\"output_file_name\"]\n", argv[0]);
       fprintf(stderr, "The input image file must be in BMP format, uncompressed, 24 bit color depth\n");
       fprintf(stderr, "The default output file is \"out.bmp\".\n");
       return 0;
    }
    const char* input_file_name = argv[1];
    const char* output_file_name = argc < 3 ? "out.bmp" : argv[2];

    printf("%s %s\n", input_file_name, output_file_name);
    int exit_code = 0;
    FILE* inp_file = fopen(input_file_name, "rb");
    if (!inp_file){
    	fprintf(stderr, "Error: Unable to open \"%s\" for reading.\n", input_file_name);
	exit_code = 1;
	goto the_end;
    }

    FILE* out_file = fopen(output_file_name, "wb");
    if (!out_file){
    	fprintf(stderr, "Error: Unable to open \"%s\" for writing.\n", output_file_name);
	exit_code = 1;
	goto close_inp_file;
    }

    struct image img;
    img.data = NULL;
    enum bmp_read_status read_status = image_from_bmp(inp_file, &img);
    if (read_status){
    	fprintf(stderr, "Error: %s\n", read_status_msg[read_status]);
	exit_code = 1;
	goto clean_up;
    }
    
    sepia_inplace(&img);
    enum bmp_write_status write_status = image_to_bmp(out_file, img);
    if (write_status){
    	fprintf(stderr, "Error: Unable to write to %s\n", output_file_name);
	exit_code = 1;
    }

clean_up:
    if (img.data) free(img.data);
    fclose(out_file);
close_inp_file:
    fclose(inp_file);
the_end:
    return exit_code;
}
