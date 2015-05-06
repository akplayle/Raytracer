/*
 *  SpotLight.h
 *  Raytracer
 *
 *  Created by Amber on 3/2/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "Light.h"

class SpotLight : public Light
{
public:
	unsigned int raycount;
	Vector3f normal;
	float coneAngle;
	float d;
	float radius;
	float randI;
	float randJ;
	
	SpotLight() : Light()
	{
		raycount = 1;
		normal << 0.0, -1.0, -1.0;
		normal = normal.normalized();
		float coneAngle = 40;
		float d = 1.0;
		radius = atan((3.14159/180.0) * coneAngle)/d;
		
		randI = frand();
		randJ = frand();
		
	};
	SpotLight(Vector3f position, color c, float brightness, char name[30], Vector3f normal, float coneAngle, float d) 
	: Light(position, c, brightness, name), normal(normal), coneAngle(coneAngle), d(d)
	{
		normal.normalized();
		radius = atan((3.14159/180.0) * coneAngle)/d;
		
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
		r.direction = position - r.origin;
		r.hit = NULL;
		r.t = r.direction.norm() - Epsilon;
		r.direction = r.direction.normalized();
		
		float att = 1.0f / (r.t * r.t);
		att = 1.0;
		intensity = att * c * brightness;

		float cosN = -r.direction.dot(normal);
		//cout << "cosN: " << cosN << " cosConeAngle: " << sin((3.14159/180.0) * coneAngle) << endl;
		if (cosN > sin((3.14159/180.0) * coneAngle)) {
			return true;
		}
		else {
			return false;
		}

		
	};	
	
	
	bool Intersect(Ray& r)
	{
		
		r.direction = position - r.origin;
		r.hit = NULL;
		r.t = r.direction.norm() - Epsilon;
		r.direction = r.direction.normalized();
		
		float att = 1.0f / (r.t * r.t);
		att = 1.0;
		intensity = att * c * brightness;
		
		float cosN = -r.direction.dot(normal);
		if (cosN < cos((3.14159/180.0) * coneAngle)) {
			return true;
		}
		else {
			return false;
		}
	};
	
	
};
#endif