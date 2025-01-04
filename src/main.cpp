#include "hexa_lib.h"
#include "input.h"

#include "platform.h"


#define APIENTRY
#define GL_GLEXT_PROTOTYPES
#include "glcorearb.h"

// Windows PLATFORM
#ifdef _WIN32
#include "win32_platform.cpp"
#endif

#include "gl_renderer.cpp"


int main()
{
    BumpAllocator transientStorage=make_bump_allocator(MB(50));

    platform_create_window(1200,720,"HEXAPAWA");
    input.screenSizeX=1200;
    input.screenSizeY=720;
    gl_init(&transientStorage);
    while(running)
    {
    
        // Game loop
        platform_update_window();
        gl_render();
        platform_swap_buffers();
    }

    return 0;
}