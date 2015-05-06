/*
 *  Lambert.h
 *  Raytracer
 *
 *  Created by Amber on 2/15/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef LAMBERT_H
#define LAMBERT_H

#include "Shader.h"


class Lambert : public Shader
{
public:	
	float ka;
	color c;
	Lambert(Scene *scene) : Shader(scene) 
	{
		ka = 0.2;
	};
	Lambert(Scene *scene, color c, float ka) : Shader(scene), c(c), ka(ka){};
	~Lambert() {};
	
    
    color Shade(Ray r)
	{
        //////////////////////////////
        //      INITIALIZATION      //
        //////////////////////////////
        color diffuse = c;
        float a = ka;
        
        
        
		Vector3f normal = r.hit->getNormal(r);
        
		if (normal.dot(r.direction) > 0) {
			normal = -normal;
		}
        
        
        //get the color
		if (getTextureCount() != 0 && getTexture("diff")!=NULL) {
			diffuse = getTexture("diff")->getTexel(r.hit->getUV(r));
		}
		
		color ambient = diffuse * a;
        color result;
        //color result = ambient;
        
        
        
		float totalBrightness = 0;
		for (unsigned int k = 0; k < scene->lights.size(); k++) {
			totalBrightness += scene->lights[k]->brightness;
		}

		color lightColor;
        color lightBright;
		float currentBrightness=0;
 
		for (unsigned int k = 0; k < scene->lights.size(); k++) {

			
			Ray shadow = r;
            shadow.origin = shadow.origin + shadow.t * shadow.direction;
            
			shadow.direction = (scene->lights[k]->position - shadow.origin).normalized();

            
            float brightness = scene->lights[k]->brightness;
            currentBrightness += brightness;
           
            
			color result_local;
            
			float samples = scene->lights[k]->getNumberOfRays();
			for (unsigned int s = 0; s < samples; s++) {
				
				//set the color for a single light ray with result_local
				if (scene->lights[k]->Illuminate(shadow, s)) {
					
					
					float distance = shadow.t;
					
                    //Light Color
                    color light = scene->lights[k]->intensity;
                    color lightB = scene->lights[k]->brightness * color(1.0,1.0,1.0,1.0);
                    lightColor = light * (brightness / totalBrightness) + (totalBrightness - currentBrightness) / totalBrightness * lightColor;
                    
                    lightBright = lightB * (brightness / totalBrightness) + (totalBrightness - currentBrightness) / totalBrightness * lightBright;
                    
                    float cosLightNormal = shadow.direction.dot(normal);
					//truncate the cosine
					
					if (cosLightNormal < 0) {
                        cosLightNormal = 0;
                    }
        
                        
                    //Diffuse Color
                    result_local = (1 - cosLightNormal) * ambient;
                    color diff = cosLightNormal * diffuse;
                    result_local += diff * lightColor;
                    

                    
                    
                    //if in shadow
                    if (scene->castShadows && scene->Intersect(shadow) && shadow.hit->castShadows()) {
                        if (shadow.t < distance) {
                            //is in shadow
                            color shadow_color;
                            shadow_color =  ambient * lightBright;
                            
                            
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
                    
                    
					
					
				}
                
				//divide by the number of light rays
				result_local /= float(samples);
				//add the local result to the final result
                result += result_local;
                //result = result / float(samples);
			}
			
		}
        
		return result;
		
	};
	
	
};

#endif