#include <windows.h>
#include <iostream>

int main() {
    std::cout << "BlueBrains started!" << std::endl;

    MessageBoxW(nullptr, L"BlueBrains started!", L"BlueBrains", MB_OK);

    return 0;
}

