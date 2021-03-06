﻿#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glad/glad.h>

#include <G10/GXtypedef.h>

// Contains information about a texture
struct GXTexture_s
{
    // Name
    char* name;

    // For OpenGL
    unsigned int textureID;
    unsigned int textureUnitIndex;

    // Resolution
    size_t       width;
    size_t       height;
};
typedef struct GXTexture_s GXTexture_t;

// Constructors
GXTexture_t* createTexture            ( );                                                        // ✅ Creates an empty texture
GXTexture_t* loadTexture              ( const char path[] );                                      // ✅ Loads a texture from the disk

// Texture streaming
/*unsigned int loadTextureToTextureUnit ( GXTexture_t* image, GXActiveTextures_t* activeTextures);*/ // Loads a texture 

// Destructors
int          unloadTexture            ( GXTexture_t* image );                                     // ✅ Unloads a texture and all of its contents

