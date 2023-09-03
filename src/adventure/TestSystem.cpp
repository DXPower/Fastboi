#include "TestSystem.h"
#include "Item.h"
#include <iostream>
#include <atomic>
#include <chrono>
#include <ratio>
using namespace Fastboi;

void Fastboi::TestPhysicsSystem(Gameobject& go, Collider& col [[maybe_unused]]) {
    if (go.name != "") {
        std::cout << go.name;
    }
}

static void Timer(std::atomic<float>& t) {
    namespace ch = std::chrono;
    using namespace std::chrono_literals;

    auto start = ch::steady_clock::now();
    auto end = start;

    while (true) {
        auto dur = ch::duration<float, std::deca>(end - start);
        t += dur.count();

        std::this_thread::sleep_for(10ms);
        end = ch::steady_clock::now();
    }
}

void Adventure::TestKeySystem(Gameobject& go [[maybe_unused]], Key& key [[maybe_unused]], Item& item, Transform& transform) {
    // static std::atomic<float> t = 0;
    // static std::thread thr(Timer, std::ref(t));
    
    // std::cout << t << "\n";

    // if (!item.isHeld)
    //     transform;
}
