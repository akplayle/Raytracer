/*
 *  PointLight.h
 *  Raytracer
 *
 *  Created by Amber on 2/8/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include <Eigen/Dense>
#include "Light.h"

using namespace Eigen;

class PointLight : public Light
{
public:

	PointLight() : Light(){};
	PointLight(Vector3f position, color c, float brightness, char name[30]) 
		: Light(position, c, brightness, name){};

	unsigned int getNumberOfRays()
	{
		return 1;
	}
	
	bool Intersect(Ray &r)
	{
		float radius = 5;
		
		Vector3f d = position - r.origin;
		float b = r.direction.dot(d);
		float c = d.dot(d) - powf(radius, 2);
		float delta = b*b - c;
		//cout << "b: " << b << " c: " << c << " delta: " << delta << endl;
		
		if (b >= 0 && delta >= 0) {
			r.t = b - sqrt(delta);
			return true;
		}
		return false;	
	}
		
	
	bool Illuminate(Ray &r, int index)
	{
		r.direction = position - r.origin;
		r.hit = NULL;
		r.t = r.direction.norm() - Epsilon;
		r.direction = r.direction.normalized();
		
		
		float att = 1.0f / (r.t * r.t);
		att = 1.0;
		
		intensity = att * c * brightness;
		return true;		
	};
};

#endif