#pragma once

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Surface;

namespace Application {
    using GRenderer = SDL_Renderer;

    extern SDL_Window* window;
    extern SDL_Surface* windowSurface;
    extern GRenderer* gRenderer;
    extern bool quit;

    bool InitApplication();
    void Cleanup();

    void GetWindowSize(int* w, int* h);
}