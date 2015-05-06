/*
 *  Cone.h
 *  Raytracer
 *
 *  Created by Amber on 3/2/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef CONE_H
#define CONE_H

#include <Eigen/Dense>

#include "Primitive.h"

using namespace Eigen;

class Cone : public Primitive
{	
	
	
public:
	
	float s0, s1, s2;
	Vector3f n0, n1, n2;
	
	Cone()
	{
		position << 0.0, 0.0, 0.0;
		s0 = s1 = s2 = 1.0;
		n0 << 1.0, 0.0, 0.0;
		n1 << 0.0, 1.0, 0.0;
		n2 << 0.0, 0.0, 1.0;
	}
	
	
	virtual ~Cone(){};

    Box calcBounds()
	{
		Box b = Box();
		b.Extend(position + Vector3f(-s0, -s1, -s2));
		b.Extend(position + Vector3f(s0, s1, s2));
		return b;
	}
	
	virtual bool Intersect(Ray &r)
	{
		
		
		Matrix3f M; //Transformaion Matrix
		Vector3f x = n0.normalized()/s0;
		Vector3f y = n1.normalized()/s1;
		Vector3f z = n2.normalized()/s2;
		M <<	x(0), x(1), x(2),
		y(0), y(1), y(2),
		z(0), z(1), z(2);
		
		Vector3f n = M * r.direction;
		Vector3f P = M * (position - r.origin);
		float a = n.dot((0,0,-1) * n);
		float b = 2 * P.dot((0,0,-1) * n);
		float c = P.dot((0,0,-1) * P);
		float delta = b*b - 4 * a * c;
		
		if (b >= 0 && delta >= 0) {
			float th = (b - sqrt(delta)) / (2 * a);
			if (th < Epsilon) {
				th = (b + sqrt(delta))/(2*a);
				
				if (th < r.tmin || th > r.t) {
					return false;
				}
			}			
			r.t = th;
			r.hit = this;
			return true;
		}
		return false;
		
		
		
		
	}
	
	virtual Vector3f getNormal(Ray &r)
	{
		Vector3f v;
		Vector3f p = r.origin + r.t * r.direction;
		
		v(0) = (p(0) - position(0))/ (s0 * s0);
		v(1) = (p(1) - position(1))/ (s1 * s1);
		v(2) = -1 * (p(2) - position(2))/ (s2 * s2);
		
		return v.normalized();
	}
	
	
};

#endif