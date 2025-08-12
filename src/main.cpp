#include <windows.h>
#include <iostream>
#include "platform/win32_window.hpp"
#include "renderer/d3d12_renderer.hpp"

int main() {
    Win32Window window;
    if (!window.create({ L"BlueBrains", 1280, 720 })) return -1;

    D3D12Renderer renderer;
    if (!renderer.init({ window.hwnd(), window.width(), window.height() })) return -2;

    while (window.pump()) {
        renderer.begin_frame();
        renderer.draw();
        renderer.end_frame();
        Sleep(1);
    }
    return 0;
}
