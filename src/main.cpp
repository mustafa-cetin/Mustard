#include "hexa_lib.h"

// PLATFORM GLOBALS
static bool running=true;


// PLATFORM FUNCTIONS
bool platform_create_window(int width, int height, char* title);
void platform_update_window();

// Windows PLATFORM
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

// Windows GLOBALS
static HWND window;
// PLATFORM Implementations
LRESULT CALLBACK windows_window_callback(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    switch (msg)
    {
        case WM_CLOSE:
        case WM_DESTROY:
        {
            running = false;
        } break;
        default:
        {
            result = DefWindowProcA(window,msg,wParam,lParam);
        } break;
    }
    return result;
}

bool platform_create_window(int width, int height, char* title)
{
    HINSTANCE instance = GetModuleHandleA(0);

    WNDCLASSA wc = {};
    wc.hInstance = instance;
    wc.hIcon = LoadIcon(instance,IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL,IDC_ARROW);
    wc.lpszClassName = title; // not title unique identifer
    wc.lpfnWndProc = windows_window_callback; // callback for input into the window
    if (!RegisterClassA(&wc))
    {
        return false;
    }
    int dwStyle = WS_OVERLAPPEDWINDOW;

    window = CreateWindowExA(0,title,
    title,
    dwStyle,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    width,
    height,
    NULL,
    NULL,
    instance,
    NULL);

    if (window==NULL)
    {
        return false;
    }
    ShowWindow(window,SW_SHOW);
    return true;
}

void platform_update_window()
{
    MSG message;
    while (PeekMessageA(&message,window,0,0,PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
}
#endif

int main()
{
    platform_create_window(1200,720,"HEXAPAWA");
    while(running)
    {
    
        // Game loop
        platform_update_window();

    }

    return 0;
}