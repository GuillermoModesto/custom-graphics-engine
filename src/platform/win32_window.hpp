#pragma once
#include <cstdint>
#include <string>

struct Win32WindowDesc {
    std::wstring title{ L"BlueBrains" };
    uint32_t width{ 1280 };
    uint32_t height{ 720 };
};

class Win32Window {
public:
    bool create(const Win32WindowDesc& d);
    bool pump();                 // process OS messages; return false when app should quit

    void* hwnd() const { return hwnd_; }
    uint32_t width()  const { return width_; }
    uint32_t height() const { return height_; }

private:
    void* hwnd_ = nullptr;  // store as void* to avoid leaking <windows.h> in headers
    uint32_t  width_ = 0;
    uint32_t  height_ = 0;
};

