/*
 *  Shader.h
 *  Raytracer
 *
 *  Created by Amber on 2/13/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef SHADER_H
#define SHADER_H

#include "Ray.h"
#include "Color.h"
#include "Texture.h"

using namespace std;

class Scene; //forward declaration

class Shader
{
protected:
	Scene *scene;
	
	vector<Texture*> textures;
	
public:
	char name[30];
    
	Shader(Scene *scene) : scene(scene) {};
	
	virtual ~Shader() {}
	virtual color Shade(Ray r) = 0;
	
	unsigned int getTextureCount() const {return textures.size();}
	
	void addTexture(Texture* tex)
	{
		textures.push_back(tex);
	}
	
	void removeTexture(Texture* tex)
	{
		for (vector<Texture*>::iterator it = textures.begin(); it != textures.end();) {
			if ((*it) == tex) {
				it = textures.erase(it);
			}
			else {
				it++;
			}

		}
	}
	
	Texture* getTexture(string n)
	{
		Texture* tex = NULL;
		for (int i = 0; i < textures.size(); i++) {
			if (textures[i]->name == n) {
				tex = textures[i];
			}
		}
		return tex;
	}
	
};


#endif