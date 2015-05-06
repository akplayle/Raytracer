/*
 *  Ray.h
 *  Raytracer
 *
 *  Created by Amber on 2/6/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef RAY_H
#define RAY_H 

#include "StdDefs.h"
#include <Eigen/Dense>

using namespace Eigen;

class Primitive;
class Object;

class Ray
{
public:
	Vector3f origin;
	Vector3f direction;
	
	float t, tmin, tmax;
	
	Primitive *hit;
	Object *o;
	
	float u;
	float v;
	
	unsigned int bounces;
	float alpha;
	
    float seed;
	
	Ray()
	{
		o = NULL;
		hit = NULL;
		
		tmin = 2.0;
		tmax = Infinity;
		t = tmax;
		
		u = v = 0.0f;
		bounces = 0;
		alpha = 0.0f;
        seed = 0;
	}
	
	Ray(const Ray& r)
	{
		origin = r.origin;
		direction = r.direction;
		t = r.t;
		tmin = r.tmin;
		tmax = r.tmax;
		hit = r.hit;
		o = r.o;
		u = r.u;
		v = r.v;
		bounces = r.bounces;
		alpha = r.alpha;
        seed = r.seed;
	}
	
	Ray& operator = (const Ray& r)
	{
		origin = r.origin;
		direction = r.direction;
		t = r.t;
		tmin = r.tmin;
		tmax = r.tmax;
		hit = r.hit;
		o = r.o;
		u = r.u;
		v = r.v;
		bounces = r.bounces;
		alpha = r.alpha;
        seed = r.seed;
		return *this;
	}
	
};

#endif