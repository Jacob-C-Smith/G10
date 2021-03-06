#include <G10/GXScene.h>

GXScene_t* createScene ( )
{
    GXScene_t* ret = malloc(sizeof(GXScene_t));

    if (ret == 0)
        return ret;
    
    // Populate with 0's
    ret->entities   = 0;
    ret->cameras    = 0;
    ret->lights     = 0;
    ret->name       = 0;

    return ret;
}

GXScene_t* loadScene ( const char path[] )
{
    // Argument checking
    #ifndef NDEBUG
        if(path == 0)
            goto nullPath;
    #endif

    // Uninitialized data
    int          i;
    char*        data;
    int          rootTokenCount;
    JSONValue_t* rootContents; 

    // Initialized data
    GXScene_t*   ret = createScene();
    size_t       l   = 0;
    FILE*        f   = fopen(path, "rb");

    if (ret == 0)
        return (void*)0;

    // Load the file
    {
        // Check if file is valid
        if (f == NULL)
        {
            printf("Failed to load file %s\n", path);
            return (void*)0;
        }

        // Find file size and prep for read
        fseek(f, 0, SEEK_END);
        i = ftell(f);
        fseek(f, 0, SEEK_SET);

        // Allocate data and read file into memory
        data = malloc(i+1);
        if (data == 0)
            return (void*)0;
        fread(data, 1, i, f);

        // We no longer need the file
        fclose(f);

        // For reasons beyond me, the null terminator isn't included.
        data[i] = '\0';
    }

    // Preparse the scene
    l              = strlen(data);
    rootTokenCount = GXParseJSON(data, i, 0, (void*)0);
    rootContents   = calloc(rootTokenCount, sizeof(JSONValue_t));

    // Parse the scene
    GXParseJSON(data, i, rootTokenCount, rootContents);

    // Find and exfiltrate pertinent information.
    for (size_t j = 0; j < rootTokenCount; j++)
    {
        // Handle comments
        if (strcmp("comment", rootContents[j].name) == 0)
        {
            #ifndef NDEBUG
                // Print out comment
                printf("comment in file \"%s\" - \"%s\"\n\n", path, (char*)rootContents[j].content.nWhere);
            #endif
            continue;
        }

        // Copy out the name of the scene
        if (strcmp("name", rootContents[j].name) == 0)
        {
            // Initialized data.
            size_t len = strlen(rootContents[j].content.nWhere);
            ret->name  = calloc(len+1,sizeof(char));

            // Copy the string
            strncpy(ret->name, rootContents[j].content.nWhere, len);

            continue;
        }

        // Create entities
        if (strcmp("entities", rootContents[j].name) == 0)
        {
            for (size_t k = 0; rootContents[j].content.aWhere[k]; k++)
                appendEntity(ret, (*(char*)rootContents[j].content.aWhere[k] == '{') ? loadEntityAsJSON(rootContents[j].content.aWhere[k]) : loadEntity(rootContents[j].content.aWhere[k]));
            continue;
        }

        // Create cameras
        else if (strcmp("cameras", rootContents[j].name) == 0)
        {
            for (size_t k = 0; rootContents[j].content.aWhere[k]; k++)
                appendCamera(ret, (*(char*)rootContents[j].content.aWhere[k] == '{') ? loadCameraAsJSON(rootContents[j].content.aWhere[k]) : loadCamera(rootContents[j].content.aWhere[k]));
            continue;
        }

        // Set up lights
        else if (strcmp("lights", rootContents[j].name) == 0)
        {
            for (size_t k = 0; rootContents[j].content.aWhere[k]; k++)
                appendLight(ret, (*(char*)rootContents[j].content.aWhere[k] == '{') ? loadLightAsJSON(rootContents[j].content.aWhere[k]) : loadLight(rootContents[j].content.aWhere[k]));
            continue;
        }
    }

    // Finish up
    free(rootContents);

    return ret;

    // The path argument was null
    nullPath:
    #ifndef NDEBUG
        printf("Null pointer provided for \"path\" in function \"%s\"\n", __FUNCSIG__);
    #endif
        return 0;
}

