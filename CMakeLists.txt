# ──────────────────────────────────────────────────────────────
# 1. Minimum CMake version & project definition
# ──────────────────────────────────────────────────────────────

cmake_minimum_required(VERSION 3.25)  # Require CMake 3.25+ (new features, avoids legacy mode)

project(BlueBrains LANGUAGES CXX)     # Project name + languages used (C++ only here)

# ──────────────────────────────────────────────────────────────
# 2. Global C++ settings
# ──────────────────────────────────────────────────────────────

set(CMAKE_CXX_STANDARD 20)            # Request C++20 standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)   # Fail if compiler can't do C++20 (no silent downgrade)

# ──────────────────────────────────────────────────────────────
# 3. Create the executable target
# ──────────────────────────────────────────────────────────────

# Define an executable target named 'BlueBrains' built from these source files.
# This is our "main program" target — all build rules are attached to it.
add_executable(BlueBrains # WIN32 if it's a GUI app (no console window on Windows)
  src/main.cpp
  src/platform/win32_window.cpp
  src/renderer/d3d12_renderer.cpp
)

# ──────────────────────────────────────────────────────────────
# 4. Target include paths
# ──────────────────────────────────────────────────────────────

# Add 'src' to the compiler's header search paths for this target only.
# PRIVATE = only this target sees it; won't propagate to dependent targets.
target_include_directories(BlueBrains PRIVATE src)

# ──────────────────────────────────────────────────────────────
# 5. Compiler options and definitions (MSVC-specific)
# ──────────────────────────────────────────────────────────────

if(MSVC)
  # Add MSVC compiler flags:
  #   /W4        = High warning level (more warnings)
  #   /permissive- = Strict C++ standard conformance (reduces MSVC extensions)
  #   /EHsc      = Standard C++ exception handling model
  target_compile_options(BlueBrains PRIVATE /W4 /permissive- /EHsc)

  # Preprocessor definitions:
  #   WIN32_LEAN_AND_MEAN = Exclude rarely used parts of <windows.h>
  #   NOMINMAX            = Prevent <windows.h> from defining min/max macros
  target_compile_definitions(BlueBrains PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)

  # Link this target against Windows system libraries required for Direct3D 12.
  target_link_libraries(BlueBrains PRIVATE
    d3d12   # Direct3D 12 core API
    dxgi    # DXGI (swap chains, adapter enumeration)
    dxguid  # GUID/IID definitions for DirectX COM interfaces
  )
endif()

