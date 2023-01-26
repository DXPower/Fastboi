#include "FastboiCore.h"
#include <algorithm>
#include "Angles.h"
#include "Archetype.h"
#include "Application.h"
#include "Camera.h"
#include "Collision.h"
#include "CollisionMask.h"
#include "Gameobject.h"
#include "Input.h"
#include <mutex>
#include "Renderer.h"
#include "Rendering.h"
#include "Resources.h"
#include "SDL/SDL.h"
#include "soloud/soloud.h"
#include "Texture.h"
#include "Timer.h"
#include <unordered_set>
#include <list>
#include <map>

#ifndef DEBUG
extern "C" {
    #include "CacheLineSize.h"
}
#else
    #include "CacheLineSize.h"
#endif

#define WHITE 255, 255, 255, 255
#define BLACK 0, 0, 0, 255
#define RED 255, 0, 0, 255
#define GREEN 0, 255, 0, 255
#define BLUE 0, 0, 255, 255

using namespace Fastboi;
using namespace std::chrono;

bool quit = false;
bool paused = false;

double Fastboi::tickDelta = 0.0;
double Fastboi::physicsDelta = 0.0;

std::size_t cacheLineSize;

std::vector<Gameobject*> gosToDelete; // Need to keep track of the objects we delete

std::map<RenderOrder, std::vector<Renderer*>> renderers;
std::unordered_set<Collider*> colliders;

void Fastboi::Destroy(Gameobject& go) {
    go.Destroy();

    UnregisterGameobject(&go);
}

void Fastboi::Tick() {
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

    for (auto& [k, archetypePtr] : detail::archetypes) {
        ArchetypeBase& archetype = *archetypePtr;
        archetype.TickSystems();
    }

    gosToDelete.clear();
}

void Fastboi::Render() {
    uint32_t flags = SDL_GetWindowFlags(Application::gWindow);
    if (!(flags & (SDL_WINDOW_INPUT_FOCUS))) {
        // continue;
        return;
    }

    auto bgColor = Rendering::GetBGColor();
    SDL_SetRenderDrawColor(Rendering::gRenderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderClear(Rendering::gRenderer);

    const static auto sortByZ = [](const Renderer* a, const Renderer* b) -> bool {
        return a->data.zindex < b->data.zindex;
    };

    for (auto& [order, range] : renderers) {
        for (Renderer* r : range) {
            if (r->isEnabled && r->isStarted && !r->isDeleted) {
                std::sort(range.begin(), range.end(), sortByZ);
                r->Render();
            }
        }        
    }

    // Rendering::Render_AllDebugRects();

    Position mouseWorldPos = Fastboi::GetCamera().ScreenToWorldPos(Input::GetMousePosition());
    Position screenCenterWorldPos = Fastboi::GetCamera().ScreenToWorldPos(Application::GetWindowSize() / 2);
    Gameobject* goUnderMouse = Fastboi::GetGameobjectAtPosition(mouseWorldPos, CollisionLayer::ALL);

    if (goUnderMouse != nullptr) {
        Rendering::SetColor(0, 0, 0, 255);
        Rendering::Render_Rect<Rendering::FillType::UNFILLED>(goUnderMouse->transform);
    }

    auto raytrace = Fastboi::Raytrace(mouseWorldPos, mouseWorldPos - screenCenterWorldPos, CollisionLayer::ALL, 10000);

    Rendering::SetColor(255, 0, 0, 255);
    Rendering::Render_Line(raytrace.origin, raytrace.ending);

    if (raytrace.hit != nullptr) {
        Rendering::SetColor(255, 0, 0, 255);
        Rendering::Render_Rect<Rendering::FillType::UNFILLED>(raytrace.hit->transform);
    }

    SDL_RenderPresent(Rendering::gRenderer);
}

/**
 * \brief Apply velocities, prune potential collisions, detect collisions, 
 * resolve post-collision positions, and dispatch events
**/
void Fastboi::Physics() {
    Collision::PotentialCollisions_t potentialCollisions;
    Collision::Collisions_t collisions;

    // std::lock_guard lock(renderingMtx);

    Collision::ProgressRigidbodies();
    Collision::BroadPhase(colliders, potentialCollisions);
    Collision::NarrowPhase(potentialCollisions, collisions);
    Collision::ResolveColliders(collisions);

    Transform::UpdateAllParentRelations(); // Apply all updates to childrens' positions

    Collision::UpdateAABBTree(colliders);

    Collision::DispatchCollisions(collisions);
}

// Once Fastboi::GameLoop() has ended, clean up all resources.
void Cleanup() {
    gosToDelete.clear();
    renderers.clear();
    colliders.clear();
}

#include <fstream>

void TickPhysicsThread() {
    // Timing constants
    constexpr int TICK_FREQUENCY = 120;
    constexpr std::chrono::duration<double, std::milli> TICK_TIME(1000.0 / TICK_FREQUENCY);
    
    // System timing
    static Timer tickTimer;

    Fastboi::tickDelta = 0;
    tickTimer.Tick(); // Initial tick because the time between app start and this line is quite long

    while (!quit) {
        if (!paused) {
            // Cap tick rate with a blocking if
            // if (tickTimer.TimeSinceLastTick() > TICK_TIME) {
                Fastboi::tickDelta = std::chrono::duration_cast<duration<double>>(tickTimer.TimeSinceLastTick()).count(); // Convert from ms to s
                Fastboi::physicsDelta = Fastboi::tickDelta;

                Input::PollEvents();

                tickTimer.Tick();

                

                Physics();
                Tick();
                Render();

                Fastboi::tickDelta = 0;
                Fastboi::physicsDelta = 0;
        }
    }  
}

/**
 * \brief Starts an infinite loop that calls Input::PollEvents(), Fastboi::Tick(), Fastboi::Physics(), and Fastboi::Render()
 * 
 * Only intended to be called by main
**/
void Fastboi::GameLoop() {
    // cacheLineSize = CacheLineSize();
    cacheLineSize = 64;
    GetCacheSize();

    Resources::GetSoloud().init();

    Input::PollEvents();
    Tick();

    TickPhysicsThread();

    Cleanup();
}

std::size_t Fastboi::GetCacheLineSize() {
    return cacheLineSize;
}

std::size_t Fastboi::GetCacheSize() {
   return 100 * 1024; // Assuming 100kb of L2 cache for now.
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
    std::vector<Renderer*>& range = renderers[r->GetOrder()];
    range.erase(std::find(range.begin(), range.end(), r));
}

void Fastboi::RegisterCollider(Collider* c) {
    colliders.emplace(c);
}

void Fastboi::UnregisterCollider(Collider* c) {
    colliders.erase(c);
}


bool Fastboi::IsRenderingThread() {
    return true;
}

bool Fastboi::IsUpdateThread() {
    return true;
}

/**
 * @example FastboiCore.cpp
 * Shows examples of Instantiating and Destroying gameobjects.
**/