#include "Application.h"
#include "FastboiCore.h"
#include "Resources.h"
#include "Game.h"
#include <iostream>

int main() {
    std::cout << "Starting..." << std::endl;

    if (Application::InitApplication()) {
        std::cout << "Application started" << std::endl;
        Minecraft::InitGame();
        Fastboi::GameLoop();
        Fastboi::Resources::Cleanup();
    }

    Application::Cleanup();
}