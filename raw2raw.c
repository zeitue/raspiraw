#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char ra[3840*2160*2];
short pa[3840*2160*2];

enum {
    RAW10,
    RAW12,
    RAW14
} RAW_TYPE;

static const char *format_array[3] = {"RAW10", "RAW12", "RAW14"};
static const char *help = "help";

int main(int argc, char *argv[])
{
    printf("argc=%d\n", argc);
    if (argc < 4) {
        printf("Usage: <filename> <format: RAW10/RAW12/RAW14> <width> <height> [stride]\r\n");
        return -1;
    }
    int format_index = -1;
    char *format = argv[2];
    for(int i=0; i<3; i++) {
        if(0 == strcmp(format_array[i], format)) {
            format_index = i;
            break;
        }
    }
    if (format_index == -1) {
        fprintf(stderr, "Error: Not supported format: %s\n", format);
        fprintf(stderr, "Usage: <filename> <format: RAW10/RAW12/RAW14> <width> <height> [stride]\r\n");
        return -1;
    }

    char *file_name = argv[1];
    int width = atoi(argv[3]);
    int height = atoi(argv[4]);
    int stride = 0;
    int in_size = 0;
    int out_size = 0;
    switch(format_index) {
        case RAW10:
            out_size = width*height*2;
            break;
        case RAW12:
            out_size = width*height*2;
            break;
        case RAW14:
            out_size = width*height*2;
            break;
    }
    
    FILE *raw_fb = fopen(file_name, "rb" );
    if(!raw_fb) {
        printf("open input file faile: %s\r\n", file_name);
        fprintf(stderr, "Usage: <filename> <format: RAW10/RAW12/RAW14> <width> <height> [stride]\r\n");
        return -1;
    } else {
        fseek(raw_fb, 0, SEEK_END);
        in_size = ftell(raw_fb);
        fseek(raw_fb, 0, SEEK_SET);
    }
    
    if (argc > 5) {
        stride == atoi(argv[5]);
    } else {
        // TODO: should we do ALIGN(MMAL use ALIGN 32).
        // or get from file size.
        stride = in_size / height;
    }
    out_size = width*height*2;
    
    printf("file name: %s \r\n", file_name);
    printf("format: %s \r\n", format);
    printf("width: %d \r\n", width);
    printf("height: %d \r\n", height);
    printf("in_size: %d \r\n", in_size);
    printf("out_size: %d \r\n", out_size);
    printf("stride: %d \r\n", stride);

    char out_file[256];
    sprintf(out_file, "%s_%s", file_name, format);
    printf("open input file name: %s\r\n", out_file);
    FILE *pixel_fb = fopen(out_file, "wb");
    if(!pixel_fb) {
        printf("open output file faile: %s\r\n", out_file);
        fprintf(stderr, "Usage: <filename> <format: RAW10/RAW12/RAW14> <width> <height> [stride]\r\n");
        return -1;
    }

    printf("start convertion format_index: %d \r\n", format_index);
    int bIdx = 0;
    int pIdx = 0;
    fread(ra, 1, in_size, raw_fb);
    switch(format_index) {
        case RAW10:
            for(bIdx=0, pIdx=0; bIdx < (in_size-5); ) {
                pa[pIdx+0] = (((short)(ra[bIdx+0]))<<2) & 0x03FC;
                pa[pIdx+0] = pa[pIdx+0] | (short)((ra[bIdx+4]>>0) & 0x0003);

                pa[pIdx+1] = (((short)(ra[bIdx+1]))<<2) & 0x03FC;
                pa[pIdx+1] = pa[pIdx+1] | (short)((ra[bIdx+4]>>2) & 0x0003);

                pa[pIdx+2] = (((short)(ra[bIdx+2]))<<2) & 0x03FC;
                pa[pIdx+2] = pa[pIdx+2] | (short)((ra[bIdx+4]>>4) & 0x0003);

                pa[pIdx+3] = (((short)(ra[bIdx+3]))<<2) & 0x03FC;
                pa[pIdx+3] = pa[pIdx+3] | (short)((ra[bIdx+4]>>6) & 0x0003);

                bIdx = bIdx + 5;
                pIdx = pIdx + 4;
            }
            break;

        case RAW12:
            for(bIdx=0, pIdx=0; bIdx < (in_size-3); ) {
                pa[pIdx+0] = (((short)(ra[bIdx+0]))<<4) & 0x0FF0;
                pa[pIdx+0] = pa[pIdx+0] | (short)((ra[bIdx+2]>>0) & 0x000F);

                pa[pIdx+1] = (((short)(ra[bIdx+1]))<<4) & 0x0FF0;
                pa[pIdx+1] = pa[pIdx+1] | (short)((ra[bIdx+2]>>4) & 0x000F);

                bIdx = bIdx + 3;
                pIdx = pIdx + 2;
            }
            break;

        case RAW14:
            for(bIdx=0, pIdx=0; bIdx < (in_size-7); ) {
                pa[pIdx+0] = (((short)(ra[bIdx+0]))<<6) & 0x3FC0;
                pa[pIdx+0] = pa[pIdx+0] | (short)((ra[bIdx+4]>>0) & 0x003F);

                pa[pIdx+1] = (((short)(ra[bIdx+1]))<<6) & 0x3FC0;
                pa[pIdx+1] = pa[pIdx+1] | (short)((ra[bIdx+4]>>6) & 0x0003);
                pa[pIdx+1] = pa[pIdx+1] | (short)((ra[bIdx+5]>>0) & 0x000F);

                pa[pIdx+2] = (((short)(ra[bIdx+2]))<<6) & 0x3FC0;
                pa[pIdx+2] = pa[pIdx+2] | (short)((ra[bIdx+5]>>4) & 0x000F);
                pa[pIdx+2] = pa[pIdx+2] | (short)((ra[bIdx+6]>>0) & 0x0003);

                pa[pIdx+3] = (((short)(ra[bIdx+3]))<<6) & 0x3FC0;
                pa[pIdx+3] = pa[pIdx+3] | (short)((ra[bIdx+6]>>2) & 0x003F);

                bIdx = bIdx + 7;
                pIdx = pIdx + 4;
            }
            break;
    }
    fwrite(pa, 1, out_size, pixel_fb);

    fclose(raw_fb);
    fclose(pixel_fb);

    printf("convert finished !!! \r\n");
    return 0;
}

/* use follwoing command to compile 
gcc -o raw2raw raw2raw.c
 */
