#pragma once
#include <cstdint>

struct NativeWindow {
    void* hwnd{};
    uint32_t width{}, height{};
};

class IRenderer {
public:
    virtual bool init(const NativeWindow&) = 0;
    virtual void begin_frame() = 0;
    virtual void draw() = 0;   // we'll just Clear() now
    virtual void end_frame() = 0;
    virtual ~IRenderer() = default;
};

