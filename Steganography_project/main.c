/*
Name:Dinesh A

Date:

Description: Steganography
Sample input   :1.For Encoding: ./a.out -e beautiful.bmp secret.txt stego_image.bmp
2.For Decoding: ./a.out -d steged_beautiful.bmp decode_msg.txt

 * Output      :1->encoding
 * 	    	Selected encoding............................!
 * 		Read and Validate encode arguments is  a success
 * 		<---------------Started Encoding-------------->
 *      	Open files is a success
 *      	width = 1024
 *		height = 768
 *		Check capacity is a success
 *      	Copied bmp header successfully
 *		The Magic string is encoded
 *		Encoded secret file extn size
 *		Encoded secret file extn successfully
 *		Secret file size is encoded successfully
 *		Encoded secret file data
 *		Copied remaining data
 *		<------------Encoded successfully-------------->
 *
 *               2->decoding
 *	    	Selected decoding...........................!
 *		output file is not mentioned. Creating decoded_msg.txt as default
 *		Read and validate decode arguments is a success
 *		<-------------Started Decooding----------------->
 *		Open files is a success
 *		Decoded Magic string successfully
 *		Decoded File Extension size successfully
 *		Decoded Extension successfully
 *		Decoded File size successfully
 *		Decoded File data Successfully
 *		<-------------Decoded Successfully------------->
 */

#include <stdio.h>
#include <string.h>
#include "types.h"
#include "common.h"
#include "encode.h"
#include "decode.h"
#include "types.h"



int main(int argc, char *argv[])
{
    EncodeInfo encInfo;
    uint img_size;
    DecodeInfo decInfo;
    /* checking command line is passed or not */
    if(argc == 1 )
    {
	printf("please pass arguments through command line\n");
    }
    /* enabeling the encoding */
    if(check_operation_type(argv) == e_encode)
    {
	printf("Selected Encoding------------!!\n");
	/* checking the read and validation  for ecnoding */
	if(read_and_validate_encode_args(argv,&encInfo) == e_success)
	{
	    printf("read and validation is success\n");
	    printf("<------------------Started Encoding----------------->\n");



 /* Encoding part */



	    if(do_encoding(&encInfo) == e_success)
	    {
		printf("<-------------------Encoded done------------------->\n");
	    }
	    else
	    {
		printf("Failed to encode\n");
	    }
	}
	else
	{
	    printf("read and validation is failed\n");
	}
    }

    /* enabeling the decoding */
    else if(check_operation_type(argv) == e_decode)
    {
	printf("Selected decoding--------------!!\n");
	/* checking the read and validation  for decoding */
	if(read_and_validate_decode_args(argv, &decInfo) == e_success)
	{
	    printf("read and validation of decode is success\n");
	    printf("<------------------Started Decoding----------------->\n");
	    /* decoding part */
	    if(do_decoding(argv, &decInfo)== e_success)
	    {
		printf("<-------------------Decoded done------------------->\n");
	    }
	    else
	    {
		printf("Failed to decode\n");
	    }
	}
	else
	{
	    printf("read and validation is failed\n");
	}
    }
    return 0;
}



