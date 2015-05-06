//
//  DepthOfFieldCamera.h
//  Raytracer
//
//  Created by Amber on 4/21/14.
//
//

#ifndef DEPTHOFFIELDCAMERA_H
#define DEPTHOFFIELDCAMERA_H

#include "Camera.h"

class DepthOfFieldCamera : public Camera
{
public:
    
    float lens;
    float focalD;
    
    DepthOfFieldCamera() : Camera()
    {
        lens = 1.0;
        focalD = 35.0;
    }
    
    
    virtual ~DepthOfFieldCamera(){};
    
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
        
        if (lens > Epsilon) {
            
            //random jitter
            x = (0.5 -frand()) * 0.3;
            y = (0.5 -frand()) * 0.3;
            
            Vector3f fp = primRay.origin + focalD * primRay.direction;
            
            primRay.origin[0] += x;
            primRay.origin[1] += y;
            primRay.direction = (fp - primRay.origin).normalized();
            
        }
        
        
        
        return primRay;
    }
 
};

#endif
