/*
 *  Gooch.h
 *  Raytracer
 *
 *  Created by Amber on 2/15/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef GOOCH_H
#define GOOCH_H

#include "Shader.h"


class Gooch : public Shader
{
public:	
	
	float ke;
	bool crisp;
	color c1;
	color c2;
	Gooch(Scene *scene) : Shader(scene) 
	{
		ke = 5;
		crisp = false;
	};
	Gooch(Scene *scene, color c1, color c2) : Shader(scene), c1(c1), c2(c2), ke(ke), crisp(crisp) {};
	~Gooch() {};
	
	
	
	virtual color Shade(Ray r)
	{
        
        //////////////////////////////
        //      INITIALIZATION      //
        //////////////////////////////
        color diffuse = c1;
        color shade = c2;
        float spec = ke;
        
        
		
		Vector3f normal = r.hit->getNormal(r);
		
		if (normal.dot(r.direction) > 0) {
			normal = -normal;
		}
		
		Ray shadow;
		shadow.origin = r.origin + r.t * r.direction;
		shadow.hit = r.hit;
		Vector3f reflect = r.direction - 2 * normal.dot(r.direction) * normal;
		
		
		color result;
		for (unsigned int k = 0; k < scene->lights.size(); k++) {
			
			
			shadow.direction = (scene->lights[k]->position - shadow.origin).normalized();
			
			
			
			
			//cout << "number of rays: " << scene->lights[k]->getNumberOfRays() << endl;
			for (unsigned int s = 0; s < scene->lights[k]->getNumberOfRays(); s++) {
				
				color result_local;
				//set the color for a single light ray with result_local
				if (scene->lights[k]->Illuminate(shadow, s)) {
					color lightColor = scene->lights[k]->intensity;
					
					float distance = shadow.t;
					
					float cosLightNormal = shadow.direction.dot(normal);
					
					//gooch cosine
					cosLightNormal = (1 + cosLightNormal) / 2.0;
					
                    color ambient = shade;
					result_local = (1-cosLightNormal) * ambient;
					
					color diff = (diffuse * cosLightNormal);
					result_local += diff * lightColor;
					
					Ray Reflect;
					Reflect.direction = reflect.normalized();
					Reflect.origin = r.origin + r.t * r.direction;;
					
					shadow.direction = (scene->lights[k]->position - shadow.origin).normalized();
                    
                    
                    
                    //cout << cosN << endl;
					if (scene->castShadows && scene->Intersect(shadow) && shadow.hit->castShadows()) {
						if (shadow.t < distance) {
							//is in shadow
							color shadow_color(0.0,0.0,0.0,1.0); // set to shadow color
							shadow_color = ambient;
							
                            
							//shadow type
							//if normal
							if (scene->shadow_type == 0) {
								result = shadow_color;
							}
							
							//gooch shadows
							if (scene->shadow_type == 1) {
								Vector3f objNormal = shadow.hit->getNormal(shadow);
								float cosObjNormal = shadow.direction.dot(objNormal);
								cosObjNormal = 1 - cosObjNormal * cosObjNormal;
								result = cosObjNormal * result_local + (1- cosObjNormal) * shadow_color;
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
									if (trans.hit == obj) {
										d += trans.t;
									}
									obj = trans.hit;
								}
								
								
								float cosShadow = d/ldist * cosLightNormal;
								cosShadow = powf(cosShadow, (1.0/5.0));
								
								result = (1-cosShadow) * result_local + cosShadow * shadow_color;
							}
							
							break;
						}
					}
                    
                    
                    
                    
                    
                    
					float cosLightReflect = shadow.direction.dot(reflect);
					if (cosLightReflect > 0 && spec > Epsilon) {
						
						cosLightReflect = powf(cosLightReflect, spec);
						
						if (crisp) {
							if (scene->lights[k]->Intersect(Reflect)) {
								cosLightReflect = 1.0;
							}
							else {
								cosLightReflect = 0.0;
							}
							
						}
						//result_local.print();
						color specColor = scene->lights[k]->c;
						result_local = result_local * (1-cosLightReflect) + cosLightReflect * specColor * lightColor;
					}
					
					
					
					
					
					
					
				}
				//divide by the number of light rays
				result_local /= float(scene->lights[k]->getNumberOfRays());
				//add the local result to the final result
				result += result_local;
			}
			
			
		}
		
        return result;
		
	};
	
};

#endif