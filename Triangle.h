/*
 *  Triangle.h
 *  Raytracer
 *
 *  Created by Amber on 3/23/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Primitive.h"

class Triangle : public Primitive
{
private:
	
	Vector3f a;
	Vector3f b;
	Vector3f c;
	Vector3f na;
	Vector3f nb;
	Vector3f nc;
	PAIR ta;
	PAIR tb;
	PAIR tc;

public:
	
	Box calcBounds()
	{
		Box bounds = Box();
		
        Vector3f x(1.0,0.0,0.0);
		Vector3f y(0.0,1.0,0.0);
		Vector3f z(0.0,0.0,1.0);
        
        bounds.Extend(a + position);
		bounds.Extend(b + position);
		bounds.Extend(c + position);
		return bounds;
	}
	
	
	Triangle() : Primitive()
	{
    };
	
	Triangle(Vector3f a, Vector3f b, Vector3f c, 
			 Vector3f na, Vector3f nb, Vector3f nc, 
			 PAIR ta, PAIR tb, PAIR tc)
	: Primitive(), a(a), b(b), c(c), na(na), nb(nb), nc(nc), ta(ta), tb(tb), tc(tc)
	{
    };
	
	
	virtual ~Triangle()
	{}
	
	bool Intersect(Ray &r)
	{
       
        Vector3f p0 = a + position;
        Vector3f p1 = b + position;
        Vector3f p2 = c + position;
        
        Vector3f e1 = p1-p0;
		Vector3f e2 = p2-p0;
		
		const Vector3f pvec = r.direction.cross(e2);

		
		const float det = e1.dot(pvec);
		
		if (fabs(det) < Epsilon) {
			return false;
		}
		
		const float inv_det = 1.0/det;
		
		const Vector3f tvec = r.origin - p0;
		
		float lambda = tvec.dot(pvec);
		lambda *= inv_det;
		
		if (lambda < 0.0f || lambda > 1.0f) {
			return false;
		}
		
		const Vector3f qvec = tvec.cross(e1);
		
				
		float mue = r.direction.dot(qvec);
		mue *= inv_det;
		
		if (mue < 0.0f || mue + lambda > 1.0f) {
			return false;
		}
		
		float th = e2.dot(qvec);
		th = th * inv_det;
		if (th < Epsilon) {
			return false;
		}
		
		r.u = lambda;
		r.v = mue;
		r.t = th;
		r.hit = this;
		r.alpha = 1.0;
		return true;
		
		
		
		
	};
	
	
	virtual Vector3f getNormal(Ray &r)
	{
        Vector3f p0 = a + position;
        Vector3f p1 = b + position;
        Vector3f p2 = c + position;
        
        Vector3f e1 = p1-p0;
		Vector3f e2 = p2-p0;
		Vector3f normal = (e1.cross(e2)).normalized();
		
		Vector3f null;
		null << NULL, NULL, NULL;
		if (na != null && nb != null && nc != null) {
			na = na.normalized();
			nb = nb.normalized();
			nc = nc.normalized();
			
			normal = r.u * nb + r.v * nc + (1 - r.u - r.v) * na;
		}
		
		return normal;
	}
	
	PAIR getUV(Ray &r)
	{
		
		
		float u = r.u;
		float v = r.v;
		
		PAIR null;
		null.first = NULL;
		null.second = NULL;
		
		if (ta != null && tb != null && tc != null) {
			u = r.u * tb.first + r.v * tc.first + (1 - r.u - r.v) * ta.first;
			v = r.u * tb.second + r.v * tc.second + (1 - r.u -r.v) * ta.second;
		}
		
		return PAIR(u,v);
	}
	
};
#endif