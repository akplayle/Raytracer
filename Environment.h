/*
 *  Environment.h
 *  Raytracer
 *
 *  Created by Amber on 3/30/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "Shader.h"


class Environment : public Shader
{
public:	
	
	color c;
	Environment(Scene *scene) : Shader(scene) {};
	Environment(Scene *scene, color c) : Shader(scene), c(c){};
	~Environment() {};
	
	
	
	color Shade(Ray r)
	{
		color result;
		//get the color
		if (getTextureCount() != 0 && getTexture("env")!=NULL) {
			PAIR uv = scene->getUV(r);
			result = getTexture("env")->getTexel(uv);
		}
		return result;
		
	};
	
};

#endif