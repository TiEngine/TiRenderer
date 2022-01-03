#include <windows.h>
#include <crtdbg.h>
#include <tchar.h>

INT WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PTSTR pCmdLine, INT nCmdShow)
{
    #if defined(DEBUG) || defined(_DEBUG)
    // Enable runtime memory check for debug builds.
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    #endif

    return 0;
}
