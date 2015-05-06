/*
 *  Image.h
 *  Raytracer
 *
 *  Created by Amber on 3/9/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef IMAGE_H
#define IMAGE_H

#include "Color.h"
#include <cassert>

class Image
{
	int resX;
	int resY;
	color *pixel;
	
public:
	Image(int resX, int resY) : resX(resX), resY(resY)
	{
		assert (resX > 0 && resY > 0);
		pixel = new color[resX * resY];
	}
	
	Image() : resX(0), resY(0)
	{
		pixel = NULL;
	}
	
	~Image()
	{
		if (pixel) {
			delete[] pixel;
		}
	};
	
	color * operator[](int y)
	{
		assert(pixel != NULL);
		return pixel + y * resX;
	};
	
	int getWidth() const { return resX; }
	int getHeight() const { return resY; }
	
	color& getPixel(int x, int y)
	{
		if (x < 0 || y < 0) {
			printf("x = %d, y = %d\n", x, y);
		}
		assert (pixel != NULL && x >=0 && y >=0);
		assert(x < resX && y < resY);
		
		return pixel[y * resX + x];
	};
	
	void setPixel (const color& p, int x, int y)
	{
		assert(pixel != NULL && x >=0 && y >= 0);
		assert(x < resX && y < resY);
		
		pixel[y * resX + x] = p;
	};
	
	bool ReadPNG(const char* fileName);
	
	inline bool LoadImage(const char* fileName) {return ReadPNG(fileName); };
	
	void WritePNG(const char* fileName);
	
	
};
#endif