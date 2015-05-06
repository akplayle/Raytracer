/*
 *  Projection.h
 *  Raytracer
 *
 *  Created by Amber on 3/17/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef PROJECTION_H
#define PROJECTION_H

#include "Light.h"

class Projection : public Light
{
public:
	
	Vector3f normal;
	float coneAngle;
	float d;
	Vector3f position;
	
	Projection() : Light()
	{
		coneAngle = 180;
		d = 5.0;
		
		position << 0.0, 0.0, 0.0;
		image = NULL;
		normal << 0.0, -1.0, -1.0;
		normal = normal.normalized();
		
		
	};
	Projection(Vector3f position, Texture* img, float brightness, char name[30], Vector3f n, float ca, float di)  
	{
		normal = n;
		normal = normal.normalized();
		
		position = position;
		image = img;
		brightness = brightness;
		name = name;
		
		
		coneAngle = ca;
		d = di;
	};
	
	
	virtual Vector3f getNormal(Ray& r)
	{
		return normal;
	}
	
	unsigned int getNumberOfRays() { return 1; }
	
	bool Illuminate(Ray &r, int index)
	{
		
		color c;
		
		float scale = 20.0;
		
		
		float w = image->getWidth() * scale;
		float h = image->getHeight() * scale;
		
		//float sx = w/h;
		//float sy = 1.0;
		
		
		Vector3f upV(0.0, 1.0, 0.0);
		if (normal.cross(upV).norm() == 0) {
			upV << 1.0, 0.0, 0.0;
		}
		
		
		upV = upV.normalized();
		Vector3f n0 = (upV.cross(normal)).normalized();
		Vector3f n1 = normal.cross(n0);
		
		Vector3f pos = position - normal * d;
		
		Vector3f Po = pos - n0*(w/2.0) - n1 * (h/2.0);
		
		
		float denom = r.direction.dot(normal);
		
		//cout << Po << endl;
		
		if (denom != Epsilon) {
			
			float d = (r.origin - pos).dot(normal);
			r.t = -d/denom;
			
			
			if (r.t >= Epsilon) {
				Vector3f Pp = r.origin + r.direction * r.t;
				//find x and y point on plane

				float x = ((Pp - Po).dot(n0))/w;
				float y = ((Pp - Po).dot(n1))/h;
				
								
				//if intersects the light shape
				if(IntersectRectangle(x, y, w, h))
				{
					

					
					float u = x;
					float v = y;
					
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
					
					c = image->getTexel(PAIR(u,v));
					
					float att = 1.0f / (r.t * r.t);
					att = 1.0;
					intensity = att * c;
					float cosN = r.direction.dot(normal);
					//cout << cosN << endl;
					//cout << cosf((3.14159/180.0) * coneAngle) <<endl;
					if (cosN > cos((3.14159/180.0) * coneAngle)) {
						return true;
					}
				}
				
			}
			
			
		}
		return false;
		
	};	
	
	
	bool Intersect(Ray& r)
	{
		return false;
		r.direction = position - r.origin;
		r.hit = NULL;
		r.t = r.direction.norm() - Epsilon;
		r.direction = r.direction.normalized();
		
		
		
		float cosN = -r.direction.dot(normal);
		if (cosN < cos((3.14159/180.0) * coneAngle)) {
			return true;
		}
		else {
			return false;
		}
	};
	
	
	
	bool IntersectRectangle(float x, float y, float w, float h)
	{
		
		if (x <= w/2 && x >= 0 && y <= w/2 && y >= 0) {
			return true;
		}
		else {
			return false;
		}
		
	}
	
	
	
};
#endif