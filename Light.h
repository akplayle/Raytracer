/*
 *  Light.h
 *  Raytracer
 *
 *  Created by Amber on 2/6/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef LIGHT_H
#define LIGHT_H

#include <Eigen/Dense>
#include "Color.h"
#include "Ray.h"
#include "Texture.h"

using namespace Eigen;

class Scene;

class Light
{
protected:
	friend class Scene;
	Scene* s;
public:
	Vector3f position;
	char name[30];
	color c;
	float brightness;
	color intensity;
    
    Texture* image;
	
	Light()
	{
		s = NULL;
		position << 0.0, 0.0, 0.0;
		c.set(1.0, 1.0, 1.0);
		brightness = 1.0;
		strcpy(name, "");
		intensity.set(0.0, 0.0, 0.0);
	};

	Light(Vector3f position, color c, float brightness, char name[30]) 
	{
		s = NULL;
		position = position;
		c = c;
		brightness = brightness;
		strcpy(name, name);
		intensity = c * brightness;
	
	};

	virtual ~Light() {}
	
	virtual bool Intersect(Ray &r) = 0;
	
	virtual bool Illuminate(Ray &r, int index) = 0;
	
	virtual unsigned int getNumberOfRays() = 0;
	
	
};

#endif