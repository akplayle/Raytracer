/*
 *  Camera.h
 *  Raytracer
 *
 *  Created by Amber on 2/9/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef CAMERA_H
#define CAMERA_H

#include <Eigen/Dense>
#include "Color.h"
#include "Ray.h"
#include "Texture.h"

using namespace Eigen;

class Camera
{
public:
	Vector3f position;
	Vector3f upV;
	Vector3f viewV;
	
	float width;
	float height;
    
    Texture* inputImage;
    float scale;
    
    Vector3f n0, n1, n2;
    
    Camera()
    {
        position << 0.0,0.0,0.0;
        upV << 0.0,1.0,0.0;
        viewV << 0.0,0.0,1.0;
        
        width = 640;
        height = 480;
        
        n2 = viewV;
        n1 = upV;
        n0 = (upV.cross(viewV)).normalized();
    }
    
    virtual ~Camera(){}
    
    virtual Ray InitRay(float x, float y) = 0;
};

#endif