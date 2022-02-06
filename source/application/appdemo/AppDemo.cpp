#include <Windows.h>
#include <tchar.h> // for _tWinMain (ANSI: WinMain, Unicode: wWinMain)

#include "common/Logger.hpp"

#pragma comment(linker,                                     \
                "/manifestdependency:\""                    \
                "type='win32' "                             \
                "name='Microsoft.Windows.Common-Controls' " \
                "version='6.0.0.0' "                        \
                "processorArchitecture='*' "                \
                "publicKeyToken='6595b64144ccf1df' "        \
                "language='*'\"")

LRESULT CALLBACK WndMsgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void BeginProc(HWND hwnd);
void UpdateProc();
void EndProc();

#ifdef TI_OPT_WINAPP_WINDOW
INT WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PTSTR pCmdLine, INT nCmdShow)
#else
int main(int argc, char* argv[])
#endif
{   LOG_TAG(AppDemo)

    TI_LOG_I(TAG, "TiRenderer Application.");

    #if defined(DEBUG) || defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    TI_LOG_D(TAG, "Enable runtime memory check for debug builds.");
    #endif

    TI_LOG_I(TAG, "Initialize window.");
    MSG msg{};
    HWND hwnd{};
    // GetModuleHandle(NULL) will return HMODULE which we can interpret as a HINSTANCE.
    // If call it from a dll, it will return the instance of the exe that has loaded
    // the dll, not of the dll itself. Here we just want to get a exe instance.
    HINSTANCE hInstance = GetModuleHandle(NULL);
    {
        WNDCLASS wndclass{};
        wndclass.style = CS_HREDRAW | CS_VREDRAW;
        wndclass.lpfnWndProc = WndMsgProc;
        wndclass.cbClsExtra = 0;
        wndclass.cbWndExtra = 0;
        wndclass.hInstance = hInstance;
        wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndclass.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
        wndclass.lpszMenuName = NULL;
        wndclass.lpszClassName = TAG;

        if (!RegisterClass(&wndclass)) {
            TI_LOG_F(TAG, "Register main window class failed, requires at least WindowsNT!");
            MessageBox(NULL, TEXT("Register main window class failed, requires at least WindowsNT!"),
                TEXT("TiRenderer"), MB_ICONERROR | MB_OK);
            return 0;
        }

        hwnd = CreateWindow(TAG, TEXT("TiRenderer"), WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            GetDesktopWindow(), NULL, hInstance, NULL);
        if (!hwnd) {
            TI_LOG_F(TAG, "Create application window failed!");
            MessageBox(NULL, TEXT("Create application window failed!"),
                TEXT("TiRenderer"), MB_ICONERROR | MB_OK);
            return 0;
        }

        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
    }

    TI_LOG_I(TAG, "Program BEGIN running.");
    BeginProc(hwnd);
    {
        // Default window size, this is window size, and is not swapchain size!
        RECT rect{};
        GetWindowRect(hwnd, &rect);
        MoveWindow(hwnd, rect.left, rect.top, 800, 600, TRUE);
        // Message loop to process windows message and draw each frame.
        while (msg.message != WM_QUIT) {
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            } else {
                UpdateProc();
            }
        }
    }
    EndProc();
    TI_LOG_I(TAG, "Program END running.");
    return static_cast<int>(msg.wParam);
}
