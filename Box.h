/*
 *  Box.h
 *  Raytracer
 *
 *  Created by Amber on 2/13/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef BOX_H
#define BOX_H

#include "Ray.h"

using namespace std;

class Box
{
public:
	Vector3f min;
	Vector3f max;
	
	Box()
	{
		Clear();
	};
	
	Box(const Box& b)
	{
		min = b.min;
		max = b.max;
	}
	
	inline void Extend(Vector3f a)
	{
		min = setMin(min, a);
		max = setMax(max, a);
	}
	
	Vector3f setMin(Vector3f a, Vector3f b)
	{
		Vector3f q;
		q[0] = a[0] < b[0] ? a[0] : b[0];
		q[1] = a[1] < b[1] ? a[1] : b[1];
		q[2] = a[2] < b[2] ? a[2] : b[2];
		return q;
	}
	
	Vector3f setMax(Vector3f a, Vector3f b)
	{
		Vector3f q;
		q[0] = a[0] > b[0] ? a[0] : b[0];
		q[1] = a[1] > b[1] ? a[1] : b[1];
		q[2] = a[2] > b[2] ? a[2] : b[2];
		return q;
	}
	
	void Clear()
	{
		min << Infinity, Infinity, Infinity;
		max << -Infinity, -Infinity, -Infinity;
	}
	
	inline void Extend(Box box)
	{
		Extend(box.min);
		Extend(box.max);
	}
	
	PAIR Intersect(const Ray &r)
	{
		float d0 = -Infinity, d1 = Infinity;
		
		if (fabs(r.direction[0]) > Epsilon) {
			d0 = (min[0] - r.origin[0]) / r.direction[0];
			d1 = (max[0] - r.origin[0]) / r.direction[0];
			
			if (d1 < d0) {
				swap(d0, d1);
			}
		}
		
		if (fabs(r.direction[1]) > Epsilon) {
			float t0, t1;
			t0 = (min[1] - r.origin[1]) / r.direction[1];
			t1 = (max[1] - r.origin[1]) / r.direction[1];
			if (t1 < t0) {
				swap(t0, t1);
			}
			d0 = MAX(d0, t0);
			d1 = MIN(d1, t1);
		}
		
		if (fabs(r.direction[2]) > Epsilon) {
			float t0, t1;
			t0 = (min[2] - r.origin[2]) / r.direction[2];
			t1 = (max[2] - r.origin[2]) / r.direction[2];
			if (t1 < t0) {
				swap(t0, t1);
			}
			d0 = MAX(d0, t0);
			d1 = MIN(d1, t1);
		}
		
		if (d1 < d0 || d0 == -Infinity) {
			return PAIR(Infinity, -Infinity);
		}
		else {
			return PAIR(d0,d1);
		}

		
	}
};

#endif