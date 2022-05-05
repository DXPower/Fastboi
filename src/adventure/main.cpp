#include "Application.h"
#include "FastboiCore.h"
#include "Resources.h"
#include "Game.h"
#include <iostream>

#include "Spritesheet.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char* args[]) {
    std::cout << "Starting..." << std::endl;

    if (Application::InitApplication()) {
        std::cout << "Application started" << std::endl;
        Adventure::InitGame();
        Fastboi::GameLoop();
        Fastboi::Resources::Cleanup();
    }

    Application::Cleanup();

    return 0;
}