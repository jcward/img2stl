// png.c - loading PNG images

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>

#include "main.h"
#include "png.h"

// Global variables for image information
int png_width, png_height, png_channels, png_size;
unsigned char png_data[MAX_PNGBYTES];

int png_getpixels(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    png_structp png_ptr;
    png_infop info_ptr;
    png_bytep *row_pointers;
    unsigned char header[8];
    int y;

    // Open the file
    if (!fp) {
        fprintf(stderr, "File %s could not be opened for reading\n", filename);
        return 0;
    }

    // Read the header
    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8)) {
        fprintf(stderr, "File %s is not recognized as a PNG file\n", filename);
        fclose(fp);
        return 0;
    }

    // Initialize read structure
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fclose(fp);
        return 0;
    }

    // Initialize info structure
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fclose(fp);
        return 0;
    }

    // Setup error handling
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "Error during png reading\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        return 0;
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    // Get image dimensions
    png_width = png_get_image_width(png_ptr, info_ptr);
    png_height = png_get_image_height(png_ptr, info_ptr);
    png_byte color_type = png_get_color_type(png_ptr, info_ptr);
    png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    // Convert palette color to RGB
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);

    // Convert 1, 2, 4-bit to 8-bit grayscale
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png_ptr);

    // Convert grayscale to RGB if needed
    if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_GRAY || png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);

    // Convert tRNS to alpha channel
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png_ptr);

    // If we're dealing with 16-bit data, strip it to 8-bit
    if (bit_depth == 16)
        png_set_strip_16(png_ptr);

    // Update the info structure
    png_read_update_info(png_ptr, info_ptr);

    // Row size in bytes
    png_size = png_get_rowbytes(png_ptr, info_ptr);
    png_channels = png_get_channels(png_ptr, info_ptr);

    if (png_size > MAX_PNGBYTES) {
        fprintf(stderr, "Image too large\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        return 0;
    }

    // Allocate memory for entire image
    row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * png_height);
    for (y = 0; y < png_height; y++) {
        row_pointers[y] = (png_byte*) malloc(png_size);
    }

    // Read in the image data
    png_read_image(png_ptr, row_pointers);

    // Copy into our global data array
    for (y = 0; y < png_height; y++) {
        memcpy(png_data + y * png_size, row_pointers[y], png_size);
    }

    // Cleanup
    for (y = 0; y < png_height; y++) {
        free(row_pointers[y]);
    }
    free(row_pointers);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);

    printf("png_getpixels: loaded %dx%dx%d bytes of pixel data from PNG\n", png_width, png_height, png_channels);
    return png_size;
}
