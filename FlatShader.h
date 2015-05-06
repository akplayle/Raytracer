/*
 *  FlatShader.h
 *  Raytracer
 *
 *  Created by Amber on 2/15/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef FLATSHADER_H
#define FLATSHADER_H

#include "Shader.h"


class FlatShader : public Shader
{
public:	
	
	color c;
	FlatShader(Scene *scene) : Shader(scene) {};
	FlatShader(Scene *scene, color c) : Shader(scene), c(c){};
	~FlatShader() {};
	
	
	
	virtual color Shade(Ray r)
	{
		return c;
		
	};
	
};

#endif