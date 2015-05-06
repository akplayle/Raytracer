#include "StdDefs.h"




float frand()
{
	unsigned int x = rand();
	float val = float(float(x) / float(RAND_MAX));
    return val;
    
}