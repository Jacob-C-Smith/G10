#include <G10/GXPNG.h>

GXTexture_t* loadPNGImage ( const char path[] )
{
    // Uninitialized data
    SDL_Surface* image;
    size_t       allocateSize;
    u8*          data;

    // Initialized data
    GXTexture_t* ret = malloc(sizeof(GXTexture_t));
    SDL_RWops*   r   = SDL_RWFromFile(path, "rb");
    
    // Check allocated memory
    if (ret == 0)
        return ret;

    // Load the PNG through SDL
    image = IMG_LoadPNG_RW(r);
    if (!image)
        goto invalidFile;

    // Copy some information
    allocateSize = (size_t)image->w * (size_t)image->h * (size_t)image->format->BytesPerPixel;
    ret->width   = image->w;
    ret->height  = image->h;

    // Allocate space for the pixel data
    data         = malloc(allocateSize);

    if (data == 0)
        return (void*)0;

    // Copy what we need.
    memcpy(data,image->pixels, allocateSize);

    // Set up the OpenGL texture 
    {
        // Create a texture ID
        glGenTextures(1, &ret->textureID);
        glBindTexture(GL_TEXTURE_2D, ret->textureID);

        // Point it to the right place, use some ternarry operaters to determine color depth
        glTexImage2D(GL_TEXTURE_2D, 0, (image->format->BytesPerPixel == 3) ? GL_RGB : GL_RGBA, ret->width, ret->height, 0, (image->format->BytesPerPixel == 3) ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, data);

        // More OpenGL things
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // Free data. We don't really need the header anymore. 
    free(data);
    SDL_FreeSurface(image);
    SDL_FreeRW(r);

    // Debugger logging
    #ifndef NDEBUG
        printf("[G10] Loaded file \"%s\"\n\n", path);
    #endif 

    return ret;

    invalidFile:
    #ifndef NDEBUG
        printf("[G10] Failed to load file %s\n", path);
    #endif
    return 0;
}