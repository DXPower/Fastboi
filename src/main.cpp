#include "Application.h"
#include "FastboiCore.h"
#include "Game.h"
#include <iostream>

#include "Spritesheet.h"

int main(int argc, char* args[]) {
    std::cout << "Starting..." << std::endl;

    Spritesheet<int> spritesheet(nullptr);

    spritesheet.AddAnimation(1, Spritesheet<int>::Animation(Position::zero(), Size(50, 50), Veci(5, 5), 10));

    if (Application::InitApplication()) {
        std::cout << "Application started" << std::endl;
        Slowboi::InitGame();
        // Fastboi::StartGameobjects();
        Fastboi::GameLoop();
        Fastboi::Quit();
    }

    Application::Cleanup();
}