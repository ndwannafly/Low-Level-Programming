#include "bmp.h"
#include "image.h"
#include "pixel.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static void print_help(char* command) {
    printf("Usage: %s <source-image> <transformed-image>\n", command);
    printf("\n");
    printf("Rotate the image given by input file couter-clockwise by the given angle.\n");
    printf("The result is written to output_file in bmp file format uncompressed 24-bit color depth.\n");
    printf("When rotating, new empty area will appear. This area will be filled with fillColor.\n");
    printf("\n");
    printf("Arguments:\n");
    printf("\tsource-image  - input file name for the bmp file.\n"
           "\t              This program is currently supporting uncompressed 24-bit color depth.\n");
    printf("\ttransformed-image - output file name. Default is \"out.bmp\"\n");
    exit(0);
}

static const char* read_status_msg[] = {
    [BMP_READ_OK] = "",
    [BMP_READ_ERROR] = "Unable to read from the input file.",
    [BMP_READ_INVALID_SIGNATURE] = "The file is not be a bmp image.",
    [BMP_READ_INVALID_HEADER] = "Invalid bmp header.",
    [BMP_READ_CORRUPTED] = "The input file is corrupted.",
    [BMP_READ_UNSUPPORTED_HEADER] = "This program only supports all headers with size not less than 40 bytes.",
    [BMP_READ_UNSUPPORTED_COLOR_DEPTH] = "This program only supports 24-bit color depth.",
    [BMP_READ_UNSUPPORTED_COMPRESSION_METHOD] = "This program only supports uncompressed bmp file.",
    [BMP_READ_MEMORY_ERROR] = "If you see this, then your computer's suck! :)"
};

const double pi = 3.14159;

int main(int argc, char** argv) {
    if (argc < 2) {
        print_help(argv[0]);
        return 0;
    }
    const char* input_file_name = argv[1];
    const char* output_file_name = argc < 3 ? "out.bmp" : argv[2];
    //struct pixel fill_color = {0, 0, 0};
    
    
    int exit_code = 0;
    FILE* inp_file = fopen(input_file_name, "rb");
    if (!inp_file) {
        fprintf(stderr, "Error: Unable to open \"%s\".\n", input_file_name);
        exit_code = 1;
        goto the_end;
    }
    FILE* out_file = fopen(output_file_name, "wb");
    if (!out_file) {
        fprintf(stderr, "Error: Unable to open \"%s\".\n", output_file_name);
        exit_code = 1;
        goto close_inp_file;
    }
    struct image img, new_img;
    img.data = NULL;
    new_img.data = NULL;
    enum bmp_read_status read_status = image_from_bmp(inp_file, &img);
    if (read_status) {
        fprintf(stderr, "Error: %s\n", read_status_msg[read_status]);
        exit_code = 1;
        goto clean_up;
    }
    
    new_img = rotate_image_90(img, 1);
    
    enum bmp_write_status write_status = image_to_bmp(out_file, new_img);
    if (write_status) {
        fprintf(stderr, "Error: Unable to write to %s\n", output_file_name);
        exit_code = 1;
        // goto clean_up;  // wait we're already there!
    }

clean_up:
    if (new_img.data) free(new_img.data); 
    if (img.data) free(img.data); 
    fclose(out_file);
close_inp_file:
    fclose(inp_file);
the_end:
    return exit_code;
}
