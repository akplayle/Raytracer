/*
 *  DirectionalLight.h
 *  Raytracer
 *
 *  Created by Amber on 3/2/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "Light.h"

class DirectionalLight : public Light
{
public:
	unsigned int raycount;
	Vector3f normal;
	
	float randI;
	float randJ;
	
	DirectionalLight() : Light()
	{
		raycount = 9;
		normal << 0.0, -1.0, -1.0;
		normal = normal.normalized();
		position = INFINITY * normal * -1;
		
		randI = frand();
		randJ = frand();
		
	};
	
	DirectionalLight(color c, float brightness, char name[30], Vector3f normal) 
	: Light(), normal(normal)
	{
		c = c;
		brightness = brightness;
		strcpy(name, name);
		normal.normalized();
		position =  INFINITY * normal * -1;
		
		randI = frand();
		randJ = frand();
		
	};
	
	
	virtual Vector3f getNormal(Ray& r)
	{
		return normal;
	}
	
	unsigned int getNumberOfRays() { return raycount; }
	
	void setNumberOfRays(int i) { raycount = i;}
	
	
	bool Illuminate(Ray &r, int index)
	{
		
		float att = 1.0f / (r.t * r.t);
		att = 1.0;
		intensity = att * c * brightness;

		
		if (Intersect(r)) {
			return true;
		}
		else {
			return false;
		}

		
	};	
	
	
	bool Intersect(Ray& r)
	{
		position = (r.tmax-Epsilon) * normal * -1;
		float denom = r.direction.dot(normal);
		if (denom != Epsilon) {
			float d = (r.origin - position).dot(normal);
			r.t = -d/denom;
			if (r.t >= Epsilon) {
				return true;
			}
		}
		return false;
	};
	
};
#endif