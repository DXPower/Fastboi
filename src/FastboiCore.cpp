#include "FastboiCore.h"
#include "AABB.h"
#include <algorithm>
#include "Angles.h"
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
#include "soloud/soloud.h"
#include "Texture.h"
#include "Timer.h"
#include <thread>
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

std::thread::id renderingThreadID;
std::thread::id updateThreadID;

std::mutex renderingMtx;

void Fastboi::Destroy(Gameobject& go) {
    go.Destroy();

    UnregisterGameobject(&go);
}

void Fastboi::Tick() {
    extern GameobjectAllocator gameobjectAllocator;
    std::lock_guard<std::mutex> lock(renderingMtx);
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
    // Timer renderTimer;
    // constexpr int TICK_FREQUENCY = 120;
    // constexpr std::chrono::duration<double, std::milli> TICK_TIME(1000.0 / TICK_FREQUENCY);
    
    // std::chrono::duration<double, std::milli> renderDelta(0);

    // while (!quit) {
        // renderingMtx.lock();
        // Input::PollEvents();
        // renderingMtx.unlock();

        Texture::CreateQueuedTextures();

        // if (!(renderTimer.TimeSinceLastTick() > TICK_TIME)) continue;

        // renderDelta = renderTimer.elapsed_seconds;
        // renderTimer.Tick();

        // if (renderDelta > (decltype(renderDelta){10}))
        //     printf("Render: %f\n", renderDelta);

        // // printf("Attempting render...\n");
        uint32_t flags = SDL_GetWindowFlags(Application::gWindow);
        if (!(flags & (SDL_WINDOW_INPUT_FOCUS))) {
            // continue;
            return;
        }

        auto bgColor = Rendering::GetBGColor();
        SDL_SetRenderDrawColor(Rendering::gRenderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        SDL_RenderClear(Rendering::gRenderer);

        // renderingMtx.lock();

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

        AABBTree::AABB::RenderAllAABBs();
        Rendering::Render_AllDebugRects();

        // renderingMtx.unlock();
        SDL_RenderPresent(Rendering::gRenderer);
        // renderDelta = renderDelta.zero();
    // }
}

/**
 * \brief Apply velocities, prune potential collisions, detect collisions, 
 * resolve post-collision positions, and dispatch events
**/
void Fastboi::Physics() {
    Collision::PotentialCollisions_t potentialCollisions;
    Collision::Collisions_t collisions;

    std::lock_guard lock(renderingMtx);

    Collision::ProgressRigidbodies();
    Collision::BroadPhase(colliders, potentialCollisions);
    Collision::NarrowPhase(potentialCollisions, collisions);
    Collision::ResolveColliders(colliders, collisions);

    Transform::UpdateAllParentRelations(); // Apply all updates to childrens' positions

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

    std::vector<double> frameTimes;
    std::vector<double> physicsTimes;
    std::vector<double> tickTimes;
    std::vector<double> renderTimes;
    unsigned int n = 10000;

    frameTimes.reserve(n);
    frameTimes.reserve(n);
    frameTimes.reserve(n);
    frameTimes.reserve(n);

    while (!quit) {
        if (!paused) {
            // Cap tick rate with a blocking if
            // if (tickTimer.TimeSinceLastTick() > TICK_TIME) {
                Fastboi::tickDelta = std::chrono::duration_cast<duration<double>>(tickTimer.TimeSinceLastTick()).count(); // Convert from ms to s
                Fastboi::physicsDelta = Fastboi::tickDelta;

                Input::PollEvents();

                tickTimer.Tick();

                

                Physics();
                // physicsTimes.push_back(tickTimer.TimeSinceLastTick().count());
                Tick();
                // tickTimes.push_back(tickTimer.TimeSinceLastTick().count());
                Render();
                // renderTimes.push_back(tickTimer.TimeSinceLastTick().count());

                // frameTimes.push_back(tickTimer.TimeSinceLastTick().count());

                Fastboi::tickDelta = 0;
                Fastboi::physicsDelta = 0;

                // if (frameTimes.size() == n) {
                //     std::ofstream file("times.txt", std::ios::trunc);

                //     file << "Frame" << "," << "Time" << "," << "Frame" << "," << "Physics" << "," << "Ticks" << "," << "Render" << std::endl;

                //     double curTime = 0.;
                //     for (unsigned int x = 0; x < frameTimes.size(); x++) {
                //         file << x << "," << curTime << "," << frameTimes[x] << "," << physicsTimes[x] << "," << tickTimes[x] << "," << renderTimes[x] << std::endl;
                //         curTime += frameTimes[x];
                //     }

                //     Fastboi::Quit();
                // }

            // }
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

    // Degree half = 3 * Radian::PI();

    // printf("Half: %f %f\n", half(), (double) half.As<Radian>());
    
    // half = -Radian::PI() / 4;

    // printf("Half: %i %f\n", (int) half, (double) (Radian) half);

    // half = -15_deg;

    // printf("Half: %f %f\n", half(), half.As<Radian>().Value());

    // half = -45_deg;
    // printf("half: %i %f\n", half.As<int>(), half.As<Radian>().Value());


    // if (half == 315_deg)
    //     printf("Half == -45deg == 315deg\n");
    // else
    //     printf("Half == -45deg != 315deg\n");

    // auto a = Radian::PI() / 4;
    // auto b = -7./4. * Radian::PI();

    // printf("a: %f b: %f\n", a.Value(), b.Value());

    // if (a == b)
    //     printf("They're equal!\n");

    // printf("Cache Line Size detected: %lu bytes\n", cacheLineSize);

    // Input::PollEvents();
    // Tick();

    // std::thread bgThread(TickPhysicsThread);

    // updateThreadID = bgThread.get_id();
    // renderingThreadID = std::this_thread::get_id();

    // Render();
    // bgThread.join();
    Resources::GetSoloud().init();

    Input::PollEvents();
    Tick();

    TickPhysicsThread();

    // while (!quit) {
    //     printf("Looping top\n");
    //     if (paused) continue;
    //     printf("Next\n");

    //     Input::PollEvents();
    //     printf("1\n");
    //     Physics();
    //     printf("2\n");
    //     Tick();
    //     printf("3\n");
    //     Render();
    // }

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