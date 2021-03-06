#include <G10/GXTexture.h>

#include <G10/G10.h>
#include <G10/GXBitmap.h>
#include <G10/GXJPG.h>
#include <G10/GXPNG.h>

GXTexture_t* createTexture ( )
{
	GXTexture_t* ret = malloc(sizeof(GXTexture_t));
	if (ret == 0)
		return ret;

	ret->width              = 0,
	ret->height             = 0,
	ret->textureID          = 0;
	ret->textureUnitIndex = -1;

	return ret;
}

GXTexture_t* loadTexture ( const char path[] )
{
	// Initialized data
	char*        fileExtension = 1+strrchr(path, '.');
	GXTexture_t* ret           = (void*)0;

	// Figure out what type of file we are dealing with throught the extenstion. This is an admittedly naive approach,
	// but each loader function checks for signatures, so any error handling is handed off to them. 
	if (strcmp(fileExtension, "png") == 0 || strcmp(fileExtension, "PNG") == 0)
		return loadPNGImage(path); 
	else if (strcmp(fileExtension, "bmp") == 0 || strcmp(fileExtension, "dib") == 0 ||
		     strcmp(fileExtension, "BMP") == 0 || strcmp(fileExtension, "DIB") == 0)
		return loadBMPImage(path);
	else if (strcmp(fileExtension, "jpg")  == 0 || strcmp(fileExtension, "jpeg") == 0 ||
		     strcmp(fileExtension, "jpe")  == 0 || strcmp(fileExtension, "jif")  == 0 ||
	  	     strcmp(fileExtension, "jfif") == 0 || strcmp(fileExtension, "jfi")  == 0 ||
		     strcmp(fileExtension, "JPG")  == 0 || strcmp(fileExtension, "JPEG") == 0 ||
		     strcmp(fileExtension, "JPE")  == 0 || strcmp(fileExtension, "JIF")  == 0 ||
		     strcmp(fileExtension, "JFIF") == 0 || strcmp(fileExtension, "JFI")  == 0)
		return loadJPGImage(path);
	else
	#ifndef NDEBUG
		printf("Could not load file %s, unrecognized file extension.\n", path);
	#endif

	return ret;
}

/*
unsigned int loadTextureToTextureUnit(GXTexture_t* image, GXActiveTextures_t* activeTexures)
{
	// Initialized data
	size_t i = 0;

	// Iterate over all active textures
	for (; i < activeTexures->activeTextureCount; i++)
	{
		// Check if the active texture unit in the block is not allocated
		if (activeTexures->activeTextureBlock[i].textureUnitIndex == -1)
		{
			// Bind the texture to the unused texture unit
			activeTexures->activeTextureBlock[i].textureUnitIndex = i;                            // Set the texture unit index in the texture
			glActiveTexture(GL_TEXTURE0 + activeTexures->activeTextureBlock[i].textureUnitIndex); // Set the active texture unit
			glBindTexture(GL_TEXTURE_2D, activeTexures->activeTextureBlock[i].textureID);         // Set the active texture to the corresponding unit
			return i;
		}
	}

	return -1;
}
*/
int unloadTexture ( GXTexture_t* image )
{
	// Invalidate width, height.
	image->width  = 0,
	image->height = 0;

	// Delete OpenGL buffers
	glDeleteTextures(1, &image->textureID);
	image->textureID = 0;

	// Free the image
	free(image);

	return 0;
}

