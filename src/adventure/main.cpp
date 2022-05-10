#include "Application.h"
#include "FastboiCore.h"
#include "Resources.h"
#include "Game.h"
#include <iostream>

#include "AABBTree.h"
#include "Shape.h"

using namespace Fastboi;
using namespace Collision;

int main([[maybe_unused]] int argc, [[maybe_unused]] char* args[]) {
    std::cout << "Starting..." << std::endl;

    // AABBTree tree(0.f);

    // // BoundingBox a{Position(0, 0), Position(10, 10)};
    // // BoundingBox b{Position(100, 100), Position(110, 110)};
    // // BoundingBox c{Position(20, 20), Position(30, 30)};
    // // BoundingBox d{Position(-10, -10), Position(0, 0)};
    // // BoundingBox e{Position(30, 30), Position(40, 40)};
    // // BoundingBox f{Position(110, 110), Position(120, 120)};
    // // BoundingBox g{Position(-20, -20), Position(-10, -10)};

    // // tree.InsertLeaf(a);
    // // tree.InsertLeaf(b);
    // // tree.InsertLeaf(c);
    // // tree.InsertLeaf(d);
    // // tree.InsertLeaf(e);
    // // tree.InsertLeaf(f);
    // // tree.InsertLeaf(g);

    // BoundingBox a{Position(0, 0),   Position(10, 10) };
    // BoundingBox b{Position(10, 10), Position(20, 20) };
    // BoundingBox c{Position(20, 20), Position(30, 30) };
    // BoundingBox d{Position(30, 30), Position(40, 40) };
    // BoundingBox e{Position(40, 40), Position(50, 50) };

    // auto handlea = tree.InsertLeaf(a);
    // auto handleb = tree.InsertLeaf(b);
    // auto handlec = tree.InsertLeaf(c);
    // auto handled = tree.InsertLeaf(d);
    // auto handlee = tree.InsertLeaf(e);

    // // handlea.Reset();
    // // handleb.Reset();
    // // handlec.Reset();
    // // handled.Reset();
    // handlee.Reset();
    
    if (Application::InitApplication()) {
        std::cout << "Application started" << std::endl;
        Adventure::InitGame();
        Fastboi::GameLoop();
        Fastboi::Resources::Cleanup();
    }

    Application::Cleanup();

    return 0;
}