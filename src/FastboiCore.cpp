#include "FastboiCore.h"
#include <algorithm>
#include "Application.h"
#include "Camera.h"
#include "Collision.h"
#include "Gameobject.h"
#include "Input.h"
#include <mutex>
#include "Renderer.h"
#include "Rendering.h"
#include "Resources.h"
#include "SDL/SDL.h"
#include "Texture.h"
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
bool paused = false;

float Fastboi::tickDelta = 0.0f;
float Fastboi::physicsDelta = 0.0f;

std::vector<Gameobject*> gosToDelete; // Need to keep track of the objects we delete

std::map<RenderOrder, std::vector<Renderer*>> renderers;
std::unordered_set<Collider*> colliders;

std::thread::id renderingThreadID;
std::thread::id updateThreadID;

std::mutex renderingMtx;

void Fastboi::Destroy(Gameobject& go) {
    go.Destroy();

    UnregisterGameobject(&go);
}

void Fastboi::Tick() {
    std::lock_guard<std::mutex> lock(renderingMtx);

    extern GameobjectAllocator gameobjectAllocator;
    gameobjectAllocator.StartAll();

    for (auto it = gameobjectAllocator.GO_Begin(); it != gameobjectAllocator.GO_End(); it++) {
        Gameobject& go = *it;

        if (!go.isStarted) continue;

        go.Update();
    }

    for (Gameobject* go : gosToDelete) {
        go->~Gameobject();
        gameobjectAllocator.Deallocate(static_cast<void*>(go));
    }

    gosToDelete.clear();
}

void Fastboi::Render() {
    Timer renderTimer;
    constexpr int TICK_FREQUENCY = 120;
    constexpr float TICK_TIME = 1.0f / TICK_FREQUENCY;
    
    float renderDelta = 0;

    while (!quit) {
        renderingMtx.lock();
        Input::PollEvents();
        renderingMtx.unlock();

        Texture::CreateQueuedTextures();

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

        renderingMtx.lock();

        const static auto sortByZ = [](const Renderer* a, const Renderer* b) -> bool {
            return a->data.zindex < b->data.zindex;
        };

        for (auto& [order, range] : renderers) {
            for (Renderer* r : range) {
                if (r->isStarted && !r->isDeleted) {
                    std::sort(range.begin(), range.end(), sortByZ);
                    r->Render();
                }
            }        
        }

        renderingMtx.unlock();
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

    std::lock_guard lock(renderingMtx);

    Collision::ApplyVelocities();
    Collision::BroadPhase(colliders, potentialCollisions);
    Collision::NarrowPhase(potentialCollisions, collisions);
    Collision::ResolveColliders(colliders, collisions);
    Collision::DispatchCollisions(collisions);

}

// Once Fastboi::GameLoop() has ended, clean up all resources.
void Cleanup() {
    gosToDelete.clear();
    renderers.clear();
    colliders.clear();
}

void TickPhysicsThread() {
    // Timing constants
    constexpr int TICK_FREQUENCY = 120;
    constexpr float TICK_TIME = 1.0f / TICK_FREQUENCY;
    
    // System timing
    static Timer tickTimer;

    Fastboi::tickDelta = -TICK_TIME;

    tickTimer.tick(); // Initial tick because the time between app start and this line is quite long

    while (!quit) {
        tickTimer.tick();

        if (!paused) {
            Fastboi::tickDelta += tickTimer.elapsed_seconds;
            Fastboi::physicsDelta += tickTimer.elapsed_seconds;

            // Cap tick rate with a blocking if
            if (std::isgreater(Fastboi::tickDelta, TICK_TIME)) {
                Physics();
                Tick();

                Fastboi::tickDelta = -TICK_TIME;
                Fastboi::physicsDelta = -TICK_TIME;
            }
        }
    }  
}

/**
 * \brief Starts an infinite loop that calls Input::PollEvents(), Fastboi::Tick(), Fastboi::Physics(), and Fastboi::Render()
 * 
 * Only intended to be called by main
**/
void Fastboi::GameLoop() {
    Input::PollEvents();
    Tick();

    std::thread bgThread(TickPhysicsThread);

    updateThreadID = bgThread.get_id();
    renderingThreadID = std::this_thread::get_id();

    Render();
    bgThread.join();

    Cleanup();
}

/**
 * @brief Pauses the update loop of Fastboi. This halts all Instantiation, Destruction, Start(), Update(),
 * and physics calculations of gameobjects and colliders. * 
 */
void Fastboi::Pause() {
    paused = true;
}

/**
 * @brief Resumes the update loop of Fastboi.
 */
void Fastboi::Unpause() {
    paused = false;
}

/**
 * @brief Gets the current paused state of Fastboi.
 */
bool Fastboi::IsPaused() {
    return paused;
}

/**
 * \brief Quits Fastboi by ending Fastboi::GameLoop() before the beginning of the next loop.
 * <b>Deletes all Gameobjects managed by Fastboi</b>.
*/
void Fastboi::Quit() {
    quit = true;
}

// // Registers a Gameobject to be managed by Fastboi. Use Destroy() to delete.
// Gameobject& Fastboi::RegisterGameobject(Gameobject* go) {
//     // Print("New object!");
//     // gosToAdd.emplace_back(go);

//     gameobjects.push_back(go);

//     return *gameobjects.back();
// }

void Fastboi::UnregisterGameobject(Gameobject* go) {
    // Print("Dead object!");
    gosToDelete.push_back(go);
}

void Fastboi::RegisterRenderer(Renderer* r) {
    // Print("New renderer!");
    
    RenderOrder order = r->GetOrder();
    renderers[order].push_back(r);
    // Print("Renderer name: %s\n", r->gameobject.name);
}

//! Helper function used by Renderer::SetOrder(). This should not be directly called.
void Fastboi::ChangeRenderOrder(Renderer* r, RenderOrder old, RenderOrder _new) {
    // Add renderer to new layer before removing to avoid it disappearing for a frame
    renderers[_new].push_back(r);
    
    std::vector<Renderer*>& range = renderers[old];
    range.erase(std::find(range.begin(), range.end(), r));
}

void Fastboi::UnregisterRenderer(Renderer* r) {
    // Print("Dead renderer!");
    std::vector<Renderer*>& range = renderers[r->GetOrder()];
    range.erase(std::find(range.begin(), range.end(), r));
}

void Fastboi::RegisterCollider(Collider* c) {
    // Print("New collider! %p\n", c);
    colliders.emplace(c);
}

void Fastboi::UnregisterCollider(Collider* c) {
    // Print("Dead collider!");
    colliders.erase(c);
}

std::thread::id Fastboi::GetRenderingThreadID() {
    return renderingThreadID;
}

std::thread::id Fastboi::GetUpdateThreadID() {
    return updateThreadID;
}

bool Fastboi::IsRenderingThread() {
    return std::this_thread::get_id() == GetRenderingThreadID();
}

bool Fastboi::IsUpdateThread() {
    return std::this_thread::get_id() == GetUpdateThreadID();
}

/**
 * @example FastboiCore.cpp
 * Shows examples of Instantiating and Destroying gameobjects.
**/