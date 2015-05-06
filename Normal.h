/*
 *  Normal.h
 *  Raytracer
 *
 *  Created by Amber on 3/11/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef NORMAL_H
#define NORMAL_H

#include "Texture.h"

class Normal : public Texture
{
private:
	Image* image;
	
	color _getTexel(int px, int py)
	{
		if (image == NULL) {
			return color();
		}
		
		switch (mode) {
			case CLAMP:
				px = max(0, min(px, resX - 1));
				py = max(0, min(py, resY - 1));
				break;
			case REPEAT:
				px = (resX + px % resX) % resX;
				py = (resY + py % resY) % resY;
				break;
			default:
				break;
		};
		
		return image->getPixel(px, py);
	}
	
	color _getSmoothTexel(float u, float v)
	{
		float px = resX * u + 0.5;
		float py = resY * v + 0.5;
		
		PAIR uv[2][2];
		uv[0][0] = PAIR(floor(px - 1), floor(py - 1));
		uv[0][1] = PAIR(floor(px - 1), floor(py));
		uv[1][0] = PAIR(floor(px), floor(py - 1));
		uv[1][1] = PAIR(floor(px), floor(py));
		
		color tex[2][2];
		tex[0][0] = _getTexel((int)uv[0][0].first, (int)uv[0][0].second);
		tex[0][1] = _getTexel((int)uv[0][1].first, (int)uv[0][1].second);
		tex[1][0] = _getTexel((int)uv[1][0].first, (int)uv[1][0].second);
		tex[1][1] = _getTexel((int)uv[1][1].first, (int)uv[1][1].second);
		
		float fx = fabs(px - (int)px);
		float fy = fabs(py - (int)py);
		
		color result = tex[0][0]*(1-fx)*(1-fy) + tex[1][0]*fx*(1-fy) + tex[0][1]*(1-fx)*fy + tex[1][1]*fx*fy;
		return result;
		
	}
	
	
public:
	
	float factor;
	
	Normal(int resX, int resY) : Texture()
	{
		resX = resX;
		resY = resY;
		factor = 1.0;
		image = new Image(resX, resY);
	};
	
	Normal(char* fileName) : Texture(), image(NULL), factor(1.0)
	{
		resX = 1;
		resY = 1;
		Image* img = new Image(1,1);
		if (img->LoadImage(fileName)) {
			image = img;
			resX = img->getWidth();
			resY = img->getHeight();
		}
		else {
			delete img;
		}
	}
	
	~Normal()
	{
		if (image) {
			delete image;
		}
	}
	
	void setNormalFactor(float f) { factor = f; }	
	
	color getTexel(float u, float v)
	{
		switch (getInterpolationMode()) {
			case NEAREST:
				return _getTexel(int(resX * u), int(resY * v));
				break;
			case LINEAR:
				return _getSmoothTexel(u, v);
				break;
			default:
				break;
		};
		return color();
	}
	
	int getWidth()
	{
		return resX;
	}
	
	int getHeight()
	{
		return resY;
	}
};
#endif