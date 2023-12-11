#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types
#include "common.h" // Contains user defined types
#include <string.h> // Contains user defined types

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /* output file name */
    char *output_fname;
    FILE *fptr_output;
    int output_file_extn_size;
    char *output_file_extn ;
    int output_file_size;
} DecodeInfo;


/* Decoding function prototype */

/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the encoding */
Status do_decoding(char *argv[],DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_decode_files(DecodeInfo *decInfo);

/* Store Magic String */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Encode secret file extenstion */
Status decode_secret_file_extn(char *argv[], DecodeInfo *decInfo);

/* Encode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

int decode_num_from_img(char *, FILE *);

/* Encode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Encode function, which does the real encoding */
Status decode_data_to_image(char *data, int size, FILE *fptr_stego_image);

/* Encode a byte into LSB of image data array */
char decode_byte_to_lsb(char *image_buffer);

#endif

