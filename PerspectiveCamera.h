//
//  PerspectiveCamera.h
//  Raytracer
//
//  Created by Amber on 4/21/14.
//
//

#ifndef PERSPECTIVECAMERA_H
#define PERSPECTIVECAMERA_H

#include "Camera.h"

class PerspectiveCamera : public Camera
{
public:
    PerspectiveCamera() : Camera()
    {}
    
    
    virtual ~PerspectiveCamera(){};
    
    virtual Ray InitRay(float x, float y)
    {
        Ray primRay;
        
        primRay.seed = (y + x) * 123456;
        //cout << primRay.seed << endl;
        
        primRay.origin = position;
        Vector3f Pc = position + (primRay.tmin * viewV.normalized());
        n0 = (upV.cross(viewV)).normalized();
        n1 = viewV.normalized().cross(n0);
        float sx = width/height;
        float sy = 1.0;
        Vector3f Po = Pc - n0*(sx/2.0) - n1 * (sy/2.0);
        
        Vector3f Pp = Po + n0 * sx * x + n1 * sy * y;
        primRay.direction = (Pp - primRay.origin).normalized();
        
        return primRay;
        
    }
};

#endif
