/*
 *  AreaLight.h
 *  Raytracer
 *
 *  Created by Amber on 2/16/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef AREALIGHT_H
#define AREALIGHT_H

#include "Light.h"

class AreaLight : public Light
{
public:
	unsigned int raycount;
	Vector3f normal;
	float w, h;
	
	float randI;
	float randJ;
	
	AreaLight() : Light()
	{
		raycount = 16;
		normal << 0.0, -1.0, -1.0;
		normal = normal.normalized();
		w = 1.0;
		h = 1.0;
		
		randI = frand();
		randJ = frand();
		
	};
	AreaLight(Vector3f position, color c, float brightness, char name[30], Vector3f normal, float w, float h) 
	: Light(position, c, brightness, name), normal(normal), w(w), h(h)
	{
		normal.normalized();
		
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
		randI = frand();
		randJ = frand();
        
        color lcolor = c;
        float bright = brightness;
        
		float att = 1.0f / (r.t * r.t);
		att = 1.0;
		intensity = att * lcolor * bright;
		
		
		Vector3f upV(0.0, -1.0, 0.0);
		if (upV[1] == normal[1]) {
			upV << -1.0, 0.0, 0.0;
		}
		if (normal[1] >= 0) {
			upV = -upV;
		}
		
		
		upV = upV.normalized();
		Vector3f n0 = (upV.cross(normal)).normalized();
		Vector3f n1 = normal.cross(n0);
		
		Vector3f Po = position - n0*(w/2.0) - n1 * (h/2.0);
		
		randI = frand();
		randJ = frand();
		
		float m = float(sqrt(raycount));
		float n = float(sqrt(raycount));
		
		float i = float(fmod(index, m)/m) +  randI/m;
		float j = float(int(index / n)/n) +  randJ/n;
		//float i = float(index/(2*m)) + randI/m;
        //float j = float(index/(2*n)) + randJ/n;
        
        
		//cout << "index: " << index << " i: " << i << " j: " << j << endl;
		
		Vector3f Pos = Po + i * n0 * w + j * n1 * h;
		
		r.direction = Pos - r.origin;
		r.hit = NULL;
		r.t = r.direction.norm();
		r.direction = r.direction.normalized();
		
		float denom = r.direction.dot(normal);
		
		if (denom != Epsilon) {
			
			
			float d = (r.origin - Pos).dot(normal);
			r.t = -d/denom;
			
			if (r.t > Epsilon) {
				Vector3f Pp = r.origin + r.direction * r.t;
				
				//find x and y point on plane
				float x = ((Pp - Po).dot(n0))/w;
				float y = ((Pp - Po).dot(n1))/h;
				//cout << "x: " << x << " y: " << y << endl;
				
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
					
					lcolor = image->getTexel(PAIR(u,v));
					
					float att = 1.0f / (r.t * r.t);
					att = 1.0;
					intensity = att * lcolor * bright;
                    
                    
					return true;
				}

			}
			
			
		}
		return false;
	
	};	

	
	bool Intersect(Ray& r)
	{
		
		Vector3f upV(0.0, -1.0, 0.0);
		if (normal[1] >= 0) {
			upV = -upV;
		}
		
		upV = upV.normalized();
		Vector3f n0 = (upV.cross(normal)).normalized();
		Vector3f n1 = normal.cross(n0);
		
		Vector3f Po = position - n0*(w/2.0) - n1 * (h/2.0);
		
		
		
		float denom = r.direction.dot(normal);
		if (denom != Epsilon) {
			
			float d = (r.origin - position).dot(normal);
			r.t = -d/denom;
			
			if (r.t >= Epsilon) {
				Vector3f Pp = r.origin + r.direction * r.t;
				
				//find x and y point on plane
				float x = ((Pp - Po).dot(n0))/w;
				float y = ((Pp - Po).dot(n1))/h;
				
				
				//if intersects the specular shape
				if(IntersectRectangle(x, y, w/4, h/4))
				{
					return true;
				}
			}
			
		}
		return false;
	};
	
	
	bool IntersectRectangle(float x, float y, float w, float h)
	{
		if (fabs(x) <= w/2.0 && fabs(y) <= h/2.0) {
			return true;
		}
		else {
			return false;
		}

	}
	
	bool IntersectCircle(float x, float y, float r)
	{
		
		if (powf(x, 2) + powf(y, 2) - powf(r, 2) <= 0.0) {
			return true;
		}
		else {
			return false;
		}

	}
	
	
};
#endif