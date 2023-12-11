
#include <stdio.h>
#include "types.h"
#include "common.h"
#include "encode.h"


uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    //Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    //Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    //Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);
	return e_failure;
    }

    // No failure return e_success
    return e_success;
}
//Function definition to check operation type
OperationType check_operation_type(char *argv[])
{
    //checking Encode
    if(strcmp(argv[1],"-e") == 0)
	return e_encode;
    //checking Decode
    else if(strcmp(argv[1],"-d") == 0)
	return e_decode;
    else
	return e_unsupported;

}
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    //checking whether passed image file is bmp file or not
    if(strcmp(strstr(argv[2],".bmp"), ".bmp") == 0)
	//strcpy(encInfo->src_image_fname,argv[2]);
	encInfo->src_image_fname=argv[2];
    else
	return e_failure;

    //checking Argument or not
    if(strcmp(strstr(argv[3],".txt"), ".txt") == 0)
	encInfo->secret_fname=argv[3];
    else
	return e_failure;

    //checking whether output file is passed or not
    if(argv[4]==NULL)
    {
	//If output file is not passed then creating a new file with name stego_image.bmp
	encInfo->stego_image_fname="default.bmp";
    }
    else
	encInfo->stego_image_fname=argv[4];
    return e_success;
}
//Function definition to get file size
uint get_file_size(FILE *fptr)
{
    //seek file pointer to 0
    fseek(fptr, 0L, SEEK_END);
    return ftell(fptr);
}


//funtion definition to check capacity of beautiful.bmp file
Status check_capacity(EncodeInfo *encInfo)
{
    int image_capacity;
    int size_secret_file;
    //Funtion call to get image size for bmp and storing return value
    image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

    //calling the function to get file size and storing return value
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    if(image_capacity > (54 + 16 + 32+ 32 + 8* size_secret_file))  
	return e_success;
    else
	return e_failure;
}

//Function definition for copying bmp header to stego_image.bmp file
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char str[54];

    //seek file pointer to 0
    fseek(fptr_src_image, 0L, SEEK_SET);

    //Reading 54 bytes of src image to an array
    fread(str, 54, 1, fptr_src_image);

    //Writing 54 bytes from array to destination i.e stego file
    fwrite(str, 54, 1, fptr_dest_image);
    return e_success;
}
//Function definitin for encoding magic string to output image file
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    encode_data_to_image((char*) magic_string,strlen(magic_string),encInfo->fptr_src_image,encInfo->fptr_stego_image);
    return e_success;
}

//Function defintion for encoding data to output image file
//Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char str[8];
    for(int i = 0; i < size; i++)
    {
	fread(str, 8, 1, fptr_src_image);
	encode_byte_tolsb(data[i], str);
	fwrite(str, 8, 1, fptr_stego_image);
    }
    return e_success;
}

//Function defintion for encoding each byte of magic string to lsb
Status encode_byte_tolsb(char data, char *str)
{
    //IMPORTANT CODE
    for(int i = 0; i < 8; i++)
    {
	if((data >> (7 - i) & 1))
	{
	    str[i] = str[i] | 1;
	}
	else
	    str[i] = str[i] & ~1;
    }

}

//Function definition for encoding size to output image file
Status encode_size_to_lsb(int size, char *image_buffer)
{
    unsigned int data = 8;
    for(int i = 0; i < 32; i++)
    {
	if((size >> (31 - i)) & 1)
	{
	    image_buffer[i] = image_buffer[i]  | 1;
	}
	else
	    image_buffer[i] = image_buffer[i] & ~1;
    }
}

//Function definition for encoding secret file extention size output image file
Status encode_secret_file_extn_size(int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    printf("%d\n",size);
    char str[32];
    fread(str, 32, 1, fptr_src_image);
    encode_size_to_lsb(size, str);
    fwrite(str, 32, 1, fptr_stego_image);
    return e_success;
}


//Function definition for encoding secret file extension to output image file
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    encode_data_to_image((char *)file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;
}

//Function definition for encoding secret file size
Status encode_secret_file_size(long int size, EncodeInfo *encInfo)
{
    char str[32];

    // printf("%d",size);
    fread(str, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb( size, str);
    fwrite(str, 32, 1, encInfo->fptr_stego_image);
    return e_success;
}

//Functuon definition foe encoding secret file data to output image file
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    fseek(encInfo->fptr_secret, 0, SEEK_SET);

    char str[encInfo->size_secret_file];

    fread(str, encInfo->size_secret_file, 1, encInfo->fptr_secret);
    encode_data_to_image(str, strlen(str), encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;
}

//function definition for copying remaining data of beautiful.bmp file to output image file
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    while(( fread ( &ch, 1, 1, fptr_src) ) > 0)
    {
	fwrite( &ch, 1, 1, fptr_dest);
    }
    return e_success;
}
/*Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
Status do_encoding(EncodeInfo *encInfo)
{
    //Function call to check capacity function and checking comdition whether success or not
    if(open_files(encInfo) == e_success)
    {
	printf("INFO : Open files is Success\n");

	//Function call to check capacity function & checking whether condition is success or not
	if(check_capacity(encInfo) == e_success)
	{
	    printf("INFO : Check Capacity is Success\n");

	    //Function call for checking bmp header function and checking condition whether success or not
	    if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
	    {
		printf("INFO : Copy bmp header is Success\n");

		//Function call to encode magic string & checking whether condition success or not
		if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)
		{
		    printf("Magic String is Copied Successfully\n");


		    strcpy(encInfo->extn_secret_file,strstr(encInfo->secret_fname,"."));

		    //Function call for encode secret file extention size  and checking condition whether success or not
		    if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
		    {
			printf("INFO : Sectet file data is encoded\n");

			//Copying secret file extention
			if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
			{
			    printf("Secret file extention is encoded\n");

			    // Function call encode secret file extention function and checking condition whether success or not
			    if(encode_secret_file_size(encInfo->size_secret_file, encInfo ) == e_success)
			    {
				printf("secret file size is encoded\n");
				//Function call to encode secret file size function & checking the condition is success or not
				if(encode_secret_file_data(encInfo) == e_success)
				{
				    printf("Secret file data is encoded\n");
				    //Function call for copying remaing image data function & checking contion success or not
				    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
				    {
					printf("Remaining image data is copied\n");
					return e_success;
				    }
				    else
				    {
					printf("Remaining image data is not copied\n");
				    }
				}
				else
				{
				    printf("Secret file data is not encoded\n");
				}
			    }
			    else
			    {
				printf("Secret file size is not encoded\n");
			    }
			}
			else
			{
			    printf("Secret file extention is not encoded\n");
			}
		    }
		    else
		    {
			printf("Secret file extention size is not encoded\n");
			return e_failure;
		    }
		}
		else
		{
		    printf("Magic string is not copied\n");
		    return e_failure;
		}
	    }
	    else
	    {
		printf("Copying bmp header is not Sucessful\n");
		return e_failure;
	    }
	}
	else
	{
	    printf("Check capacity is failed\n");
	    return e_failure;
	}
    }
    else
    {
	printf("Open files is failed\n");
	return e_success;
    }
}
