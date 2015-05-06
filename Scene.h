/*
 *  SDFReader.h
 *  Raytracer
 *
 *  Created by Amber on 2/7/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <Eigen/Dense>

#include "Timer.h"
#include "Camera.h"
#include "Object.h"
#include "Light.h"


using namespace Eigen;
using namespace std;

class Scene
{
public:
	Timer timer;
	bool castShadows;
	
	const int depth;
	pthread_mutex_t *locks;
    
    
	enum typeShadows {
		normal = 0,
		gooch = 1,
		soft = 2
	};
	typeShadows shadow_type;
	
	color background;
	Shader* env;
	Camera* camera;
	
	vector<Object*> objects;
	vector<Light*> lights;
	
	int numberOfObjects;
	int numberOfLights;
	
	FILE *sceneFile;
	
	
	Scene() :castShadows(true), shadow_type(normal), depth(5), env(NULL)
    {
        pthread_mutex_t array[depth];
        locks = array;
    }
	~Scene() 
	{
		for (int i = 0; i < objects.size(); i++) {
			if (objects[i]) {
				delete objects[i];
			}
		}
		for (int i = 0; i < lights.size(); i++) {
			if (lights[i]) {
				delete lights[i];
			}
		}
	}
	
	
	void AddObject(Object* obj)
	{
		objects.push_back(obj);
	}
	
	void AddLight(Light* light)
	{
		lights.push_back(light);
	}
	
    
	color raytrace(Ray r)
	{
        color result;
		
        //Set Environment Color
		if (env != NULL) {
			result = env->Shade(r);
		}
		else {
			result = background;
		}
		
		
		r.o = NULL;
		r.alpha = 0.0;

		if (r.bounces < depth && Intersect(r)) {
            r.bounces++;
			// get object color
			if (r.hit->getShader() != NULL) {
                result = r.hit->getShader()->Shade(r); 
			}
			else {
				result.set(1.0, 1.0, 1.0);
			}
			
			//if transparent recursivly get next object
			
			if (r.alpha < 1.0)
			{
				Ray trans = r;
				trans.origin = r.origin + r.t * r.direction;
				trans.bounces = r.bounces + 1;
				color next = raytrace(trans);
				result = (1 - r.alpha) * next + result * r.alpha;
			}
            r.bounces--;
		}
		
		//if transparent and done bouncing go get next object
		if (r.bounces > depth && r.alpha < 1.0) {
			
			Ray alpha = r;
			alpha.bounces = 0;
			alpha.origin = r.origin + r.t * r.direction;
			color next = raytrace(alpha);
			result = (1 - r.alpha) * next + r.alpha * result;
		}
		
        
		return result;

	}
	
    /*
     Need to Implement -> for caustics and color bleeding
     
    */
    color photontrace(Ray &r)
    {
        color result;
        
        //for each light
        for (int i = 0; i < lights.size(); i++) {
            //
        }
        
        
        return result;
    }
    
    
	
	bool Intersect(Ray &r)
	{
		
		bool hit = false;
		Object* obj = NULL;
		float th = r.tmax;
		Object* currentObj = r.o;
		for (int i = 0; i < objects.size(); i++) {
			if (currentObj!= objects[i] && objects[i]->Intersect(r)) {
				if (r.t < th && r.t > Epsilon) {
					th = r.t;
					obj = objects[i];
					
				}
			}
		}
		
		if (th < r.tmax && th > Epsilon) {
			r.alpha = 0.0;
			if(obj->Intersect(r))
			{
				hit = true;
			}
			else {
				hit = false;
			}
			
		}
		
		return hit;
	}
	
	PAIR getUV(Ray r)
	{
		Vector3f n0, n1, n2;
		
		n0 << 1.0, 0.0, 0.0;
		n1 << 0.0, 1.0, 0.0;
		n2 = n1.cross(n0);
		
		n0 = n0.normalized();
		n1 = n1.normalized();
		n2 = n2.normalized();
		
		
		
		float x = n0.dot(r.direction);
		float y = n1.dot(r.direction);
		float z = n2.dot(r.direction);
		
		float phi = (float)acos(y);
		float theta = (float)atan2(z,x);
		
		if (theta < 0) {
			theta = 2 * PI + theta;
		}
		
		float u = theta / (2 * PI);
		float v = (PI - phi)/ PI; 
		return PAIR(u,v);
	}
	
};


#endif
