/*
 *  Image.cpp
 *  Raytracer
 *
 *  Created by Amber on 3/9/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include "Image.h"
#include "PNGhelp.h"

bool Image::ReadPNG(const char* fileName)
{
	int channels, bit_depth;
	int color_type = PNG_COLOR_TYPE_RGB;
	unsigned char* buffer = NULL;
	
	read_png(fileName, buffer, resX, resY, channels, bit_depth, color_type, true);
	if (!(color_type == PNG_COLOR_TYPE_GRAY 
		|| color_type == PNG_COLOR_TYPE_RGB 
		|| color_type == PNG_COLOR_TYPE_RGBA)) {
		cout << "Image::ReadPNG(" << fileName << "): not supported color type" << endl;
		if (buffer) {
			free(buffer);
			return false;
		}
	}
	
	if (bit_depth != 8) {
		cout << "Image::ReadPNG(" << fileName << "): not supported bit depth" << endl;
		if (buffer) {
			free(buffer);
		}
		return false;
	}
	
	cout << "Read PNG File: " << fileName << endl;
	
	bool grey = color_type == PNG_COLOR_TYPE_GRAY;
	bool rgba = color_type == PNG_COLOR_TYPE_RGBA;
	//bool rgb = color_type == PNG_COLOR_TYPE_RGB;
	
	if (pixel) {
		delete [] pixel;
	}
	
	pixel = new color[resX * resY];
	
	for (int y = 0; y < resY; y++) {
		for (int x = 0; x < resX; x++) {
			color v = color(float(buffer[(y*resX + x) * channels + (grey ? 0 : 0)]) / 255.0,
							float(buffer[(y * resX + x) * channels + (grey ? 0 : 1)]) / 255.0,
							float(buffer[(y * resX + x) * channels + (grey ? 0 : 2)]) / 255.0,
							rgba ? float(buffer[(y * resX + x) * channels + (grey ? 0 : 3)]) / 255.0 : 1);
			
			setPixel(v, x, y);
			
		}
	}
	
	if (buffer) {
		free(buffer);
	}
	
	return true;
}



void Image::WritePNG(const char* fileName)
{
	int channels = 3;
	int bit_depth = 8;
	int color_type = PNG_COLOR_TYPE_RGB;
	
	unsigned char* buffer = (unsigned char*)malloc(resX * resY * sizeof(unsigned char) * channels);
	
	for (int y = 0; y < resY; y++) {
		for (int x = 0; x < resX; x++) {
			const color& p = getPixel(x, y);
			
			buffer[(y*resX + x) * channels + 0] = (unsigned char)(255 * p.Red());
			buffer[(y*resX + x) * channels + 1] = (unsigned char)(255 * p.Green());
			buffer[(y*resX + x) * channels + 2] = (unsigned char)(255 * p.Blue());
		}
	}
	
	write_png(fileName, buffer, resX, resY, channels, bit_depth, color_type, true);
	
	if (buffer) {
		free(buffer);
	}
	
}
