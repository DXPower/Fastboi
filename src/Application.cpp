#include "Application.h"
#include <iostream>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"

Application::GRenderer* Application::gRenderer = nullptr;
SDL_Window* Application::window = nullptr;
SDL_Surface* Application::windowSurface = nullptr;
bool Application::quit = false;

int windowX = 900;
int windowY = 800;

bool Application::InitApplication() {
    SDL_Rect rect;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "Failed to start init SDL_VIDEO" << std::endl;
        return false;
    }

    Application::window = SDL_CreateWindow("Slowboi"
                                         , SDL_WINDOWPOS_UNDEFINED
                                         , SDL_WINDOWPOS_UNDEFINED
                                         , 900, 800
                                         , SDL_WINDOW_SHOWN);

    if (Application::window == nullptr) {
        SDL_Quit();
        std::cout << "Failed to start SDL Window" << std::endl;
        return false;
    }

    Application::windowSurface = SDL_GetWindowSurface(window);
    Application::gRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (Application::gRenderer == nullptr) {
        std::cout << "Renderer could not be created" << std::endl;
        return false;
    }

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
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Application::GetWindowSize(int* w, int* h) {
    *w = windowX;
    *h = windowY;
}