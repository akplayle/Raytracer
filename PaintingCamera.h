//
//  PaintingCamera.h
//  Raytracer
//
//  Created by Amber on 4/23/14.
//
//

#ifndef PAINTINGCAMERA_H
#define PAINTINGCAMERA_H

#include "Camera.h"

class PaintingCamera : public Camera
{
public:
    
    
    PaintingCamera() : Camera()
    {
        inputImage = NULL;
        scale = 0.0;
    }
    
    
    virtual ~PaintingCamera(){};
    
    virtual Ray InitRay(float x, float y)
    {
        
        Ray primRay;
        
        primRay.seed = int(width * y + x);
        
        primRay.origin = position;
        Vector3f Pc = position + (primRay.tmin * viewV.normalized());
        Vector3f n0 = (upV.cross(viewV)).normalized();
        Vector3f n1 = viewV.normalized().cross(n0);
        float sx = width/height;
        float sy = 1.0;
        Vector3f Po = Pc - n0*(sx/2.0) - n1 * (sy/2.0);
        
        Vector3f Pp = Po + n0 * sx * x + n1 * sy * y;
        primRay.direction = (Pp - primRay.origin).normalized();
        
        
        //inputImage = shader->getTexture("image");
		if (inputImage != NULL) {
			color c = inputImage->getTexel(PAIR(sx * x, sy * y));
			float nx = 2 * c.r - 1;
			float ny = 2 * c.g - 1;
			float nz = 2 * c.b - 1;
			
			primRay.origin[0] += nx * scale;
            primRay.origin[1] += ny * scale;
            primRay.origin[2] += nz * scale;
			
		}
        
        return primRay;
    }
    
};


#endif
