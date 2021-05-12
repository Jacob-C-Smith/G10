#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include <G10/GXLinear.h>

#define GX_MAX_LIGHTS 8 // Maximum number of lights to be used by the shader at a time. 
                        // Subject to change with hardware capabilities, graphics settings, etc.

struct GXLight_s
{
	const char* name;       // The name of the light 

	GXvec4_t   color;       // The color of the light, w component is unused
	GXvec4_t   location;    // The location of the light, w component is last distance from camera

	struct GXLight_s* next; // The next light in the list
};
typedef struct GXLight_s GXLight_t;

GXLight_t* createLight  (  );                 // Creates a blank light 
int        destroyLight ( GXLight_t* light ); // Destroys a light