#include "Application.h"
#include "FastboiCore.h"
#include "Resources.h"
#include "Game.h"
#include <iostream>

#include "Spritesheet.h"

int main(int argc, char* args[]) {
    std::cout << "Starting..." << std::endl;

    if (Application::InitApplication()) {
        std::cout << "Application started" << std::endl;
        CS::InitGame();
        Fastboi::GameLoop();
        Fastboi::Resources::Cleanup();
    }

    Application::Cleanup();
}