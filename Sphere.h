/*
 *  Sphere.h
 *  Raytracer
 *
 *  Created by Amber on 2/7/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef SPHERE_H
#define SPHERE_H

#include <Eigen/Dense>

#include "Primitive.h"

using namespace Eigen;

class Sphere : public Primitive
{	
	
	
public:
	float radius;

	Sphere() : Primitive()
	{
		
		radius	= 1.0;
		
	}
	
	virtual ~Sphere(){};
	
    Box calcBounds()
	{
		Box b = Box();
		b.Extend(position + Vector3f(-radius, -radius, -radius));
		b.Extend(position + Vector3f(radius, radius, radius));
		return b;
	}
	
	
	
	virtual bool Intersect(Ray &r)
	{
		
		Vector3f d = position - r.origin;
		float b = r.direction.dot(d);
		float c = d.dot(d) - powf(radius, 2);
		float delta = b*b - c;
		
		//ray is inside the sphere
		if (c < Epsilon) {
			return false;
		}
		
		//cout << "b: " << b << " c: " << c << " delta: " << delta << endl;
		if (b >= Epsilon && delta >= Epsilon) {
			float th = b - sqrt(delta);
			if (th < Epsilon) {
                
				th = b + sqrt(delta);
				if (th < Epsilon) {
					return false;
				}
                
                //return false;
			}
			r.t = th;
			r.hit = this;
			r.alpha = 1.0;
			return true;
		}
		return false;
	}
	
	virtual Vector3f getNormal(Ray &r)
	{
		
		Vector3f normal = ((r.origin + r.t * r.direction) - position).normalized();
		
		Vector3f n = normal;
		
		
		Texture* bumpmap = shader->getTexture("bump");
		
		float bumpfactor = -1.0;
		
		Vector3f p = r.origin + r.t * r.direction;
		
		if (bumpmap != NULL) {
			float bump = bumpmap->getTexel(r.hit->getUV(r)).average();
			Vector3f z(0.0,0.0,1.0);
			Vector3f du = p + normal.cross(normal + z).normalized();
			Vector3f dv = p + (normal.cross(du)).normalized();
			p += bumpfactor * bump * normal;
			du = p - du;
			dv = p - dv;
			n = du.cross(dv).normalized();
            
		}
		
		
		Texture* normalmap = shader->getTexture("normal");
		normal = ((r.origin + r.t * r.direction) - position).normalized();
		p = r.origin + r.t * r.direction;
		if (normalmap != NULL) {
			n = normal;
			color norm = normalmap->getTexel(r.hit->getUV(r));
			float nx = 2 * norm.r - 1;
			float ny = 2 * norm.g - 1;
			float nz = norm.b;
			
			n = n0 * nx + n1 * ny + n2 * -nz;
			n = (normal + n).normalized();
			
			
		}
		
		return n;
	}
	
	
	virtual PAIR getUV(Ray &r)
	{
		
		Vector3f Pp = r.origin + r.direction * r.t;
		
        n0 << 1.0, 0.0, 0.0;
		n1 << 0.0, 1.0, 0.0;
		n2 = n0.cross(n1);
        
		float x = n0.dot(Pp - position) / radius;
		float y = n1.dot(Pp - position) / radius;
		float z = n2.dot(Pp - position) / radius;
		
		float phi = (float)acos(y);
		float theta = (float)atan2(z,x);
		
		if (theta < 0) {
			theta = 2 * PI + theta;
		}
		
		float u = theta / (2 * PI);
		float v = (PI - phi)/ PI; 
		return PAIR(u,v);
		
	}
	
	
};

#endif