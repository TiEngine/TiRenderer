#include <Windows.h>
#include "DemosBaseHeader.h"

namespace {
std::unique_ptr<DemoBase> g_demo;
}

LRESULT WndMsgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

void BeginProc(HWND hwnd)
{
    g_demo = std::make_unique<Demo_01_Backend>();
    //g_demo = std::make_unique<Demo_02_..>();
    g_demo->Begin();
}

void UpdateProc()
{
    g_demo->Update();
}

void EndProc()
{
    g_demo->Finish();
    g_demo.reset();
}
