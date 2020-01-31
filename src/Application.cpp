#include "Application.h"
#include "Input.h"
#include <iostream>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"

Application::GRenderer* Application::gRenderer = nullptr;
SDL_Window* Application::gWindow = nullptr;
SDL_Surface* Application::windowSurface = nullptr;
// bool Application::quit = false;

const int smallWindowX = 900;
const int smallWindowY = 800;

bool isFullscreen = true;

Fastboi::Vec<int> window(1920, 1080);

bool Application::InitApplication() {
    SDL_Rect rect;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "Failed to start init SDL_VIDEO" << std::endl;
        return false;
    }

    Application::gWindow = SDL_CreateWindow("Slowboi"
                                         , SDL_WINDOWPOS_CENTERED
                                         , SDL_WINDOWPOS_CENTERED
                                         , window.x, window.y
                                         , SDL_WINDOW_FULLSCREEN | SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    if (Application::gWindow == nullptr) {
        SDL_Quit();
        std::cout << "Failed to start SDL Window" << std::endl;
        return false;
    }

    Application::windowSurface = SDL_GetWindowSurface(gWindow);
    Application::gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);

    if (Application::gRenderer == nullptr) {
        std::cout << "Renderer could not be created" << std::endl;
        return false;
    }

    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

    SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
    SDL_RenderClear(gRenderer);

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cout << "Could not initialize PNG" << std::endl;
        return false;
    }

    std::cout << "Application started" << std::endl;

    return true;
}

void Application::Cleanup() {
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    SDL_Quit();
}

const Fastboi::Vec<int>& Application::GetWindowSize() {
    return window;
}

void Application::WindowSizeChanged(const Fastboi::Vec<int>& newSize) {
    window = newSize;

    Fastboi::WindowResizeEvent e { newSize };
    Fastboi::Input::resizeSignal.fire(e);
    printf("Window size changed! %i %i\n", window.x, window.y);
}

void Application::ToggleFullscreen() {
    printf("Fullscreen toggle! Currently: %i\n", isFullscreen);

    if (!isFullscreen) {
        SDL_SetWindowResizable(gWindow, SDL_FALSE);
        SDL_SetWindowFullscreen(Application::gWindow, SDL_WINDOW_FULLSCREEN);

        SDL_DisplayMode dm;
        SDL_GetDisplayMode(SDL_GetWindowDisplayIndex(Application::gWindow), 0, &dm);
        SDL_SetWindowDisplayMode(Application::gWindow, &dm);

        WindowSizeChanged(Fastboi::Vec<int>(dm.w, dm.h));
    } else {
        SDL_SetWindowFullscreen(Application::gWindow, 0);
        SDL_SetWindowResizable(gWindow, SDL_TRUE);
        SDL_SetWindowSize(gWindow, smallWindowX, smallWindowY);
        SDL_SetWindowPosition(gWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

        WindowSizeChanged(Fastboi::Vec<int>(smallWindowX, smallWindowY));
    }

    isFullscreen = !isFullscreen;
}

bool Application::IsFullscreen() {
    return isFullscreen;
}

void Application::ThrowRuntimeException(const char* err, Exception code) {
    printf("Runtime exception thrown. Code %i: %s\n", code, err);
    std::exit(code);
}

void Application::ThrowRuntimeException(Exception code) {
    printf("Runtime exception thrown. Code %i\n", code);
    std::exit(code);
}