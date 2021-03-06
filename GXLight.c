#include <G10/GXLight.h>

GXLight_t* createLight( )
{
    // Initialized data
    GXLight_t* ret = malloc(sizeof(GXLight_t));

    if (ret == (void*)0)
        return ret;

    // Set everything to zero
    ret->name = (void*)0;

    ret->color.x    = 0.f;
    ret->color.y    = 0.f;
    ret->color.z    = 0.f;
    ret->color.w    = 0.f;

    ret->location.x = 0.f;
    ret->location.y = 0.f;
    ret->location.z = 0.f;
    ret->location.w = 0.f;

    ret->next       = 0;

    // Return
    return ret;
}

GXLight_t* loadLight(const char path[])
{
    // Uninitialized data
    u8*        data;
    GXLight_t* ret;

    // Initialized data
    size_t       l = 0;
    FILE*        f = fopen(path, "rb");

    #ifndef NDEBUG
        printf("Loading \"%s\".\n", (char*)path);
    #endif	

    // Load the file
    {
        // Check for file
        if (f == NULL)
        {
            printf("Failed to load file %s\n", path);
            return (void*)0;
        }

        // Find file size and prep for read
        fseek(f, 0, SEEK_END);
        l = ftell(f);
        fseek(f, 0, SEEK_SET);

        // Allocate data and read file into memory
        data = malloc(l + 1);
        fread(data, 1, l, f);

        // We no longer need the file
        fclose(f);
    }

    ret = loadLightAsJSON(data);

    // Finish up
    free(data);

    return ret;
}

GXLight_t* loadLightAsJSON(char* token)
{
    // Initialized data
    GXLight_t*   ret        = createLight();
    size_t       len        = strlen(token);
    size_t       tokenCount = GXParseJSON(token, len, 0, (void*)0);
    JSONValue_t* tokens     = calloc(tokenCount, sizeof(JSONValue_t));

    // Parse the camera object
    GXParseJSON(token, len, tokenCount, tokens);

    // Iterate through key / value pairs to find relevent information
    for (size_t l = 0; l < tokenCount; l++)
    {
        // Parse out the light name
        if (strcmp("name", tokens[l].name) == 0)
        {
            ret->name = malloc(strlen(tokens[l].content.nWhere)+1);
            if (ret->name == (void*)0)
                return 0;
            strcpy(ret->name, (const char*)tokens[l].content.nWhere);
            continue;
        }

        // Parse out light color
        if (strcmp("color", tokens[l].name) == 0)
        {
            ret->color = (GXvec3_t){ (float)atof(tokens[l].content.aWhere[0]), (float)atof(tokens[l].content.aWhere[1]), (float)atof(tokens[l].content.aWhere[2]) };
            continue;
        }

        // Parse out light position
        if (strcmp("position", tokens[l].name) == 0)
        {
            ret->location = (GXvec3_t){ (float)atof(tokens[l].content.aWhere[0]), (float)atof(tokens[l].content.aWhere[1]), (float)atof(tokens[l].content.aWhere[2]) };
            continue;
        }
    }

    free(tokens);
    ret->next   = 0;

    return ret;
}

int destroyLight(GXLight_t* light)
{
    free((void*)light->name);

    light->color.x    = 0.f,
    light->color.y    = 0.f,
    light->color.z    = 0.f;

    light->location.x = 0.f,
    light->location.y = 0.f,
    light->location.z = 0.f;

    light->next = 0;

    free(light);

    return 0;
}