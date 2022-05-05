#pragma once

#include "Vec.h"
#include "Exceptions.h"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Surface;
struct SDL_WindowEvent;

namespace Application {
    using GRenderer = SDL_Renderer;

    extern SDL_Window* gWindow;
    extern GRenderer* gRenderer;

    bool InitApplication();
    void Cleanup();

    void ToggleFullscreen();
    bool IsFullscreen();

    const Fastboi::Vec<int>& GetWindowSize();
    void WindowSizeChanged(const Fastboi::Vec<int>& newSize);

    void ThrowRuntimeException(const char* err, Exception code, const char* msg) __attribute__((noreturn));
    void ThrowRuntimeException(const char* err, Exception code) __attribute__((noreturn));
    void ThrowRuntimeException(Exception code) __attribute__((noreturn));
}