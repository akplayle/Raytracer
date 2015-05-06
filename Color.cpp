/*
 *  Color.cpp
 *  Raytracer
 *
 *  Created by Amber on 2/6/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */


#include "Color.h"

using namespace std;

/**********************************************************************/
/* stuff for points (stolen from points.cc) */
/***********************************************************************/
/* creation */
color::color()
{
	r = 0.0;
	g = 0.0;
	b = 0.0;
	a = 1.0;
}
color::color(double ir,
			   double ig,
			   double ib)
{
	r = ir;
	g = ig;
	b = ib;
	a = 1.0;
}
color::color(double ir,
			 double ig,
			 double ib,
			 double ia)
{
	r = ir;
	g = ig;
	b = ib;
	a = ia;
}
color::color(double* v)
{
	r = v[0];
	g = v[1];
	b = v[2];
	a = 1.0;
}

color::color(double* v, float alpha = 0.0f)
{
	r = v[0];
	g = v[1];
	b = v[2];
	a = alpha;
}
color::color(float* v)
{
	r = v[0];
	g = v[1];
	b = v[2];
	a = 1.0;
}
color::color(float* v, float alpha = 0.0f)
{
	r = v[0];
	g = v[1];
	b = v[2];
	a = alpha;
}
color::color(const color& c)
{
	r = c.r;
	g = c.g;
	b = c.b;
	a = c.a;
}

/***********************************************************************/
/* print - use vector # format */
void color::print() const {
	printf("%f, %f, %f\n",r,g,b);  
}

bool color::zero() const {
	if (r == 0 && g == 0 && b == 0) {
		return true;
	}
	else {
		return false;
	}
}


/***********************************************************************/
/* operators - create temps and stick them on stack */
color color::operator + (const color& c) const
{
	color q;
	q.r = r + c.r;
	q.g = g + c.g;
	q.b = b + c.b;
	q.a = a + c.a;
	return(q);
}
color color::operator - (const color& c) const
{
	color q;
	q.r = r - c.r;
	q.g = g - c.g;
	q.b = b - c.b;
	q.a = b - c.a;
	return(q);
}
color color::operator * (const color& c) const 
{
	color q;
	q.r = c.r * r;
	q.g = c.g * g;
	q.b = c.b * b;
	return q;
}
color color::operator * (const double s)  const /* scale */
{
	color q;
	q.r = r*s;
	q.g = g*s;
	q.b = b*s;
	return q;
}

color color::operator - () const /* negate */
{
	color q;
	q.r = -r;
	q.g = -g;
	q.b = -b;
	return q;
}

void color::includeMax(const color& c)
{
	if (c.r > r) r = c.r;
	if (c.g > g) g = c.g;
	if (c.b > b) b = c.b;
}
void color::includeMin(const color& c)
{
	if (c.r < r) r = c.r;
	if (c.g < g) g = c.g;
	if (c.b < b) b = c.b;
}

void color::clamp()
{
	r = max(min(r, 1.0), 0.0);
	g = max(min(g, 1.0), 0.0);
	b = max(min(b, 1.0), 0.0);
}

color color::normalize()
{
	//Conver to HSL
	color hue;
	float cmax = max(r, max(b, g));
	float cmin = min(r, min(b, g));
	float delta = cmax - cmin;
	
	float luminance = 0.2126 * r + 0.7152 * g + 0.0722 * b;
	luminance = luminance / (luminance + 1.0f);
	float L = cmax + cmin / 2.0f;
	L = luminance;
	
	//cout << cmax << " " << cmin << endl;
	
	hue.r = 60 * fmod((g-b)/delta, 6);
	hue.g = 60 * ((b-r)/delta + 2.0);
	hue.b = 60 * ((r-g)/delta + 4.0);
	
	float H;
	if (cmax - r < 0.001) {
		H = hue.r;
	}
	if (cmax - g < 0.001)
	{
		H = hue.g;
	}
	if (cmax - b < 0.001) {
		H = hue.b;
	}
		
	float S;
	if (delta == 0) {
		S = 0;
	}
	else {
		S = delta / (1 - fabs(2 * L - 1.0));
	}
	
	
	//Change Luminance using Global Tone Mapping
	
	
	//float luminance = L / (L+1);
	//cout << "L: " << luminance << endl;
	
	//Convert HSL Back to RGB
	float C = (1.0f - fabs(2.0 * L - 1.0f)) * S;
	float X = C * (1.0f - fabs(fmod((H / 60.0), 2) - 1.0f));
	float m = L - C/2.0;
	
	
	color q;
	if (H >= 0 && H < 60) {
		q.set(C, X, 0);
	}
	if (H >= 60 && H < 120) {
		q.set(X, C, 0);
	}
	if (H >= 120 && H < 180) {
		q.set(0, C, X);
	}
	if (H >= 180 && H < 240) {
		q.set(0, X, C);
	}
	if (H >= 240 && H < 300) {
		q.set(X, 0, C);
	}
	if (H >= 300 && H < 360) {
		q.set(C, 0, X);
	}
	
	if (q.r + m < 0 || q.g + m < 0 || q.b + m < 0) {
		cout << "C: " << C << " X: " << X << " m: " << m << endl;
	}
	
	q.set(q.r + m, q.g + m, q.b + m);
	return q;

	

}

color color::HSL(double hue, double sat, double lum)
{
    hue = fmod(hue, 360);
    
    //Convert HSL Back to RGB
	float C = (1.0f - fabs(2.0 * lum - 1.0f)) * sat;
	float X = C * (1.0f - fabs(fmod((hue / 60.0), 2) - 1.0f));
	float m = lum - C/2.0;
    
	color q;
	if (hue >= 0 && hue < 60) {
		q.set(C, X, 0);
	}
	if (hue >= 60 && hue < 120) {
		q.set(X, C, 0);
	}
	if (hue >= 120 && hue < 180) {
		q.set(0, C, X);
	}
	if (hue >= 180 && hue < 240) {
		q.set(0, X, C);
	}
	if (hue >= 240 && hue < 300) {
		q.set(X, 0, C);
	}
	if (hue >= 300 && hue < 360) {
		q.set(C, 0, X);
	}
	
    //q.print();
    
	if (q.r + m < 0 || q.g + m < 0 || q.b + m < 0) {
		cout << "C: " << C << " X: " << X << " m: " << m << endl;
	}
	
	//q.set(q.r + m, q.g + m, q.b + m);
	return q;

}

float color::average()
{
	return (r + g + b) / 3.0 ;
}

void color::set(double red, double green, double blue)
{
	r=red; g=green; b=blue;
}
