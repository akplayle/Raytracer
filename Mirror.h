/*
 *  Mirror.h
 *  Raytracer
 *
 *  Created by Amber on 3/30/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef MIRROR_H
#define MIRROR_H

#include "Shader.h"
#include "Fresnel.h"


class Mirror : public Shader
{
public:
	color c;
	float ks, ka, ke, ki;
	bool crisp;
    float glossy;
    
    Fresnel* fresnel;
    
	Mirror(Scene* scene) : Shader(scene)
	{
		ks = 0.5;
		ka = 0.5;
		ke = 5;
        ki = 0.0;
		crisp = false;
        glossy = 0.0;
	};
	
	~Mirror(){}
	
	color Shade(Ray r)
	{
        
        //////////////////////////////
        //      INITIALIZATION      //
        //////////////////////////////
        color diffuse = c;
        float amb = ka;
        float spec = ke;
        float refl = ks;
        
        
        
		//get the normal
		Vector3f normal = r.hit->getNormal(r);
		if (normal.dot(r.direction) > 0) {
			normal = -normal;
		}
		
		//get the color
		if (getTextureCount() != 0 && getTexture("diff")!=NULL) {
			diffuse = getTexture("diff")->getTexel(r.hit->getUV(r));
		}
		
		
		//calculate reflection vector
		Vector3f reflect = r.direction - 2 * normal.dot(r.direction) * normal;

		
		//calculate shadow ray
		Ray shadow;
		shadow.origin = r.origin + r.t * r.direction;
		
		
		
		//get the total light contribution
		float totalBrightness = 0;
		for (unsigned int k = 0; k < scene->lights.size(); k++) {
			totalBrightness += scene->lights[k]->brightness;
		}
		
		//calculate basic phong shading
		color result;
		color lightColor;
		float currentBrightness=0;
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
					
					
					float distance = shadow.t;
					
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
						}
                        else if (!scene->castShadows || !scene->Intersect(shadow) || !shadow.hit->castShadows() || shadow.t > distance)
                        {
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
					}
					
					
				}
				//divide by the number of light rays
				result_local /= float(scene->lights[k]->getNumberOfRays());
				//add the local result to the final result
                result += result_local;
				
			}
			
		}
    
		//calculate reflection color
		Ray Reflect;
		Reflect.origin = r.origin + r.t * r.direction;
		Reflect.direction = reflect.normalized();
		Reflect.bounces = r.bounces + 1;
        
        float a = (0.5 - frand()) * glossy;
        float b = (0.5 - frand()) * glossy;
        
        Vector3f z(0.0,1.0,0.0);
        Vector3f du = normal.cross(normal + z).normalized();
        Vector3f dv = (normal.cross(du)).normalized();
        reflect = reflect + a * du + b * dv;
        
        Reflect.direction = reflect.normalized();
        
        
		color reflection = scene->raytrace(Reflect);
        
        
        //calculate Irirdescence
        float t = ki;
        
        //get the thickness value
		if (getTextureCount() != 0 && getTexture("iri")!=NULL) {
			t = (getTexture("iri")->getTexel(r.hit->getUV(r))).average();
		}
        t = t * 1.0;
        
        float alpha = 1.0;
        float cosTheta = reflect.normalized().dot(normal);
        cosTheta = powf(cosTheta, alpha);
        float d = t / (cosTheta + 1.0);
        float u = (2.0 * d - t);
        
        float hue = (u * 360);
        //hue = (u * (420) + (1 - u) * 240);
        
        
        color I;
        I = I.HSL(hue, 1.0, 0.5);
        float iri = 0.0;
        color white(1.0, 1.0, 1.0, 1.0);
        I = (iri) * I + (1.0 - iri) * white;
        
        if(I.r == 0 && I.g == 0 && I.b == 0)
        {
            I.set(1.0, 1.0, 1.0);
        }
        
        
		//get the ks value
		if (getTextureCount() != 0 && getTexture("ks")!=NULL) {
			refl = (getTexture("ks")->getTexel(r.hit->getUV(r))).average();
		}
		
		//add to the base color
		result = (1 - refl) * result + refl * reflection;
        //result = result * reflection;
		//result = I;
		
		
		return result;
	}
	
	
};
#endif