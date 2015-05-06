/*
 *  PerlinVolume.h
 *  Raytracer
 *
 *  Created by Amber on 3/16/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef VCLOUD_H
#define VCLOUD_H

#include <Eigen/Dense>

#include "Primitive.h"
#include "Perlin3D.h"

using namespace Eigen;

class VCloud : public Primitive
{
	
	
	
public:
	
	float radius;
	Perlin3D pnoise;
	float step;
	float depth;
	
	
	VCloud()
	{
		depth = 155;
		position << 0.0, 0.0, 0.0;
		radius	= 1.0;
		step = radius / depth;
	}
	
	
	virtual ~VCloud(){};
	
    Box calcBounds()
	{
		Box b = Box();
		b.Extend(position + Vector3f(-radius, -radius, -radius));
		b.Extend(position + Vector3f(radius, radius, radius));
		return b;
	}
	
	virtual bool Intersect(Ray &r)
	{
		step = radius / depth;
		
		Vector3f d = position - r.origin;
		float b = r.direction.dot(d);
		float rad = radius - step * r.bounces;
		//float rad = radius;
		float c = d.dot(d) - powf(rad, 2);
		float delta = b*b - c;
		
		//ray is inside the sphere
		//if (c < Epsilon) {
		//	return false;
		//}
		
		//cout << "b: " << b << " c: " << c << " delta: " << delta << endl;
		if (b >= Epsilon && delta >= Epsilon) {
			float th = b - sqrt(delta);
			if (th < Epsilon) {
				th = b + sqrt(delta);
				if (th < Epsilon) {
					return false;
				}
			}
			r.t = th;
			r.hit = this;
			
			//hit bounds
			if (raymarch(r))
			{
				return true;
			}
			else {
				return false;
			}
		}
		return false;
	}
	
	bool raymarch(Ray& r)
	{
		step = radius / depth;
		
		int index = r.bounces;
		Vector3f pos = r.origin + r.t * r.direction;
		PAIR uv = r.hit->getUV(r); 
		// fBM
		float total = 0.0f;
		float persistence = 0.7f;
		float amplitude = persistence;
		float freq = 0.01;
		float x = pos[0] * 10;
		float y = pos[1] * 10;
		float z = (pos[2] + (index * 10) * 0.01) * 10;
		for (int i = 0; i < 6; i++) {
			total += amplitude * pnoise.noise(x*freq, y*freq, z*freq);
			freq *= 2;
			amplitude *= persistence;
		}
		
		if (total < 0) total = 0;
		if (total > 1) total = 1;
		
		
		color t;
		t.r = total;
		t.g = total;
		t.b = total;
		color white(1.0, 1.0, 1.0, 1.0);
		color result_local = t.interpolated(white,pow(1-uv.second-0.5,12));
		float alpha = result_local.average();
		r.alpha += alpha;
		
		if (r.alpha >= 0.0) {
			//march forward
			r.t += step;
			return true;
		}
		
		
		return false;
	}
	
	
	virtual Vector3f getNormal(Ray &r)
	{
		
		Texture* bumpmap = shader->getTexture("bump");
		
		float bumpfactor = 1.0;
		Vector3f normal = ((r.origin + r.t * r.direction) - position).normalized();
		Vector3f p = r.origin + r.t * r.direction;
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
	
	
	virtual PAIR getUV(Ray &r)
	{
		Vector3f n0(1.0, 0.0, 0.0);
		Vector3f n1(0.0, 0.0, 1.0);
		Vector3f n2(0.0, 1.0, 0.0);
		
		Vector3f Pp = r.origin + r.direction * r.t;
		
		float rad = radius - step * r.bounces;
		
		float x = n0.dot(Pp - position) / rad;
		float y = n1.dot(Pp - position) / rad;
		float z = n2.dot(Pp - position) / rad;
		
		float phi = acos(z);
		float theta = acos(y / fabs(sqrt(1 - z * z)));
		
		if (x < 0) {
			theta = 2 * 3.14159 - theta;
		}
		
		float u = theta / (2 * 3.14159);
		float v = phi / (2 * 3.14159);
		
		return PAIR(u,v);
		
	}
	
	
};

#endif