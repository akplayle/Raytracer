/*
 *  Texture.h
 *  Raytracer
 *
 *  Created by Amber on 3/9/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef TEXTURE_H
#define TEXTURE_H

using namespace std;

class Texture
{
public:
	string name;
	
	int resX;
	int resY;
	
	enum Mode {
		CLAMP = 0,
		REPEAT = 1
	};
	
	enum InterpolationMode {
		NEAREST = 0,
		LINEAR = 1
	};
	
	Texture() : mode(REPEAT), interpolationMode(NEAREST)
	{
	};
	
	virtual ~Texture()
	{
	}
	
	Mode getWrapMode() const {return mode;}
	
	void setWrapMode(Mode m) {mode = m;}
	
	InterpolationMode getInterpolationMode() const {return interpolationMode;}
	
	void setInterpolationMode(InterpolationMode m) {interpolationMode = m;}
	
	virtual color getTexel(float u, float v) = 0;
	inline color getTexel(const PAIR& uv) {return getTexel(uv.first, uv.second);}
	
	virtual int getWidth() = 0;
	virtual int getHeight() = 0;
	
protected:
	Mode mode;
	InterpolationMode interpolationMode;
	
	
};
#endif