#include "WindowManager.h"
#include<d3d12.h>
#include<tchar.h>

#pragma comment(lib,"d3d12.lib")

//LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
//    if (msg == WM_DESTROY) {
//        PostQuitMessage(0);
//        return 0;
//    }
//    return DefWindowProc(hwnd, msg, wparam, lparam);
//}


WindowManager::WindowManager(const unsigned int window_width, const unsigned int window_height)
{
    wndClass.cbSize = sizeof(WNDCLASSEX);
    //wndClass.lpfnWndProc = (WNDPROC)WindowProcedure;
    wndClass.lpszClassName = _T("DirectXTest");
    wndClass.hInstance = GetModuleHandle(0);
    RegisterClassEx(&wndClass);
    RECT wrc = { 0,0, window_width, window_height };
    AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);
    //ウィンドウオブジェクトの生成
    hwnd = CreateWindow(wndClass.lpszClassName,
        _T("ウィンドウ"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        wrc.right - wrc.left,
        wrc.bottom - wrc.top,
        nullptr,
        nullptr,
        wndClass.hInstance,
        nullptr);
}

WindowManager::~WindowManager()
{
}
