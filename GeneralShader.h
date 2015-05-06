/*
 *  GeneralShader.h
 *  Raytracer
 *
 *  Created by Amber on 3/30/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef GENERALSHADER_H
#define GENERALSHADER_H

#include "Shader.h"


class GeneralShader : public Shader
{
public:	
	
	bool diff, spec, reflection;
	
	bool crisp;
	
	color ambientColor;
	color diffuseColor;
	color specularColor;
	color reflectionColor;
	
	
	float ka, ke, ks;
	
	GeneralShader(Scene *scene) : Shader(scene)
	{
		diff = true;
		spec = true;
		reflection = false;
		
		crisp = false;
		
		ka = 0.2;
		ke = 5;
		ks = 0.25;
		kr = 0.5;
	};
	~GeneralShader() {};
	
	
	
	virtual color Shade(Ray &r)
	{
		Vector3f normal = r.hit->getNormal(r);
		
		if (normal.dot(r.direction) > 0) {
			normal = -normal;
		}
        
        
        //lock thread
        pthread_mutex_lock(&lock);
		
		//get the color
		if (getTextureCount() != 0 && getTexture("diff")!=NULL) {
			diffuseColor = getTexture("diff")->getTexel(r.hit->getUV(r));
		}

		Ray shadow;
		shadow.origin = r.origin + r.t * r.direction;
		
		Vector3f reflect = r.direction - 2 * normal.dot(r.direction) * normal;

		color result;
		
		
		//	Get the total brightness of the lights
		
		float totalBrightness = 0;
		for (unsigned int k = 0; k < scene->lights.size(); k++) {
			totalBrightness += scene->lights[k]->brightness;
		}
		
		
		//	Go through the lights and calculate the basic phong color
		
		color lightColor;
		float currentBrightness = 0;
		for (unsigned int k = 0; k < scene->lights.size(); k++) {
			
			
			shadow.direction = (scene->lights[k]->position - shadow.origin).normalized();
			
			color result_local;
			float samples = scene->lights[k]->getNumberOfRays();
			for (unsigned int s = 0; s < samples; s++) {
				
				
				
				
				//set the color for a single light ray with result_local
				if (scene->lights[k]->Illuminate(shadow, s)) {
					
					float cosLightNormal = shadow.direction.dot(normal);
					//truncate the cosine
					
					if (cosLightNormal <= 0) {
						cosLightNormal = 0;
					}
					
					//Light Color
					color light = scene->lights[k]->c;
					float brightness = scene->lights[k]->brightness / samples;
					currentBrightness += brightness;
					lightColor = light * (brightness / totalBrightness) + (totalBrightness - currentBrightness) / totalBrightness * lightColor;
					
					
					//Diffuse Color
					color ambient = ambientColor * ka;
					result_local = (1 - cosLightNormal) * ambient;
					color diff = cosLightNormal * diffuseColor;
					result_local += diff * lightColor;
					
					//if in shadow
					
					float distance = shadow.t;
					
					if (scene->castShadows && scene->Intersect(shadow) && shadow.hit->castShadows()) {
						if (shadow.t < distance) {
							//is in shadow
							color shadow_color(0.0,0.0,0.0,1.0);
							shadow_color = ambient;
							
							
							//shadow type
							//if normal
							if (scene->shadow_type == 0) {
								result_local = shadow_color;
							}
							
							//gooch shadows
							if (scene->shadow_type == 1) {
								Vector3f objNormal = shadow.hit->getNormal(shadow);
								float cosObjNormal = shadow.direction.dot(objNormal);
								cosObjNormal = 1 - cosObjNormal * cosObjNormal;
								result_local = cosObjNormal * result_local + (1- cosObjNormal) * shadow_color;
							}
							
							//shadows from transparent objects
							if (scene->shadow_type == 2) {
								float ldist = (scene->lights[k]->position - shadow.origin).norm();
								vector <float> t;
								t.push_back(shadow.t);
								float d = 0.0;
								Ray trans;
								trans.origin = shadow.origin + shadow.t * shadow.direction;
								trans.direction = shadow.direction;
								//cout << scene->Intersect(trans);
								Primitive* obj = shadow.hit;
								while (scene->Intersect(trans)) {
									trans.origin = trans.origin + (trans.t + Epsilon) * shadow.direction;
									trans.direction = trans.direction;
									t.push_back(trans.t);
									if (trans.hit = obj) {
										d += trans.t;
									}
									obj = trans.hit;
								}
								
								
								float cosShadow = d/ldist * cosLightNormal;
								cosShadow = powf(cosShadow, (1.0/5.0));
								result_local= (1-cosShadow) * result_local + cosShadow * shadow_color;
							}
						}
					}
					
					
					
					//Specular Color
					
					if (specular) {
						
						Ray Spec;
						Spec.direction = reflect.normalized();
						Spec.origin = r.origin + r.t * r.direction;;
						
						shadow.direction = (scene->lights[k]->position - shadow.origin).normalized();
						float cosLightReflect = shadow.direction.dot(reflect);
						if (cosLightReflect > 0 && ke > Epsilon) {
							
							cosLightReflect = powf(cosLightReflect, ke);
							
							if (crisp) {
								if (scene->lights[k]->Intersect(Spec)) {
									cosLightReflect = 1.0;
								}
								else {
									cosLightReflect = 0.0;
								}
								
							}
							if (specularColor.zero()) {
								specularColor = scene->lights[k]->c;
							}
							
							result_local = result_local * (1-cosLightReflect) + cosLightReflect * specularColor * lightColor;
						}
					}
					
	
				}
				//divide by the number of light rays
				result_local /= float(scene->lights[k]->getNumberOfRays());
				
			}
			//add the local result to the final result
			result += result_local;
		}
		
        //unlock thread
        pthread_mutex_unlock(&lock);
		
		
		//	Reflection
		
		if (reflection) {
			Ray Reflect;
			
			Reflect.origin = r.origin + r.t * r.direction;
			Reflect.direction = reflect;
			Reflect.bounces = r.bounces + 1;
			color reflection = scene->raytrace(Reflect);
			result = result * (1-ks) + ks * reflection;
			return result;
		}
		
		
	};
	
};

#endif