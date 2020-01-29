#include "FastboiCore.h"
#include <algorithm>
#include "Application.h"
#include "Camera.h"
#include "Collision.h"
#include "Gameobject.h"
#include "Input.h"
#include "Renderer.h"
#include "Rendering.h"
#include "Resources.h"
#include "SDL/SDL.h"
#include "Timer.h"
#include <thread>
#include <unordered_set>
#include <list>
#include <map>

#define WHITE 255, 255, 255, 255
#define BLACK 0, 0, 0, 255
#define RED 255, 0, 0, 255
#define GREEN 0, 255, 0, 255
#define BLUE 0, 0, 255, 255

using namespace Fastboi;

bool quit = false;

float Fastboi::tickDelta = 0.0f;
float Fastboi::physicsDelta = 0.0f;

std::vector<std::unique_ptr<Gameobject>> gameobjects;
std::vector<std::unique_ptr<Gameobject>> gosToAdd;
std::vector<Gameobject*> gosToDelete; // When an object is deleted, gameobjects still owns the pointer. Use non-owning ptr here.

std::map<RenderOrder, std::vector<Renderer*>> renderers;
std::unordered_set<Collider*> colliders;

void Fastboi::Destroy(Gameobject& go) {
    go.Destroy();

    UnregisterGameobject(&go);
}

void Fastboi::Tick() {
    for (std::unique_ptr<Gameobject>& go : gosToAdd) {
        go->Start();
        gameobjects.push_back(std::move(go));
    }

    gosToAdd.clear();

    for (const std::unique_ptr<Gameobject>& go : gameobjects) {
        go->Update();
    }

    for (const Gameobject* go : gosToDelete) {
        gameobjects.erase(std::remove_if(gameobjects.begin(), gameobjects.end(), [&](const std::unique_ptr<Gameobject>& u) {
            return go == u.get();
        }));
    }

    gosToDelete.clear();
}

void Fastboi::Render() {
    Timer renderTimer;
    constexpr int TICK_FREQUENCY = 120;
    constexpr float TICK_TIME = 1.0f / TICK_FREQUENCY;
    
    float renderDelta = 0;

    while (!quit) {
        Input::PollEvents();
        renderTimer.tick();
        renderDelta += renderTimer.elapsed_seconds;

        if (!std::isgreater(renderDelta, TICK_TIME)) continue;

        renderDelta = -TICK_TIME;

        // printf("Attempting render...\n");
        uint32_t flags = SDL_GetWindowFlags(Application::gWindow);
        if (!(flags & (SDL_WINDOW_INPUT_FOCUS))) {
            continue;
        }

        SDL_SetRenderDrawColor(Rendering::gRenderer, WHITE);
        SDL_RenderClear(Rendering::gRenderer);

        const static auto sortByZ = [](const Renderer* a, const Renderer* b) -> bool {
            return a->data.zindex < b->data.zindex;
        };

        for (auto& [order, range] : renderers) {
            for (Renderer* r : range) {
                if (r->isStarted && !r->isDeleted) {
                    // printf("Rendering %s\n", r->gameobject.name);
                    std::sort(range.begin(), range.end(), sortByZ);
                    r->Render();
                }
            }        
        }

        SDL_RenderPresent(Rendering::gRenderer);
    }
}

/**
 * \brief Apply velocities, prune potential collisions, detect collisions, 
 * resolve post-collision positions, and dispatch events
**/
void Fastboi::Physics() {
    Collision::PotentialCollisions_t potentialCollisions;
    Collision::Collisions_t collisions;

    Collision::ApplyVelocities(gameobjects);
    Collision::BroadPhase(colliders, potentialCollisions);
    Collision::NarrowPhase(potentialCollisions, collisions);
    Collision::ResolveColliders(colliders, collisions);
    Collision::DispatchCollisions(collisions);
}

// Once Fastboi::GameLoop() has ended, clean up all resources.
void Cleanup() {
    gameobjects.clear();
    gosToAdd.clear();
    gosToDelete.clear();
    renderers.clear();
    colliders.clear();
}

