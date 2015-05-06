/*
 *  PNGhelp.h
 *  Raytracer
 *
 *  Created by Amber on 3/9/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __LIBPNG_HELP__
#define __LIBPNG_HELP__

#include <libpng/png.h>
#include <cstdio>
#include <iostream>
#include <fstream>


#ifndef png_infopp_NULL
#define png_infopp_NULL (png_infopp)NULL
#endif
#ifndef png_voidp_NULL
#define png_voidp_NULL (png_infopp)NULL
#endif

#define PNG_BYTES_TO_CHECK 8

using namespace std;

int check_if_png(const char *fileName, FILE **fp)
{
	png_byte buf[PNG_BYTES_TO_CHECK];
	
	if ((*fp = fopen(fileName, "rb")) == NULL) {
		return 0;
	}
	
	if (fread(buf, 1, PNG_BYTES_TO_CHECK, *fp) != PNG_BYTES_TO_CHECK) {
		return 0;
	}
	
	bool res = (!png_sig_cmp(buf, (png_size_t)0, PNG_BYTES_TO_CHECK));
	fclose(*fp);
	return res;
}

void read_png(const char* fileName, unsigned char*& buffer,
			  int& width, int& height, int& channels,
			  int& bit_depth, int& color_type, bool flip_vert)
{
	png_structp png_ptr;
	png_infop info_ptr;
	
	FILE *fp;
	bool check = check_if_png(fileName, &fp);
	
	if (!check) {
		std::cerr << "This is not a PNG file!" << endl;
		exit(1);
	}
	
	fp = fopen(fileName, "rb");
	
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
	if (png_ptr == NULL) {
		fclose(fp);
		std::cerr << "Error initializing png_ptr!" << endl;
		exit(1);
	}
	
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		std::cerr << "Error initializing info_ptr!" << endl;
		exit(1);
	}
	
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
		fclose(fp);
		std::cerr << "Error initializing png_ptr!" << endl;
	}
	
	png_init_io(png_ptr, fp);
	
	png_set_sig_bytes(png_ptr, 0);
	
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, png_voidp_NULL);
	
	width = png_get_image_width(png_ptr, info_ptr);
	height = png_get_image_height(png_ptr, info_ptr);
	channels = png_get_channels(png_ptr, info_ptr);
	
	bit_depth = png_get_bit_depth(png_ptr, info_ptr);
	color_type = png_get_color_type(png_ptr, info_ptr);
	png_uint_32 valid = 0;
	png_get_valid(png_ptr, info_ptr, valid);
	
	png_bytep* row_pointers = (png_bytep*) malloc(height * sizeof(png_bytep));
	row_pointers = png_get_rows(png_ptr, info_ptr);
	
	const int c = width * channels * bit_depth / 8;
	buffer = (unsigned char*) malloc(sizeof(unsigned char) * channels * width * height * bit_depth / 8);
	
	if (!flip_vert) {
		for (int y = 0; y < height; ++y) {
			memcpy((void*)&(buffer[y*c]), (void*)(row_pointers[y]), c);
		}
	}
	else {
		for (int y = 0; y < height; ++y) {
			memcpy((void*)&(buffer[(height - 1 - y) * c]), (void*)(row_pointers[y]), c);
		}
	}
	
	png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
	fclose(fp);
	
}

void write_png(const char *fileName, unsigned char* buffer,
			   int width, int height, int channels, 
			   int bit_depth, png_uint_32 color_type, bool flip_vert)
{
	FILE *fp;
	png_structp png_ptr;
	png_infop info_ptr;
	fp = fopen(fileName, "wb");
	if (fp == NULL) {
		std::cerr << "Error opening file" << endl;
        exit(1);
	}
	
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
	if (png_ptr == NULL) {
		fclose(fp);
		std::cerr << "Error allocating png_ptr!" << endl;
		exit(1);
	}
	
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fclose(fp);
		png_destroy_write_struct(&png_ptr, png_infopp_NULL);
		std::cerr << "Error allocating info ptr!" << endl;
		exit(1);
	}
	
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		/* If we get here, we had a problem reading the file */
		fclose(fp);
		png_destroy_write_struct(&png_ptr, &info_ptr);
		exit(1);
    }
 
    png_init_io(png_ptr, fp);

    png_set_IHDR(png_ptr, info_ptr, (png_uint_32)width, (png_uint_32) height,
         (png_byte) bit_depth, (png_byte) color_type, PNG_INTERLACE_NONE,
         PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT );
 
    //setup row data structure from buffer
    
    png_bytep row_pointers[height];
    const int c=width*channels*bit_depth/8;
    if (!flip_vert)
	{
		for (int y=0;y<height;++y)
		{
			row_pointers[y]=(png_bytep) &(buffer[y*c]);
		}
	}
	else
	{
		for (int y=0;y<height;++y)
		{
			row_pointers[height-1-y]=(png_bytep) &(buffer[y*c]);
		}
	}
    //raise(5);
    
    png_set_rows(png_ptr, info_ptr, row_pointers);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, png_voidp_NULL);
 
    png_destroy_write_struct(&png_ptr, &info_ptr);
	fclose(fp);
	
}

#endif