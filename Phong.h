/*
 *  Phong.h
 *  Raytracer
 *
 *  Created by Amber on 2/13/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef PHONG_H
#define PHONG_H

#include "Shader.h"


class Phong : public Shader
{
public:	
	
	color c;
	bool crisp;
	float ka, ke;
	
	Phong(Scene *scene) : Shader(scene)
	{
		ka = 0.2;
		ke = 5;
		crisp = false;
	};
	Phong(Scene *scene, color c, bool crisp, float ka, float ke) 
	: Shader(scene), c(c), crisp(crisp), ka(ka), ke(ke) {};
	~Phong() {};
	
	virtual color Shade(Ray r)
	{
        
        //////////////////////////////
        //      INITIALIZATION      //
        //////////////////////////////
        color diffuse = c;
        float amb = ka;
        float spec = ke;
        
        
		Vector3f normal = r.hit->getNormal(r);
		
		if (normal.dot(r.direction) > 0) {
			normal = -normal;
		}
		
        
		
		Ray shadow;
		shadow.origin = r.origin + r.t * r.direction;
		
		Vector3f reflect = r.direction - 2 * normal.dot(r.direction) * normal;
		
		
		
		
		float totalBrightness = 0;
		for (unsigned int k = 0; k < scene->lights.size(); k++) {
			totalBrightness += scene->lights[k]->brightness;
		}
        
		//get the color
		if (getTextureCount() != 0 && getTexture("diff")!=NULL) {
			diffuse = getTexture("diff")->getTexel(r.hit->getUV(r));
		}
        
        color result;
		color lightColor;
		float currentBrightness = 0;
		for (unsigned int k = 0; k < scene->lights.size(); k++) {
			
            
            //Light Color
            color light = scene->lights[k]->c;
            float brightness = scene->lights[k]->brightness;
            currentBrightness += brightness;
            lightColor = light * (brightness / totalBrightness) + (totalBrightness - currentBrightness) / totalBrightness * lightColor;
            
            
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
					
                    
					//Diffuse Color
					color ambient = diffuse * amb;
					result_local = (1 - cosLightNormal) * ambient;
					color diff = cosLightNormal * diffuse;
					result_local += diff;
					result_local = result_local * lightColor;
					
					//if in shadow
					
					float distance = shadow.t;
					
					if (scene->castShadows && scene->Intersect(shadow) && shadow.hit->castShadows()) {
						if (shadow.t < distance) {
							//is in shadow
							color shadow_color;
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
									if (trans.hit == obj) {
										d += trans.t;
									}
									obj = trans.hit;
								}
								
								
								float cosShadow = d/ldist * cosLightNormal;
								cosShadow = powf(cosShadow, (1.0/5.0));
								result_local= (1-cosShadow) * result_local + cosShadow * shadow_color;
							}
                            
                            //break;
                            //continue;
						}
                        
					}
					//else if (!scene->castShadows || !scene->Intersect(shadow) || !shadow.hit->castShadows() || shadow.t > distance)
                    //{
                        //Specular Color
                        
                        Ray Reflect;
                        Reflect.direction = reflect.normalized();
                        Reflect.origin = r.origin + r.t * r.direction;;
                        
                        shadow.direction = (scene->lights[k]->position - shadow.origin).normalized();
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
					
					
					
					
					
				
				//}
				//divide by the number of light rays
				result_local /= samples;
				//add the local result to the final result
                result += result_local;
			}
			
		}
		//result.clamp();
        
        
		return result;
		
	};
	
};

#endif