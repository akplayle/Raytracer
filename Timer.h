/*
 *  Timer.h
 *  Raytracer
 *
 *  Created by Amber on 3/16/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef TIMER_H
#define TIMER_H

class Timer
{
	
private:
	unsigned int ticks;
	
public:
	Timer()
	{
		ticks = 0;
	}
	
	~Timer() {};
	
	void setTime(unsigned int t)
	{
		ticks = t;
	}
	
	unsigned int getTime()
	{
		return ticks;
	}
	
	void Tick()
	{
		ticks++;
	}
	
	void Restart()
	{
		ticks = 0;
	}
};
#endif