int appendEntity ( GXScene_t* scene, GXEntity_t* entity )
{
    // Argument checking
    {
        #ifndef NDEBUG
            if(scene == 0)
                goto nullScene;
            if(entity == 0)
                goto nullEntity;
        #endif
    }

    // Set the pointer to the head of the linked list
    GXEntity_t* i = scene->entities;

    // Check if the head is null. If so, set the head to the entity
    if (i == 0)
    {
        scene->entities = entity;
        return 0;
    }

    // Search for the end of the linked list, and check every entity 
    while (i->next)
    {
        // Error checking
        if (strcmp(i->name, entity->name) == 0)
            goto duplicateName;
        i = i->next;
    }

    // Assign next as entity
    i->next = entity;

    return 0;

    // Error handling
    {
        // Two entities with the same name cannot exist in the same scene
        duplicateName:
        #ifndef NDEBUG
            printf("Entity \"%s\" can not be appended to \"%s\" because an entity with that name already exists\n", entity->name, scene->name);
        #endif
        return 0;

        // The scene argument was null
        nullScene:
        #ifndef NDEBUG
            printf("Null pointer provided for scene in function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;

        // The entity argument was null
        nullEntity:
        #ifndef NDEBUG
            printf("Null pointer provided for entity in function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

int appendCamera ( GXScene_t* scene, GXCamera_t* camera ) 
{
    // Argument checking
    {
        #ifndef NDEBUG
            if(scene == 0)
                goto nullScene;
            if(camera == 0)
                goto nullCamera;
        #endif
    }

    // Set the pointer to the head of the linked list
    GXCamera_t* i = scene->cameras;

    // Check if the head is null. If so, set the head to the entity
    if (i == 0)
    {
        scene->cameras = camera;
        return 0;
    }

    // Search for the end of the linked list
    while (i->next)
    {
        // Error checking
        if (strcmp(i->name, camera->name) == 0)
            goto duplicateName;
        i = i->next;
    }

    // Assign next as entity
    i->next = camera;

    return 0;

    // Error handling
    {
        // Two cameras with the same name cannot exist in the same scene
        duplicateName:
        #ifndef NDEBUG
            printf("Camera \"%s\" can not be appended to \"%s\" because a camera with that name already exists\n", camera->name, scene->name);
        #endif
        return 0;
    
        // The scene argument was null
        nullScene:
        #ifndef NDEBUG
            printf("Null pointer provided for scene in function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;

        // The entity argument was null
        nullCamera:
        #ifndef NDEBUG
            printf("Null pointer provided for camera in function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

int appendLight  ( GXScene_t* scene, GXLight_t* light )
{
    // Argument checking
    {
        #ifndef NDEBUG
            if(scene == 0)
                goto nullScene;
            if(light == 0)
                goto nullLight;
        #endif
    }

    // Set the pointer to the head of the linked list
    GXLight_t* i = scene->lights;

    // Check if the head is null. If so, set the head to the light
    if (i == 0)
    {
        scene->lights = light;
        return 0;
    }

    // Search for the end of the linked list
    while (i->next)
    {
        // Error checking
        if (strcmp(i->name, light->name) == 0)
            goto duplicateName;
        i = i->next;
    }
    // Assign next as light
    i->next = light;

    return 0;

    // Error handling 
    {
        // Two lights with the same name cannot exist in the same scene
        duplicateName:
        #ifndef NDEBUG
            printf("Light \"%s\" can not be appended to \"%s\" because a light with that name already exists\n", light->name, scene->name);
        #endif
        return 0;

        // The entity argument was null
        nullScene:
        #ifndef NDEBUG
            printf("Null pointer provided for scene in function \"%s\"\n", __FUNCSIG__);
        #endif
            return 0;
        // The entity argument was null
        nullLight:
        #ifndef NDEBUG
            printf("Null pointer provided for light in function \"%s\"\n", __FUNCSIG__);
        #endif
            return 0;
    }
}

int drawScene ( GXScene_t* scene )
{
    // Argument checking 
    {
        #ifndef NDEBUG
            if (scene == 0)
                goto nullScene;
        #endif
    }

    // Initialized data
    GXEntity_t* i = scene->entities;

    // Iterate through list until we hit nullptr
    while (i)
    {
        // Set up the shader
        {
            // Use it
            useShader(i->shader);

            // Set some uniforms for the shader
            if (i->shader->requestedDataFlags & GXSP_Projection)
                setShaderMat4(i->shader, findValue(i->shader->requestedData, i->shader->requestedDataCount, GXSP_Projection), (const GLfloat*)&scene->cameras->projection);
            if (i->shader->requestedDataFlags & GXSP_View)
                setShaderMat4(i->shader, findValue(i->shader->requestedData, i->shader->requestedDataCount, GXSP_View), (const GLfloat*)&scene->cameras->view);
            if (i->shader->requestedDataFlags & GXSP_CameraPosition)
                setShaderVec3(i->shader, findValue(i->shader->requestedData, i->shader->requestedDataCount, GXSP_CameraPosition), scene->cameras->where);
        }
        
        // Set up lights
        {
            GXLight_t* light = scene->lights;
            if(light == (void*)0)
                goto noLight;

            char* buffer = malloc(512);
            if (buffer == (void*)0)
               return 0;

            // TODO: Dynamically determine max lights from graphics settings and machine hardware
            for(size_t j = 0; j < GX_MAX_LIGHTS && light; j++)
            {				
                buffer[sprintf(buffer, "%s[%lld]\0", findValue(i->shader->requestedData,i->shader->requestedDataCount,GXSP_LightPosition), j) + 1] = 0;
                setShaderVec3(i->shader, buffer, light->location);
                buffer[sprintf(buffer, "%s[%lld]\0", findValue(i->shader->requestedData, i->shader->requestedDataCount, GXSP_LightColor), j) + 1] = 0;
                setShaderVec3(i->shader, buffer, light->color);
            
                light = light->next;
            }

            free(buffer);
        }
        noLight:

        // Actually draw the entity
        drawEntity(i);
       
        // Rinse and repeat
        i = i->next;
        
    }

    return 0;

    // Error handling
    {
        nullScene:
        #ifndef NDEBUG
            printf("Null pointer provided for scene in function \"%s\".\n", __FUNCSIG__);
        #endif
            return 0;
    }
    
}

int computePhysics ( GXScene_t* scene, float deltaTime )
{
    // TODO: Compute physics
    GXEntity_t* i = scene->entities;
    while (i && i->rigidbody)
    {
        if (i->rigidbody)
        {
            // Summate forces 
            SSESumVecs(i->rigidbody->forcesCount, i->rigidbody->forces);

            // Calculate new acceleration, velocity, and position
            calculateDerivativesOfDisplacement(i, deltaTime);

            // TODO: Calculate angular acceleration, velocity, and rotation
        }

        i = i->next;
    }
    return 0;
}

GXEntity_t* getEntity ( GXScene_t* scene, const char name[] )
{
    // Argument checking
    {
        #ifndef NDEBUG
            if (scene == 0)
                goto nullScene;
            if (name == 0)
                goto nullName;
        #endif
    }

    // Initialized data
    GXEntity_t* i = scene->entities;

    // Sanity check
    if (i == 0)
        goto noEntities;
    
    // Iterate through list until we hit the entity we want, or zero
    while (i)
    {
        if (strcmp(name, i->name) == 0)
            return i; // If able to locate the entity in question, return a pointer
        i = i->next;
    }
    
    // Unable to locate entity
    goto noMatch;

    // Error handling
    {
        // There are no entities
        noEntities:
        #ifndef NDEBUG
            printf("There are no entities.\n");
        #endif
        return 0;	

        // There is no match
        noMatch:
        #ifndef NDEBUG
            printf("There is no entity named \"%s\".", name);
        #endif
        return 0;
    
        // The scene parameter was null
        // The scene parameter was null
        nullScene:
        #ifndef NDEBUG
            printf("Null pointer provided for scene in function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    
        // The name parameter was null
        nullName:
        #ifndef NDEBUG
            printf("Null pointer provided for name in function \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

GXCamera_t* getCamera( GXScene_t* scene, const char name[] )
{
    // Argument checking
    {
        #ifndef NDEBUG
            if (scene == 0)
                goto nullScene;
            if (name == 0)
                goto nullName;
        #endif  
    }

    // Create a pointer to the head of the list
    GXCamera_t* i = scene->cameras;

    // Check to see if there are even any cameras in the first place
    if (i == 0)
        goto noCameras; 

    // Iterate through list until we hit the camera we want, or zero
    while (i)
    {
        if (strcmp(name, i->name) == 0)
            return i; // If able to locate the camera in question, return a pointer
        i = i->next;
    }

    // Unable to locate camera
    goto noMatch;

    // Error handling
    {
        // There are no cameras
        noCameras:
        #ifndef NDEBUG
            printf("There are no cameras in \"%s\".\n", scene->name);
        #endif
        return 0;

        // There is no matching camera
        noMatch:
        #ifndef NDEBUG
            printf("There is no camera in \"%s\" named \"%s\".", scene->name, name);
        #endif
        return 0;
    
        // The scene parameter was null
        nullScene:
        #ifndef NDEBUG
            printf("Null pointer provided to \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    
        // The name parameter was null
        nullName:
        #ifndef NDEBUG
            printf("Null pointer provided to \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

GXLight_t* getLight( GXScene_t* scene, const char name[] )
{
    // Create a pointer to the head of the list
    GXLight_t* i = scene->lights;

    // Sanity check
    if (i == 0)
        goto noLights;

    // Iterate through list until we hit the light we want, or zero
    while (i)
    {
        if (strcmp(name, i->name) == 0)
            return i; // If able to locate the light in question, return a pointer
        i = i->next;
    }

    // Unable to locate light
    goto noMatch;
    {
        noLights:
        #ifndef NDEBUG
            printf("There are no lights in \"%s\".\n", scene->name);
        #endif
        return 0;

        noMatch:
        #ifndef NDEBUG
            printf("There is no light in \"%s\" named \"%s\".", scene->name, name);
        #endif
        return 0;
    }
}

int setActiveCamera ( GXScene_t* scene, const char name[] )
{
    // Arguments checking
    {
        #ifndef NDEBUG
            if (scene == 0)
                goto nullScene;
            if (name == 0)
                goto nullName;
        #endif
    }
    // Create a pointer to the head of the list 
    GXCamera_t* i = scene->cameras;
    
    // Quick sanity check
    if (i == 0)
        goto noCameras;

    // Check the head
    if (strcmp(name, i->name) == 0)
        goto noNeed;

    // Find the named camera
    while (i->next)
    {
        if (strcmp(name, i->next->name) == 0)
        {
            GXCamera_t* j = i->next;
            GXCamera_t* k = i->next->next;
    
            // Stitch up the linked list 
            i->next = k;
            j->next = scene->cameras;
            scene->cameras = j;
            return 0;
        }
        i = i->next;
    }

    goto noMatch;

    // Error checking
    {
        noCameras:
        #ifndef NDEBUG
            printf("There are no cameras, therefore no active camera.\n");
        #endif
        return 0;

        noNeed:
        #ifndef NDEBUG
            printf("\"%s\" is already the active camera.\n", name);
        #endif
        return 0;

        noMatch:
        #ifndef NDEBUG
            printf("There is no camera in \"%s\" named \"%s\".\n", scene->name, name);
        #endif
        return 0;

        // The scene parameter was null
        nullScene:
        #ifndef NDEBUG
            printf("Null pointer provided to \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    
        // The name parameter was null
        nullName:
        #ifndef NDEBUG
            printf("Null pointer provided to \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;

    }
}

int removeEntity( GXScene_t* scene, const char name[] )
{
    // Argument checking
    {
        #ifndef NDEBUG
            if (scene == 0)
                goto nullScene;
            if (name == 0)
                goto nullName;
        #endif
    }

    // Create a pointer to the head of the list
    GXEntity_t* i = scene->entities;
    
    // Quick sanity check
    if (i == 0)
        goto noEntities;
    
    // Check the head
    if (strcmp(name, i->name) == 0)
    {
        GXEntity_t* j = i->next;
        destroyEntity(i);
        i = j;
        return 0;
    }

    // Destroy the named entity
    while (i->next)
    {
        if (strcmp(name, i->next->name) == 0) 
        {
            // Make a copy of the entity that is 2 positions ahead of the current entity
            GXEntity_t* j = i->next->next;
            
            // Delete the entity
            destroyEntity(i->next);

            // Verbose logging
            #ifndef NDEBUG
                printf("Destroyed Entity \"%s\"\n", name);
            #endif
            
            // Stitch up the linked list 
            i->next = j;
            return 0;
        }
        i = i->next;
    }

    goto noMatch;

    // Error handling
    {
        noEntities:
            #ifndef NDEBUG
                printf("There are no entities in \"%s\".\n", scene->name);
            #endif
            return 0;

        noMatch:
            #ifndef NDEBUG
                printf("There is no entity in \"%s\" named \"%s\".\n", scene->name, name);
            #endif
            return 0;
        
        // The scene parameter was null
        nullScene:
        #ifndef NDEBUG
            printf("Null pointer provided to \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    
        // The name parameter was null
        nullName:
        #ifndef NDEBUG
            printf("Null pointer provided to \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

int removeCamera ( GXScene_t* scene, const char name[] )
{
    // Argument checking
    {
        if (scene == 0)
            goto nullScene;
        if (name == 0)
            goto nullName;
    }

    // Create a pointer to the head of the list
    GXCamera_t* i = scene->cameras;

    // Quick sanity check
    if (i == 0)
        goto noCameras;

    // Check the head
    if (strcmp(name, i->name) == 0)
    {
        GXCamera_t* j = i->next;
        destroyCamera(i);
        i = j;
        return 0;
    }

    // Destroy the named camera
    while (i->next)
    {
        if (strcmp(name, i->next->name) == 0)
        {
            // Make a copy of the camera that is 2 positions ahead of the current camera
            GXCamera_t* j = i->next->next;

            // Delete the camera
            destroyCamera(i->next);

            // Verbose logging
            #ifndef NDEBUG
                printf("Destroyed Camera \"%s\"\n", name);
            #endif		

            // Stitch up the linked list 
            i->next = j;
            return 0;
        }
        i = i->next;
    }

    goto noMatch;
    
    // Error handling
    {
        noCameras:
        #ifndef NDEBUG
            printf("There are no cameras in \"%s\"", scene->name);
        #endif
        return 0;
        noMatch:
        #ifndef NDEBUG
            printf("There is no camera in \"%s\" named \"%s\".", scene->name, name);
        #endif
        return 0;
        
        // The scene parameter was null
        nullScene:
        #ifndef NDEBUG
            printf("Null pointer provided to \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    
        // The name parameter was null
        nullName:
        #ifndef NDEBUG
            printf("Null pointer provided to \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}

int removeLight( GXScene_t* scene, const char name[] )
{
    // Argument checking 
    {
        #ifndef NDEBUG
            if (scene == 0)
                goto nullScene;
            if (name == 0)
                goto nullName;
        #endif
    }

    // Create a pointer to the head of the list
    GXLight_t* i = scene->lights;

    // Quick sanity check
    if (i == 0)
        goto noLights;

    // Check the head
    if (strcmp(name, i->name) == 0)
    {
        GXLight_t* j = i->next; // Get a pointer to the next object so we don't lose it when we destroy the light
        destroyLight(i);        // Destroy the light
        i = j;                  // Stitch up the linked list
        return 0;
    }

    // Destroy the named light
    while (i->next)
    {
        if (strcmp(name, i->next->name) == 0)
        {
            // Make a copy of the light that is 2 positions ahead of the current light
            GXLight_t* j = i->next->next;

            // Delete the light
            destroyLight(i->next);

            // Verbose logging
            #ifndef NDEBUG
                printf("Destroyed light \"%s\"\n", name);
            #endif		

            // Stitch up the linked list 
            i->next = j;
            return 0;
        }
        i = i->next;
    }
    goto noMatch;

    // Error handling
    {
        noLights:
        #ifndef NDEBUG
            printf("There are no lights in \"%s\"", scene->name);
        #endif
        return 0;
        noMatch:
        #ifndef NDEBUG
            printf("There is no light in \"%s\" named \"%s\".", scene->name, name);
        #endif
        return 0;

        // The scene parameter was null
        nullScene:
        #ifndef NDEBUG
            printf("Null pointer provided to \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    
        // The name parameter was null
        nullName:
        #ifndef NDEBUG
            printf("Null pointer provided to \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
        }
}

int destroyScene ( GXScene_t* scene )
{
    // Argument checking 
    {
        #ifndef NDEBUG
            if (scene == 0)
                goto nullScene;
        #endif
    }

    // Set sceneID to zero;
    GXEntity_t* i = scene->entities;
    GXCamera_t* k = scene->cameras;
    GXLight_t*  l = scene->lights;

    free(scene->name);

    // Destroy all entities in the scene
    while (i)
    {
        GXEntity_t* j = i;
        i = i->next;
        destroyEntity(j);
    }

    // Zero set the entites pointer
    scene->entities   = (void*)0;

    // Destroy the cameras
    while (k)
    {
        GXCamera_t* j = k;
        k = k->next;
        destroyCamera(j);
    }

    // Zero set the cameras pointer
    scene->cameras = (void*)0;

    // Destroy the lights
    while (l)
    {
        GXLight_t* j = l;
        l = l->next;
        destroyLight(j);
    }

    // Zero set the lights pointer
    scene->lights = (void*)0;

    // Free up the scene
    free(scene);

    return 0;

    // Error handling 
    {
        // The scene parameter was null
        nullScene:
        #ifndef NDEBUG
            printf("Null pointer provided to \"%s\"\n", __FUNCSIG__);
        #endif
        return 0;
    }
}