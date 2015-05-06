/*
 *  Cloud.h
 *  Raytracer
 *
 *  Created by Amber on 3/16/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef CLOUD_H
#define CLOUD_H


#include "Shader.h"
#include "Perlin3D.h"

class Cloud : public Shader
{
private:
	Perlin3D pnoise;
	
public:
	
	color c;
	
	Cloud(Scene *scene) : Shader(scene)
	{};
	
	color Shade(Ray r)
	{
		
		
		Vector3f pos = r.origin + r.t * r.direction;
		PAIR uv = r.hit->getUV(r); 
		// fBM
		float total = 0.0f;
		float persistence = 0.7f;
		float amplitude = persistence;
		float freq = 0.01;
		float x = pos[0] * 10;
		float y = pos[1] * 10;
		float z = (pos[2] + (scene->timer.getTime() * 10) * 0.01) * 10;
		for (int i = 0; i < 6; i++) {
			total += amplitude * pnoise.noise(x*freq, y*freq, z*freq);
			freq *= 2;
			amplitude *= persistence;
		}
		 
		if (total < 0) total = 0;
		if (total > 1) total = 1;
		
		color result;
		color t;
		t.r = total;
		t.g = total;
		t.b = total;
		t.a = total;
		color white(1.0, 1.0, 1.0, 1.0);
		result = c * t.interpolated(white,pow(1-uv.second-0.5,12));
		
		return result;
		
	};
	
};
#endif