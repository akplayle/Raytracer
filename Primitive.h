/*
 *  Primitive.h
 *  Raytracer
 *
 *  Created by Amber on 2/13/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "Box.h"
#include "Ray.h"
#include "Shader.h"

class Primitive
{
protected:
	Box bounds;
	bool dirtyBounds;
	Shader* shader;
	bool CastShadows;
	
	
	
public:
	Vector3f position;
	
    char name[30];
	
    Vector3f n0;
	Vector3f n1;
	Vector3f n2;
    
	Primitive()
	{
		dirtyBounds = true;
		shader = NULL;
		CastShadows = true;
		strcpy(name, "");
        position << 0.0,0.0,0.0;
	}
	
	virtual ~Primitive()
    {
        position << 0.0, 0.0, 0.0;
        n0 << 1.0, 0.0, 0.0;
		n1 << 0.0, 1.0, 0.0;
		n2 = n0.cross(n1);
		
		n0 = n0.normalized();
		n1 = n1.normalized();
		n2 = n2.normalized();

    }
	
    virtual Box calcBounds() = 0;
    
    
	virtual bool Intersect(Ray& r) = 0;
	virtual Vector3f getNormal(Ray& r) = 0;
	
	
	Shader* getShader() {return shader;}
	void setShader(Shader* sh){shader = sh;}

	
	
	
	virtual PAIR getUV(Ray &r) {return PAIR(0.0f, 0.0f); }
	
	inline Box& GetBounds()
	{
		if (dirtyBounds) {
			bounds = calcBounds();
			dirtyBounds = false;
		}
		return bounds;
	}
	
	inline bool& castShadows(){return CastShadows;}
	
	inline void setCastShadows(bool shadow){CastShadows = shadow;}
    
    inline void setBounds(bool db){dirtyBounds = db;}
	
};

#endif
