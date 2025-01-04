#pragma once

#include <stdio.h>

// this is get to malloc
#include <stdlib.h>

// this is get to memset
#include <string.h>

// this is get to edit timestamp of files
#include <sys/stat.h>

// defines
#ifdef _WIN32
#define DEBUG_BREAK() __debugbreak()
#elif __linux__
#define DEBUG_BREAK() __builtin_debugtrap()
#elif __APPLE__
#define DEBUG_BREAK() __builtin_trap()
#endif

#define BIT(X) (1 << X)
#define KB(x) (x * (unsigned long long)1024)
#define MB(x) (KB(x) * (unsigned long long)1024)
#define GB(x) (MB(x) * (unsigned long long)1024)  

// logging
enum TextColor
{
    TEXT_COLOR_BLACK,
    TEXT_COLOR_RED,
    TEXT_COLOR_GREEN,
    TEXT_COLOR_YELLOW,
    TEXT_COLOR_BLUE,
    TEXT_COLOR_COUNT
};

template <typename... Args>
void _log(char* prefix,char* message,TextColor textColor,Args... args)
{
    static char* TextColorTable[TEXT_COLOR_COUNT] = {
        "\x1b[30m", // black
        "\x1b[31m", // red
        "\x1b[32m", // green
        "\x1b[33m", // yellow
        "\x1b[34m" // blue
    };
    char formatBuffer[8192]={};
    sprintf(formatBuffer,"%s %s %s \033[0m",TextColorTable[textColor],prefix,message);
    
    char textBuffer[8192]={};
    sprintf(textBuffer,formatBuffer,args...);

    puts(textBuffer);
}

#define SM_TRACE(msg,...) _log("TRACE: ",msg,TEXT_COLOR_GREEN, ##__VA_ARGS__)
#define SM_WARN(msg,...) _log("WARN: ",msg,TEXT_COLOR_YELLOW, ##__VA_ARGS__)
#define SM_ERROR(msg,...) _log("ERROR: ",msg,TEXT_COLOR_RED, ##__VA_ARGS__)

#define SM_ASSERT(x,msg,...)    \
{   \
    if(!(x)) \
    { \
        SM_ERROR(msg,##__VA_ARGS__); \
        DEBUG_BREAK(); \
        SM_ERROR("ASSERTION HIT!!"); \
    } \
}   


// BUMP ALLOCATOR
struct BumpAllocator
{
    size_t capacity;
    size_t used;
    char* memory;
};

BumpAllocator make_bump_allocator(size_t size)
{
    BumpAllocator ba = {};
    ba.memory = (char*)malloc(size);
    if (ba.memory)
    {
        ba.capacity = size;
        memset(ba.memory,0,size); // sets to memory 0
        ba.used=0;
    }
    else
    {
        SM_ASSERT(false,"Failed to allocate memory for BumpAllocator"); 
    }
    return ba;
}

char* bump_alloc(BumpAllocator* ba, size_t size)
{
    char* result = nullptr;
    size_t allignedSize=(size+7) & ~7;
    if (ba->used+allignedSize <= ba->capacity)
    {
        result = ba->memory + ba->used;
        ba->used += allignedSize;
    }
    else
    {
        SM_ASSERT(false,"BumpAllocator IS FULL!");
    }
    return result;
}

// FILE I/O

long long get_timestamp(char* file)
{
    struct stat file_stat = {};
    stat(file,&file_stat);
    return file_stat.st_mtime;
}

bool file_exists(char* filePath)
{
    SM_ASSERT(filePath,"File path is empty");

    auto file = fopen(filePath,"rb");
    if (!file)
    {
        return false;
    }
    fclose(file);
    return true;
}

long get_file_size(char* filePath)
{
    SM_ASSERT(filePath,"File path is empty");
    long fileSize=0;
    auto file = fopen(filePath,"rb");
    if (!file)
    {
        SM_ERROR("Failed to open file %s",filePath);
        return 0;
    }
    fseek(file,0,SEEK_END);
    fileSize = ftell(file);
    fseek(file,0,SEEK_SET); 
    fclose(file);

    return fileSize;
}

char* read_file(char* filePath, int* fileSize, char* buffer)
{
    SM_ASSERT(filePath,"File path is empty.");
    SM_ASSERT(fileSize,"File size isn't supplied.");
    SM_ASSERT(buffer,"No buffer to write.");

    *fileSize=0;
    auto file=fopen(filePath,"rb");
    if (!file)
    {
        SM_ERROR("Failed to open file %s",filePath);
        return nullptr;
    }
    fseek(file,0,SEEK_END);
    *fileSize = ftell(file);
    fseek(file,0,SEEK_SET);

    memset(buffer,0,*fileSize+1);
    fread(buffer,sizeof(char),*fileSize,file);
    fclose(file);

    return buffer;
}
char* read_file(char* filePath, int* fileSize, BumpAllocator* bumpAllocator)
{
    SM_ASSERT(filePath,"File path is empty.");
    SM_ASSERT(fileSize,"File size isn't supplied.");

    char* file=nullptr;
    long fileSize2=get_file_size(filePath);

    if (fileSize2)
    {
        char* buffer = bump_alloc(bumpAllocator,fileSize2+1);
        file=read_file(filePath,fileSize,buffer);
    }
    return file;
}

void write_file(char* filePath, char* buffer, int size)
{
    SM_ASSERT(filePath,"File path is empty.");
    SM_ASSERT(buffer,"No buffer to write.");
    auto file = fopen(filePath,"wb");
    if (!file)
    {
        SM_ERROR("Failed to open file %s",filePath);
        return;
    }
    
    fwrite(buffer,sizeof(char),size,file);
    fclose(file);
}

bool copy_file(char* fileName,char* outputName, char* buffer)
{
    int fileSize=0;
    char* data = read_file(fileName,&fileSize,buffer);
    
    auto outputFile=fopen(outputName,"wb");
    if (!outputFile)
    {
        SM_ERROR("Failed to open file %s",outputName);
        return false;
    }

    int result = fwrite(data,sizeof(char),fileSize,outputFile);
    if (!result)
    {
        SM_ERROR("Failed to write to file %s",outputName);
        return false;
    }

    fclose(outputFile);
    return true;
}

bool copy_file(char* fileName,char* outputName, BumpAllocator* bumpAllocator)
{
    char* file=nullptr;
    long fileSize2 = get_file_size(fileName);

    if (fileSize2)
    {
        char* buffer = bump_alloc(bumpAllocator,fileSize2+1);
        return copy_file(fileName,outputName,buffer);
    }
    return false;
    
}
