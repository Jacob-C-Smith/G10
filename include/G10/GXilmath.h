﻿#pragma once

#define _USE_MATH_DEFINES // For some (utterly stupid) reason, macros for numerical constants are not automatically defined... 
#include <math.h>

static inline float toDegrees( float radians ) // ✅ converts radians to degrees
{
	return (float) radians * (180 / M_PI);  
}

static inline float toRadians( float degrees ) // ✅ converts degrees to radians
{
	return (float) degrees * (M_PI / 180);
}