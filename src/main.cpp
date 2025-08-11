#include <iostream>
#include <windows.h>
#include "platform/win32_window.hpp"

#define WIN32_LEAN_AND_MEAN

int main() {
    std::cout << "BlueBrains: creating window...\n";

    Win32Window window;
    if (!window.create({ L"BlueBrains", 1280, 720 })) {
        std::cerr << "Failed to create window\n";
        return -1;
    }

    std::cout << "Window created. Close it to exit.\n";
    while (window.pump()) {
        // (no rendering yet)
        // sleep a tiny bit so this loop doesn't peg a CPU core
        Sleep(1);
    }
    std::cout << "Bye!\n";
    return 0;
}
