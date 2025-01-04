#pragma once

#include <stdio.h>

// defines
#ifdef _WIN32
#define DEBUG_BREAK() __debugbreak()
#elif __linux__
#define DEBUG_BREAK() __builtin_debugtrap()
#elif __APPLE__
#define DEBUG_BREAK() __builtin_trap()
#endif

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