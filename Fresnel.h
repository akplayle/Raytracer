//
//  Fresnel.h
//  Raytracer
//
//  Created by Amber on 4/14/14.
//
//

#ifndef Fresnel_H
#define Fresnel_H

#include "StdDefs.h"

class Fresnel
{
public:
    virtual float Evaluate(float cosi)
    {
        return 0;
    }
};

class FresnelDielectric: public Fresnel
{
private:
    float eta_i;
    float eta_t;
    
    float FrDielectric(float cosi, float cost, float etai, float etat)
    {
        float r_par = (etat*cosi - etai*cost) / (etat * cosi + etai*cost);
        float r_per = (etai*cosi - etat*cost) / (etai*cosi + etat*cost);
        //  Assumption: The light is unpolarised and therefore the Fresnel reflectance is the average
        //  of the squares of the parallel and perpendicular polarization term.
        return 0.5 * (r_par * r_par + r_per * r_per);
    }
    
public:
    FresnelDielectric (float ei, float et) : eta_i(ei), eta_t(et) {};
    
    ~FresnelDielectric(){};
    
    float Evaluate(float cosi)
    {
        bool entering = cosi > 0;
        float ei = entering ? eta_i : eta_t;
        float et = entering ? eta_t : eta_i;
        
        //Compute using Snell's Law
        float sint = ei/et * sqrtf(MAX(0, 1-cosi * cosi));
        
        if (sint > 1) {
            //Total internal reflection
            return 1;
        }
        else
        {
            float cost = sqrtf(MAX(0.f, 1.f - sint * sint));
            return FrDielectric(fabs(cosi), cost, ei, et);
        }
    }
};

class FresnelConductor: public Fresnel
{
private:
    float eta;
    float kappa;
    
public:
    FresnelConductor (float e, float k) : eta(e), kappa(k) {};
    
    ~FresnelConductor() {};
    
    float Evaluate(float cosi)
    {
        cosi = fabsf(cosi);
        float tmp1 = eta*eta + kappa * kappa;
        float tmp2 = cosi * cosi;
        float tmp3 = 2.f * eta * cosi;
        float rparl2 = (tmp1 * tmp2 - tmp3 + 1) / (tmp1 * tmp2 + tmp3 + 1);
        float rperp2 = (tmp1 - tmp3 + tmp2)/(tmp1 + tmp3 + tmp2);
        return (rparl2 + rperp2) / 2.f;
    }
};

#endif
