
#ifndef STDDEFS_H
#define STDDEFS_H

#include <cfloat>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <utility>
#include <vector>


#define PAIR std::pair<float,float>
#define Epsilon 0.001
#define PI 3.1415926535897
#define Infinity FLT_MAX

#define randomX 2
#define randomY 75

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

float frand();

#endif