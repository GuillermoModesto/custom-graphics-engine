#include "win32_window.hpp"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
}

bool Win32Window::create(const Win32WindowDesc& d) {
    HINSTANCE hInst = GetModuleHandle(nullptr);

    WNDCLASS wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = L"BB_WndClass";
    RegisterClass(&wc);

    RECT r{ 0, 0, (LONG)d.width, (LONG)d.height };
    AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, FALSE);

    HWND h = CreateWindow(
        wc.lpszClassName, d.title.c_str(),
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        r.right - r.left, r.bottom - r.top,
        nullptr, nullptr, hInst, nullptr
    );
    if (!h) return false;

    hwnd_ = h;
    width_ = d.width;
    height_ = d.height;
    return true;
}

bool Win32Window::pump() {
    MSG msg{};
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) return false;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return true;
}

