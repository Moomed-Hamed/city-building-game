//-----------------------------------------------------------------
//  Name        : maths
//  Purpose     : Provide a fast math library
//  Last Updated: 8/8/2020
//-----------------------------------------------------------------

#pragma once
#include "../types.h"
#include <cmath>

#define NULL 0

#define PI	  3.14159265359f
#define TWOPI 6.28318530718f

#define ToRadians(value) ( ((value) * PI) / 180.0f )
#define ToDegrees(value) ( ((value) * 180.0f) / PI )

#define Swap32(a,b) a = a ^ b; b = b ^ a; a = a ^ b;
#define Swap64(a,b) a = a ^ b; b = b ^ a; a = a ^ b;
/*
float max(float a, float b) { if (a > b) return a; else return b; }
float min(float a, float b) { if (a < b) return a; else return b; }

float sin(float n);
float cos(float n);

float sqrt(float n)
{
	// TODO : implement
	return 0;
}
float inv_sqrt(float n)
{
	float x = n * 0.5F;
	float y = n;

	int64 i = *((int64*)&y);   // evil floating point bit level hacking
	i = 0x5f3759df - (i >> 1);	// what the fuck? 
	y = *(float*)&i;

	// (Newton's Approximation)
	y = y * (1.5f - (x * y * y)); // 1st iteration
 //y = y * (1.5f - (x * y * y)); // 2nd iteration
 //y = y * (1.5f - (x * y * y)); // 3rd iteration

	return y;
}
*/