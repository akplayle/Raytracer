/*
 *  Ellipsoid.h
 *  Raytracer
 *
 *  Created by Amber on 3/2/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ELLIPSOID_H
#define ELLIPSOID_H

#include <Eigen/Dense>

#include "Primitive.h"

using namespace Eigen;

class Ellipsoid : public Primitive
{	
		
public:
	
	float s0, s1, s2;
	
	Ellipsoid()
	{
		position << 0.0, 0.0, 0.0;
		s0 = s1 = s2 = 1.0;
		n0 << 1.0, 0.0, 0.0;
		n1 << 0.0, 1.0, 0.0;
		n2 << 0.0, 0.0, 1.0;
	}
	
	
	virtual ~Ellipsoid(){};

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
		float a = n.dot(n);
		float b = 2 * P.dot(n);
		float c = P.dot(P) - 1.0;
		float delta = b*b - 4 * a * c;
		
		if (b >= 0 && delta >= 0) {
			float th = (b - sqrt(delta)) / (2 * a);
			r.t = th;
			r.hit = this;
			r.alpha = 1.0;
			return true;
		}
		return false;

		


	}
	
	virtual Vector3f getNormal(Ray &r)
	{
		Matrix3f M; //Transformaion Matrix
		Vector3f x = n0.normalized()/s0;
		Vector3f y = n1.normalized()/s1;
		Vector3f z = n2.normalized()/s2;
		M <<	x(0), x(1), x(2),
		y(0), y(1), y(2),
		z(0), z(1), z(2);
		
		Vector3f v;
		Vector3f p = r.origin + r.t * r.direction;
		
		v(0) = n0.dot(p - position) / (s0 * s0);
		v(1) = n1.dot(p - position) / (s1 * s1);
		v(2) = n2.dot(p - position) / (s2 * s2);
		
		Vector3f normal = v.normalized();
		
		Texture* bumpmap = shader->getTexture("bump");
		float bumpfactor = 1.0;
		if (bumpmap != NULL) {
			float bump = bumpmap->getTexel(r.hit->getUV(r)).average();
			Vector3f z(0.0,0.0,1.0);
			Vector3f du = p + normal.cross(normal + z).normalized();
			Vector3f dv = p + (normal.cross(du)).normalized();
			p += bumpfactor * bump * normal;
			du = p - du;
			dv = p - dv;
			normal = du.cross(dv).normalized();
		}
		
		return normal;
	}
	
	
};

#endif