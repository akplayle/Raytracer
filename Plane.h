/*
 *  Plane.h
 *  Raytracer
 *
 *  Created by Amber on 2/15/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef PLANE_H
#define PLANE_H

#include "Primitive.h"


class Plane : public Primitive
{
	
	
	
public:
	Vector3f normal;
	Vector3f origin;
	
	Plane(Vector3f origin, Vector3f normal)
	: normal(normal), origin(origin)
	{ 
		normal.normalized();
		position = origin;
	};
	
	Plane()
	{
		normal << 0.0, 1.0, 0.0;
		origin << 0.0, 0.0, 0.0;
		position = origin;
	};
	
	virtual ~Plane() {}
	
	Box calcBounds()
	{
		Vector3f x(1.0,0.0,0.0);
		Vector3f y(0.0,1.0,0.0);
		Vector3f z(0.0,0.0,1.0);
		
		n0 = normal.cross(y);
		n1 = normal.cross(n0);
		
		//normal in y direction
		if (y.cross(normal).norm() == 0) {
			n0 = x;
			n1 = z;
		}
		
		Box b = Box();
		b.Extend(origin + n0 * -Infinity + n1 * -Infinity + normal * -0.1);
		b.Extend(origin + n0 * Infinity + n1 * Infinity + normal * 0.1);
		return b;
	}
    
	virtual bool Intersect(Ray& r)
	{
		normal = normal.normalized();
		float denom = r.direction.dot(normal);
		if (denom != Epsilon) {
			float d = (r.origin - origin).dot(normal);
			float th = -d/denom;
			
			if (th >= Epsilon) {
				r.t = th;
				r.hit = this;
				r.alpha = 1.0;
				return true;
			}
		}
		return false;
		
		
	};
	
	virtual Vector3f getNormal(Ray& r)
	{
		normal = normal.normalized();
		Vector3f n = normal;
		
		
		//BUMPMAP
		Texture* bumpmap = shader->getTexture("bump");
		float bumpfactor = 4.0;
		Vector3f p = r.origin + r.t * r.direction;
		if (bumpmap != NULL) {
			float bump = bumpmap->getTexel(r.hit->getUV(r)).average();
			
			Vector3f x(1.0,0.0,0.0);
			Vector3f y(0.0,1.0,0.0);
			Vector3f z(0.0,0.0,1.0);
			Vector3f du = n.cross(n + z).normalized();
			Vector3f dv = n.cross(du).normalized();
			
			// normal in same z direction as normal
			if (z.cross(n).norm() == 0) {
				du = x;
				dv = y;
			}
			
			//normal in same y direction as upV
			if (y.cross(n).norm() == 0) {
				du = x;
				dv = z;
			}
			
			//normal in same x direction as upV
			if (x.cross(n).norm() == 0) {
				du = z;
				dv = y;
			}
			
			du = p + du;
			dv = p + dv;

			p += bumpfactor * bump * normal;
			du = du - p;
			dv = dv - p;
			n = du.cross(dv).normalized();
			//cout << "new normal: " << n << endl;
		}
		
		//NORMAL MAP
		
		Texture* normalmap = shader->getTexture("normal");
		bumpfactor = 1.0;
		p = r.origin + r.t * r.direction;
		if (normalmap != NULL) {
			color norm = normalmap->getTexel(r.hit->getUV(r));
			float nx = 2.0 * (norm.r) - 1.0;
			float ny = 2.0 * (norm.g) - 1.0;
			float nz = norm.b;
			
			Vector3f x(1.0,0.0,0.0);
			Vector3f y(0.0,1.0,0.0);
			Vector3f z(0.0,0.0,1.0);
			Vector3f du = n.cross(n - z).normalized();
			Vector3f dv = n.cross(du).normalized();
			
			// normal in z direction
			if (z.cross(n).norm() == 0) {
				du = x;
				dv = y;
			}
			/*
			//normal in y direction
			if (y.cross(n).norm() == 0) {
				du = x;
				dv = z;
			}
			
			//normal in x direction
			if (x.cross(n).norm() == 0) {
				du = z;
				dv = y;
			}
			 */
			
			n = normal * nz + du * nx + dv * ny;
			n = n.normalized();
			
		}
		
		return n;
	}
	
	virtual PAIR getUV(Ray &r)
	{
		normal = normal.normalized();
		
		Vector3f x(1.0,0.0,0.0);
		Vector3f y(0.0,1.0,0.0);
		Vector3f z(0.0,0.0,1.0);
		
		n0 = normal.cross(y);
		n1 = normal.cross(n0);
		
		//normal in y direction
		if (y.cross(normal).norm() == 0) {
			n0 = x;
			n1 = z;
		}
		
		Vector3f Pp = r.origin + r.direction * r.t;
		float u = n0.dot(Pp - origin) / 20;
		float v = n1.dot(Pp - origin) / 20;
		
		u = u-(int)u;
		v = v-(int)v;
		
		if (u < 0) {
			u = 1 + u;
		}
		if (v < 0) {
			v = 1 + v;
		}
		
		u = u - (int)fabs(u);
		v = v - (int)fabs(v);
		
		//cout << "u: " << u << " v: " << v<< endl;
		
		return PAIR(u,v);
		
	}

};

#endif