#pragma once

#include <memory>
#include <mutex>
#include "RenderOrder.h"
#include <type_traits>
#include <thread>


namespace Fastboi {
    struct Collider;
    struct Gameobject;
    struct Renderer;
    
    extern float tickDelta;
    extern float physicsDelta;

    // Instatiates gameobject of type GO with arguments args
    template<class GO, typename... Args>
    GO& Instantiate(Args... args) {
        static_assert(std::is_base_of_v<Gameobject, GO>);

        GO* o = new GO(std::forward<Args>(args)...);
        return *(static_cast<GO*>(RegisterGameobject(o).get()));
    }


    void Destroy(Gameobject& go);

    void Tick();
    void Render();
    void Physics();

    void GameLoop();
    void Quit();

    const std::unique_ptr<Gameobject>& RegisterGameobject(Gameobject* go);
    void UnregisterGameobject(Gameobject* go);

    void RegisterRenderer(Renderer* r);
    void ChangeRenderOrder(Renderer* r, RenderOrder old, RenderOrder _new);
    void UnregisterRenderer(Renderer* r);

    void RegisterCollider(Collider* c);
    void UnregisterCollider(Collider* c);

    std::thread::id GetRenderingThreadID();
    std::thread::id GetUpdateThreadID();

    bool IsRenderingThread();
    bool IsUpdateThread();

    template<typename ...Args>
    void Print(const char* fmt, Args... args) {
        static std::mutex printMutex;

        printMutex.lock();
        printf(fmt, std::forward<Args>(args)...);
        printMutex.unlock();
    }
};
