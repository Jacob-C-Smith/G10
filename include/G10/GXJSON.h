﻿#pragma once	

#include <G10/GXDebug.h>
#include <G10/GXtypedef.h>
#include <stdio.h>
#include <string.h>

// Enumeration for the types of JSON objects
enum GXJSONValue_e
{
	GXJSONobject    = 0, // Anything within a "{ ... }"
	GXJSONarray     = 1, // Any array 
	GXJSONstring    = 2, // Any string
	GXJSONprimative = 3  // Any other value
};

//Container for the 
struct JSONValue_s
{
	char*              name;  // The key
	enum GXJSONValue_e type;  // The type
	void*              where; // Where the value appears, if type is array, then where is double pointer to text in array
};
typedef struct JSONValue_s JSONValue_t;

int GXParseJSON (char* text, size_t len, size_t count, JSONValue_t* where); // ✅ Parses a JSON file and places tokens at an array of JSONValue_t* tokens. 
                                                                            //   If the function is called when the where parameter is nullptr, the
                                                                            //   function will return the number of entities that need to be allocated for.