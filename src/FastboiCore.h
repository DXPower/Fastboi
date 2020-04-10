#pragma once

#include "Gameobject.h"
#include "GameobjectAllocator.h"
#include <memory>
#include <mutex>
#include "RenderOrder.h"
#include <type_traits>
#include <thread>
#include <vector>

namespace Fastboi {
    struct Collider;
    struct Renderer;
    
    extern float tickDelta;
    extern float physicsDelta;

    Gameobject& RegisterGameobject(Gameobject* go);
    void UnregisterGameobject(Gameobject* go);
    
    // Instatiates gameobject of type GO with arguments args
    template<auto InitFunc, typename... Args>
    Gameobject& Instantiate(Args&&... args) {
        extern GameobjectAllocator gameobjectAllocator;
        
        Gameobject* allocation = reinterpret_cast<Gameobject*>(gameobjectAllocator.Allocate());
        Gameobject* o = new (allocation) Gameobject();

        InitFunc(*o, std::forward<Args>(args)...);

        return *o;
    }

    template<class T, typename... Args>
    Gameobject& Instantiate(Args&&... args) {
        static_assert(std::is_same_v<T, Gameobject>);

        extern GameobjectAllocator gameobjectAllocator;

        Gameobject* allocation = reinterpret_cast<Gameobject*>(gameobjectAllocator.Allocate());
        Gameobject* o = new (allocation) Gameobject(std::forward<Args>(args)...);

        return *o;
    }

    void Destroy(Gameobject& go);

    void Tick();
    void Render();
    void Physics();

    void GameLoop();

    void Pause();
    void Unpause();
    bool IsPaused();
    void Quit();

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
