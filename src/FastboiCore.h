#pragma once

#include <memory>
#include "RenderOrder.h"
#include <type_traits>
#include <thread>


namespace Fastboi {
    struct Collider;
    struct Gameobject;
    struct Renderer;
    
    extern float tickDelta;
    extern float physicsDelta;

    // extern std::thread renderingThread;
    // extern std::thread tickPhysicsThread;

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
};
