#ifndef H_PNG
#define H_PNG

// PNG defines
#define MAX_PNGDIMENSION            8192            // max width / height for PNG, increased from TGA for potential larger images
#define MAX_PNGCHANNELS             4               // RGBA
#define MAX_PNGBYTES                (MAX_PNGDIMENSION * MAX_PNGDIMENSION * MAX_PNGCHANNELS) // 8192x8192x4 = 134,217,728 bytes

// Global variables for PNG data
extern int png_width, png_height, png_channels, png_size;
extern unsigned char png_data[MAX_PNGBYTES];

// Function prototypes
int png_getpixels(const char *filename);

#endif  // H_PNG
