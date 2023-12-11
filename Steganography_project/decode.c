
/* including all the necessary files required*/
#include "common.h"
#include "types.h"
#include <stdio.h>
#include <string.h>
#include "decode.h"
#include <stdlib.h>

/* Function definition of read and validate function */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    char *temp = strstr(argv[2],".bmp");
    if(temp == NULL)
    {
	return e_failure;
    }
    else
    {
	decInfo->stego_image_fname = argv[2];
    }

    return e_success;
}

/* Function definition of open decode files */
Status open_decode_files(DecodeInfo *decInfo)
{
    //open the .bmp file in read mode and store in the file pointer
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

	return e_failure;
    }
    return e_success;
}

/*char decode_byte_to_lsb  get the each char from the secreat file*/
char decode_byte_to_lsb(char *image_buffer)
{
    char data = 0;
    for(int i = 0; i < 8; i++)
    {
	if(image_buffer[i] & 1)
	{
	    data = data | (1 << (7- i));
	}
    }
    return data;
}

/* decode_data_to_image
 *decode all the char from the stego image*/
Status decode_data_to_image(char *data, int size, FILE *fptr_stego_image)
{
    char image_buffer[8];
    for(int i = 0; i < size-1; i++)
    {
	fread(image_buffer,1,8,fptr_stego_image);
	//Function call
	data[i] = decode_byte_to_lsb(image_buffer);
    }
    data[size-1] = '\0';

    return e_success;
}

/*decode the magic string
 *get the size of the magic string
 *ignore 54 bytes of header in stego image
 *set the pointer to the 54th position
 *decode the magic string
 *compare the data with the magic string to check whether magic string matched or not*/

/* Decoding magic string */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{

    int size = (strlen(magic_string)+1);
    char data[size];
    /* Setting stego file pointer at 54 byte position */
    fseek(decInfo->fptr_stego_image,54L,SEEK_SET);
    //function call
    decode_data_to_image(data, size , decInfo->fptr_stego_image);
    if(strcmp(data,magic_string) == 0)
    {
	return e_success;
    }
    return e_failure;
}
/*decode the integer from the ponter*/
int decode_num_from_img(char *image_buffer, FILE *fptr)
{
    int size = 0;
    for (int i = 0; i < 32; i++)
    {
	if (image_buffer[i] & 1)
	    size = size | 1 << (31 - i);
    }
    return size;
}

/*get the secret file extention*/
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char image_buffer[32] = {};
    fread(image_buffer, 1, 32, decInfo -> fptr_stego_image);
    //Function call
    decInfo -> output_file_extn_size = decode_num_from_img(image_buffer, decInfo -> fptr_stego_image);
    return e_success;

}

/*decode_secret_file_extn
 *check for the 3rd argument passed or not
 *if passed store the file name add with the extention
 *if not passed create a default file with the given secreat file extention
 */

/* Function definition of decode secret file extension */
Status decode_secret_file_extn(char *argv[], DecodeInfo *decInfo)
{
    decInfo -> output_file_extn = malloc(1 * ((decInfo -> output_file_extn_size) + 1));

    decode_data_to_image(decInfo -> output_file_extn, decInfo -> output_file_extn_size + 1, decInfo -> fptr_stego_image);

    char name[32] = "default";
    if(argv[3] == NULL)
    {
	strcat(name,decInfo->output_file_extn);
	decInfo->output_fname = name;
    }
    else
    {
	decInfo->output_fname = argv[3];
	strcat(decInfo->output_fname,decInfo->output_file_extn);
    }
    decInfo->fptr_output = fopen(decInfo->output_fname,"w");
    if (decInfo->fptr_output == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->output_fname);

	return e_failure;
    }
    return e_success;
}

/*decode the size of the secret file*/
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char image_buffer[32] = {};
    fread(image_buffer, 1, 32, decInfo -> fptr_stego_image);

    //Function call
    decInfo -> output_file_size = decode_num_from_img(image_buffer, decInfo -> fptr_stego_image);

    return e_success;
}

/*decode the secret file data
 *store the data from the stego file into a image buffer
 *call the function decode_byte_to_lsb to the the each character
 *and write the data to a output file*/

/* Function definition of secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char image_buffer[8] = {};

    for (int i = 0; i < decInfo -> output_file_size; i++)
    {
	fread(image_buffer, 1, 8, decInfo -> fptr_stego_image);
	char data = decode_byte_to_lsb(image_buffer);
	fwrite(&data, 1, 1, decInfo -> fptr_output);
    }
    return e_success;
}

/*do_decoding
 *call the function to open all the files
 *call the function to decode magic string
 *call the function to decode the size of secret file extention
 *call the function to decode the char of the secret file extention
 *call the function to decode the size of secret file size
 *call the function to decode the all the contents of secret file*/

/* Function definition of do decoding */
Status do_decoding(char *argv[], DecodeInfo *decInfo)

{
    /* opening files for decoding */
    if(open_decode_files(decInfo) == e_success)
    {
	printf("open_decode_files is SUCCESS\n");

	/* Decoding magic string */
	if(decode_magic_string(MAGIC_STRING,decInfo) == e_success)
	{
	    printf("magic string is decoded successfully\n");

	    /*Decoding file extension size */
	    if (decode_secret_file_extn_size(decInfo) == e_success)
	    {
		printf("secret file extn size is decoded successfully\n");

		/*Decoding file extension  */
		if (decode_secret_file_extn(argv, decInfo) == e_success)
		{
		    printf("secret file extn is not decoded successfully\n");

		    /*Decoding secret file size */
		    if (decode_secret_file_size(decInfo) == e_success)
		    {
			printf("secret file size is decoded successfully\n");

			/*Decoding secret file data */
			if (decode_secret_file_data(decInfo) == e_success)
			{
			    printf("secret file data is decoded successfully\n");
			    return e_success;
			}
			else
			{
			    printf("secret file data is not decoded successfully\n");
			    return e_failure;
			}
		    }
		    else
		    {
			printf("secret file size is not decoded successfully\n");
			return e_failure;
		    }
		}
		else
		{
		    printf("secret file extn is not decoded successfully\n");
		    return e_failure;
		}
	    }
	    else
	    {
		printf("secret file extn size is not decoded successfully\n");
		return e_failure;
	    }
	}
	else
	{
	    printf("magic string is not decoded successfully\n");
	}
    }
    else
	printf("open_decode_files is FAILURE\n");
    return e_success;
}
