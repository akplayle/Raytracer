/*
 *  Plasma.h
 *  Raytracer
 *
 *  Created by Amber on 3/16/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef PLASMA_H
#define PLASMA_H


#include "Shader.h"
#include "Perlin3D.h"

class Plasma : public Shader
{
private:
	Perlin3D pnoise;
	
public:
	
	color c;
	
	Plasma(Scene *scene) : Shader(scene)
	{};
	
	color Shade(Ray r)
	{
		
		PAIR uv = r.hit->getUV(r);
		
		float x = uv.first;
		float y = uv.second;
		float z = scene->timer.getTime() * 0.01;
		
		float total = pnoise.noise(x,y,z);
		
		if (total < 0) {
			total = 0;
		}
		if (total > 1) {
			total = 1;
		}
		Vector3f normal = r.hit->getNormal(r);
		color result_local;
		result_local.r = fabs(normal[0]);
		result_local.g = fabs(normal[1]);
		result_local.b = fabs(normal[2]);
		color result = result.interpolated(result_local, total);
		
		return result;
	};
	
};
#endif