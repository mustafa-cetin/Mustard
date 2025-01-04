#include "gl_renderer.h"

// open gl structs

struct GLContext
{
   GLuint programID;
};

// open gl globals
static GLContext glContext;

// open gl functions
static void APIENTRY gl_debug_callback(GLenum source,GLenum type,GLuint id, GLenum severity, GLsizei length,const GLchar* message,const void* user)
{
    if (severity==GL_DEBUG_SEVERITY_LOW || GL_DEBUG_SEVERITY_MEDIUM || GL_DEBUG_SEVERITY_HIGH)
    {
        SM_ASSERT(0,"OPENGL ERROR: %s",message);
    }
    else
    {
        SM_TRACE((char*)message);
    }
}


bool gl_init(BumpAllocator* transientStorage)
{
    load_gl_functions();

    glDebugMessageCallback(&gl_debug_callback, nullptr);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glEnable(GL_DEBUG_OUTPUT);

    GLuint vertShaderID= glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShaderID= glCreateShader(GL_FRAGMENT_SHADER);

    int fileSize=0;
    char* vertexShader = read_file("assets/shaders/quad.vert",&fileSize,transientStorage);
    char* fragShader = read_file("assets/shaders/quad.frag",&fileSize,transientStorage);
    
    if (!vertexShader || !fragShader)
    {
        SM_ASSERT(0,"Failed to load shaders");
        return false;
    }
    
    glShaderSource(vertShaderID,1,&vertexShader,0);
    glShaderSource(fragShaderID,1,&fragShader,0);
    
    glCompileShader(vertShaderID);
    glCompileShader(fragShaderID);

    // test if vertex shader worked
    {
        int success;
        char shaderLog[2048]={};
        glGetShaderiv(vertShaderID,GL_COMPILE_STATUS,&success);
        if (!success)
        {
            glGetShaderInfoLog(vertShaderID,2048,0,shaderLog);
            SM_ASSERT(0,"Failed to compile vertex shader: %s",shaderLog);
        }

    }
    // test if fragment shader worked
    {
        int success;
        char shaderLog[2048]={};
        glGetShaderiv(fragShaderID,GL_COMPILE_STATUS,&success);
        if (!success)
        {
            glGetShaderInfoLog(fragShaderID,2048,0,shaderLog);
            SM_ASSERT(0,"Failed to compile fragment shader: %s",shaderLog);
        }
    }
    
    glContext.programID=glCreateProgram();
    glAttachShader(glContext.programID,vertShaderID);
    glAttachShader(glContext.programID,fragShaderID);
    glLinkProgram(glContext.programID);


    glDetachShader(glContext.programID,vertShaderID);
    glDetachShader(glContext.programID,fragShaderID);
    glDeleteShader(vertShaderID);
    glDeleteShader(fragShaderID);

    GLuint VAO;
    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);
    
    // depth tesing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GREATER);
    
    // use program
    glUseProgram(glContext.programID);

    return true;
}
void gl_render()
{
    glClearColor(119.0f / 255.0f ,33.0f / 255.0f,111.0f/255.0f,1.f);
    glClearDepth(0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,input.screenSizeX,input.screenSizeY);
    

    glDrawArrays(GL_TRIANGLES,0,6);
}
