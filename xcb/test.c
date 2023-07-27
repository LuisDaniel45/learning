#include <stdio.h>
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_resize.h"

int resize(int new_width, int new_height, int width, int height)
{
    return  ((float) (new_width * height) /
             (width * new_height)) * new_height;
}


int main(int argc, char **argv)
{
    int num_of_filters = 6;
    char *names[] = {
        "STBIR_FILTER_DEFAULT", 
        "STBIR_FILTER_BOX", 
        "STBIR_FILTER_TRIANGLE", 
        "STBIR_FILTER_CUBICBSPLINE", 
        "STBIR_FILTER_CATMULLROM", 
        "STBIR_FILTER_MITCHELL",
    };
    int width, heigth, num_channels;
    unsigned char *img = stbi_load("ascii.jpg", &width, &heigth, &num_channels, 0); 
    int new_width  = 200,
        new_height = resize(new_width, 200, width, heigth),
        new_size   = new_width * new_height * num_channels, 
        size       = width * heigth * num_channels;
    unsigned char new_img[new_size];
    stbir_filter filter = STBIR_FILTER_BOX;
    clock_t start, end; 
    double cpu_time_used, fastest = 10, times[num_of_filters]; 
    int winner;
    for (size_t i = 0; i < num_of_filters; i++) {
        start = clock(); 
        stbir_resize(img, width, heigth, 0, new_img, new_width, new_height, 0,
                STBIR_TYPE_UINT8, num_channels, 1, 1, STBIR_EDGE_WRAP, STBIR_EDGE_WRAP,
                filter, filter, STBIR_COLORSPACE_LINEAR, NULL);
        end = clock();
        times[i] = ((double) (end - start) / CLOCKS_PER_SEC);
    }

    for (int i = 0; i < num_of_filters; i++) {
        if (fastest > times[i]) {
            winner = i;
            fastest = times[i];
        }
        printf("%s: %f\n", names[i], times[i]);
    }

    printf("\nwinner: %s %f\n", names[winner], fastest);
    free(img);
}