void TickPhysicsThread() {
    // Timing constants
    constexpr int TICK_FREQUENCY = 120;
    constexpr float TICK_TIME = 1.0f / TICK_FREQUENCY;
    
    constexpr int PHYSICS_FREQUENCY = 120;
    constexpr float PHYSICS_TIME = 1.0f / PHYSICS_FREQUENCY;
    
    // System timing
    static Timer tickTimer;
    static Timer physicsTimer;
    SDL_Event event;

    Fastboi::tickDelta = -TICK_TIME;
    Fastboi::tickDelta = -PHYSICS_TIME;

    tickTimer.tick(); // Initial tick because the time between SDL start and Fastboi::GameLoop is quite long
    physicsTimer.tick();

    printf("Entering tick/physics loop\n");

    while (!quit) {
        tickTimer.tick();
        physicsTimer.tick();

        Fastboi::tickDelta += tickTimer.elapsed_seconds;
        Fastboi::physicsDelta += physicsTimer.elapsed_seconds;

        // Cap tick rate with a blocking if
        if (std::isgreater(Fastboi::tickDelta, TICK_TIME)) {
            Tick();

            Fastboi::tickDelta = -TICK_TIME; // Reset the framerate clock
        }

        if (std::isgreater(Fastboi::physicsDelta, PHYSICS_TIME)) {
            Physics();

            Fastboi::physicsDelta = -PHYSICS_TIME;
        }  
    }  
}

/**
 * \brief Starts an infinite loop that calls Input::PollEvents(), Fastboi::Tick(), Fastboi::Physics(), and Fastboi::Render()
 * 
 * Only intended to be called by main
**/
void Fastboi::GameLoop() {
    printf("Ticking...\n");
    Input::PollEvents();
    Tick();

    printf("Ticked!\n");
    std::thread bgThread(TickPhysicsThread);

    Render();
    bgThread.join();

    printf("Cleaning up!\n");
    Cleanup();
}

/**
 * \brief Quits Fastboi by ending Fastboi::GameLoop() before the beginning of the next loop.
 * <b>Deletes all Gameobjects managed by Fastboi</b>.
*/
void Fastboi::Quit() {
    quit = true;
}

// Registers a Gameobject to be managed by Fastboi. Use Destroy() to delete.
const std::unique_ptr<Gameobject>& Fastboi::RegisterGameobject(Gameobject* go) {
    printf("New object!");
    gosToAdd.push_back(std::move(std::unique_ptr<Gameobject>(go)));

    return gosToAdd.back();
}

void Fastboi::UnregisterGameobject(Gameobject* go) {
    printf("Dead object!");
    gosToDelete.push_back(go);
}

void Fastboi::RegisterRenderer(Renderer* r) {
    printf("New renderer!");
    
    RenderOrder order = r->GetOrder();
    renderers[order].push_back(r);
}

//! Helper function used by Renderer::SetOrder(). This should not be directly called.
void Fastboi::ChangeRenderOrder(Renderer* r, RenderOrder old, RenderOrder _new) {
    // Add renderer to new layer before removing to avoid it disappearing for a frame
    renderers[_new].push_back(r);
    
    std::vector<Renderer*>& range = renderers[old];
    range.erase(std::find(range.begin(), range.end(), r));
}

void Fastboi::UnregisterRenderer(Renderer* r) {
    printf("Dead renderer!");
    std::vector<Renderer*>& range = renderers[r->GetOrder()];
    range.erase(std::find(range.begin(), range.end(), r));
}

void Fastboi::RegisterCollider(Collider* c) {
    printf("New collider! %p\n", c);
    colliders.emplace(c);
}

void Fastboi::UnregisterCollider(Collider* c) {
    printf("Dead collider!");
    colliders.erase(c);
}

/**
 * @example FastboiCore.cpp
 * Shows examples of Instantiating and Destroying gameobjects.
**/