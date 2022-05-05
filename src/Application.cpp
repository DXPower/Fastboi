#include "Application.h"
#include "Input.h"
#include <iostream>
// #include "Room.h"
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"


Application::GRenderer* Application::gRenderer = nullptr;
SDL_Window* Application::gWindow = nullptr;
// bool Application::quit = false;

// const int smallWindowX = 1000 - Adventure::Room::GetTileSize().x / 4;
// const int smallWindowY = Adventure::Room::GetSize().y / Adventure::Room::GetSize().x * smallWindowX - Adventure::Room::GetTileSize().y / 4;

const int smallWindowX = 1600;
const int smallWindowY = 900;

bool isFullscreen = true;

Fastboi::Vec<int> window(smallWindowX, smallWindowY);

bool Application::InitApplication() {
    SDL_Rect rect;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "Failed to start init SDL_VIDEO" << std::endl;
        return false;
    }

    Application::gWindow = SDL_CreateWindow("Death to Vulkan"
                                         , SDL_WINDOWPOS_CENTERED
                                         , SDL_WINDOWPOS_CENTERED
                                         , window.x, window.y
                                         , SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    if (Application::gWindow == nullptr) {
        SDL_Quit();
        std::cout << "Failed to start SDL Window" << std::endl;
        return false;
    }

    Application::gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);

    if (Application::gRenderer == nullptr) {
        std::cout << "Renderer could not be created" << std::endl;
        std::cout << SDL_GetError() << std::endl;
        return false;
    }

    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

    SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
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

void Application::ThrowRuntimeException(const char* err, Exception code, const char* msg) {
    printf("Runtime exception thrown. Code %i: %s %s\n", code, err, msg);
    std::exit(code);
    __builtin_unreachable();
}

void Application::ThrowRuntimeException(const char* err, Exception code) {
    printf("Runtime exception thrown. Code %i: %s\n", code, err);
    std::exit(code);
    __builtin_unreachable();
}

void Application::ThrowRuntimeException(Exception code) {
    printf("Runtime exception thrown. Code %i\n", code);
    std::exit(code);
    __builtin_unreachable();
}