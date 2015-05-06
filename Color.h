/*
 *  Color.h
 *  Raytracer
 *
 *  Created by Amber on 2/6/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef COLOR_H
#define COLOR_H

#include <algorithm>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>


class color
{
public:
	double r;
	double g;
	double b;
	double a;
	
	color();
	color(double r, double g, double b);
	color(double r, double g, double b, double a = 1.0);
	color(double*);
	color(double*, float alpha);
	color(float*);
	color(float*, float alpha);
	color(const color&);
	
	
	// math operations
	int operator == (const color&) const;	/* equality */
	int operator != (const color&) const;	/* non-equality */
	void operator += (const color&);	/* alter */
	void operator -= (const color&);	/* alter */
	void operator *= (const double);	/* alter (scale) */
	void operator /= (const double); 	/* also useful */
	color operator + (const color&) const;	/* create a temp */
	color operator - (const color&) const;	/* create a temp */
	color operator * (const color&) const;	/* cross product */
	color operator * (const double)	const; /* scale */
	color operator - () const; /* negate */
	
	color interpolated(const color other, float f)
	{
		color temp;
		temp.r = (1.0f - f) * r + f * other.r;
		temp.g = (1.0f - f) * g + f * other.g;
		temp.b = (1.0f - f) * b + f * other.b;
		
		return temp;
	}
	
	/***** OTHER  */
	bool zero() const;
	void print() const;
    color HSL(double hue, double sat, double lum);
	void includeMax(const color&);
	void includeMin(const color&);
	double Red() const { return r; };
	double Green() const { return g; };
	double Blue() const { return b; };
	double Alpha() const { return a; }
	// maximum (useful for determining
	// ranges) - expand the point if the
	// other point is bigger (along each
	// dimension)
	float average();
	color normalize();
	void clamp();
	void fromArray(const double from[3]);	// copy to and from an array
	void toArray(double to[3]) const;
	void set(double red, double green, double blue);
	
	inline double dot(const color& c) const {  return r*c.r + g*c.g + b*c.b; }
		
	inline friend  color	operator * (const double s, const color& c ){ return color(c.r*s,c.g*s,c.b*s,c.a*s);};
	inline friend  color	operator / (const color& c, const double s ){ return color(c.r/s,c.g/s,c.b/s,c.a/s);};
	inline friend  double	operator % (const color& c1, const color& c2){ return c1.Red()*c2.Red()+c1.Green()*c2.Green()+c1.Blue()*c2.Blue();};
	
};

inline int color::operator==(const color& c) const
	{return((r==c.r) && (g==c.g) && (b==c.b) && (a == c.a));}
inline int color::operator!=(const color& c) const
	{return( (r != c.r) || (g != c.g) || (b != c.b) || (a != c.a)); }
inline void color::operator+=(const color& c)
	{ r+= c.r; g+= c.g; b+= c.b; a+=c.a; }
inline void color::operator-=(const color& c)
	{ r-= c.r; g-= c.g; b-= c.b; a-= c.a; }
inline void color::operator*=(const double s)
	{ r *= s; g *= s; b *= s; a *= s; }
inline void color::operator/=(const double s)
	{  r /= s; g /= s; b /= s; a /= s; }
inline void color::fromArray(const double from[3])
	{ r=from[0]; g=from[1]; b=from[2]; a = 1;}
	



#endif