#include "Input.h"
#include <algorithm>
#include "Application.h"
#include "FastboiCore.h"
#include <memory>
#include "SDL/SDL.h"
#include "Renderer.h"
#include "Timer.h"
#include "Transform.h"
#include <unordered_map>

using namespace Fastboi;
using namespace Input;

const uint8_t* keyboardState;

std::vector<KeyListener*> keyListeners;
std::vector<ClickListener*> untargetedClickListeners;
std::vector<TargetedClickListener*> targetedClickListeners;

Signal<WindowResizeEvent::Signal_t_g> Input::resizeSignal;

void CheckCombinations();

auto tclCompare = [](const TargetedClickListener* left, const TargetedClickListener* right) {
    if (left->renderer->GetOrder() == right->renderer->GetOrder()) {
        return left->renderer->GetZ() > right->renderer->GetZ();
    } else {
        return left->renderer->GetOrder() > right->renderer->GetOrder();
    } 
};

bool DispatchTargetedClick(const ClickEvent& e) {
    std::vector<const Input::TargetedClickListener*> targets;

    // Get all the transform targets which are under the mouse
    for (const TargetedClickListener* tcl : targetedClickListeners) {
        if (tcl->transform != nullptr && tcl->transform->ContainsPoint(e.pos)) {
            targets.push_back(tcl);
        }
    }

    if (targets.size() > 0) {
        // Sort by z-index and get the topmost transform
        std::sort(targets.begin(), targets.end(), tclCompare);

        TargetedClickEvent te(*targets[0]->transform, e.type, e.button, e.clicks, e.pos);
        targets[0]->signal.fire(te);

        return true;
    }

    return false;
}

void DispatchUntargetedClick(const ClickEvent& e) {
    for (const ClickListener* cl : untargetedClickListeners) {
        cl->signal.fire(e);
    }
}

void Input::PollEvents() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                Fastboi::Quit();
                break;
            case SDL_WINDOWEVENT: {
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED: {
                        
                        // This is an awful hack to fix SDL automatically resizing the window when entering fullscreen
                        // for the FIRST time... It basically captures the event from the automatic resize then sets the
                        // display mode AGAIN.
                        if (Application::IsFullscreen()) {
                            SDL_DisplayMode dm;
                            SDL_GetDisplayMode(SDL_GetWindowDisplayIndex(Application::gWindow), 0, &dm);
                            SDL_SetWindowDisplayMode(Application::gWindow, &dm);
                            printf("Window resize event fix-attempt! %i %i\n", dm.w, dm.h);
                            Application::WindowSizeChanged(Vec<int>(dm.w, dm.h));
                        } else {
                            printf("Window resize event! %i %i\n", event.window.data1, event.window.data2);
                            Application::WindowSizeChanged(Vec<int>(event.window.data1, event.window.data2));
                        }
                    }
                    break;
                }

                break;
            }
            case SDL_KEYDOWN: {
                SDL_KeyboardEvent sdl_keyEvent = event.key;

                if (event.key.repeat) break;

                const KeyEvent k = KeyEvent(sdl_keyEvent.keysym.scancode, KeyEvent::KeyEventType::DOWN);

                for (const KeyListener* listener : keyListeners) {
                    if (listener->IsListeningToKey(k.key))
                        listener->signal.fire(k);
                }

                break;
            }
            case SDL_MOUSEBUTTONDOWN: {
                SDL_MouseButtonEvent sdl_mouseEvent = event.button;
                ClickEvent mouseEvent(
                    ClickEvent::DOWN
                    , sdl_mouseEvent.button
                    , sdl_mouseEvent.clicks
                    , Vec<int>(sdl_mouseEvent.x, sdl_mouseEvent.y)
                );
                
                if (!DispatchTargetedClick(mouseEvent))
                    DispatchUntargetedClick(mouseEvent);

                break;
            }
            case SDL_MOUSEBUTTONUP: {
                SDL_MouseButtonEvent sdl_mouseEvent = event.button;
                ClickEvent mouseEvent(
                    ClickEvent::UP
                    , sdl_mouseEvent.button
                    , sdl_mouseEvent.clicks
                    , Vec<int>(sdl_mouseEvent.x, sdl_mouseEvent.y)
                );
                
                if (!DispatchTargetedClick(mouseEvent))
                    DispatchUntargetedClick(mouseEvent);
                    
                break;
            }
            default:
                break;
        }
    }

    keyboardState = SDL_GetKeyboardState(NULL);

    CheckCombinations();
}

void CheckCombinations() {
    bool lalt = IsKeyDown(SDL_SCANCODE_LALT);
    bool enter = IsKeyDown(SDL_SCANCODE_RETURN);

    static Timer timer;

    if (lalt && enter) {
        printf("Timer: %f\n", timer.elapsed_seconds.count());
        if (timer.TimeSinceLastTick() > std::chrono::duration<double, std::centi>(100)) {
            timer.Tick();
            Application::ToggleFullscreen();
        }
    }
}

const uint8_t* Input::GetKeyboardState() {
    return keyboardState;
}

ClickListener::ClickListener() {
    untargetedClickListeners.push_back(this);
}

ClickListener::~ClickListener() {
    untargetedClickListeners.erase(std::find(untargetedClickListeners.begin(), untargetedClickListeners.end(), this));
}

TargetedClickListener::TargetedClickListener() : TargetedClickListener(nullptr, nullptr) { };
TargetedClickListener::TargetedClickListener(const Transform& t, const Renderer& r) : TargetedClickListener(&t, &r) { };
TargetedClickListener::TargetedClickListener(const Transform* t, const Renderer* r) : transform(t), renderer(r) {
	targetedClickListeners.push_back(this);
}

TargetedClickListener::~TargetedClickListener() {
    targetedClickListeners.erase(std::find(targetedClickListeners.begin(), targetedClickListeners.end(), this));
}

void TargetedClickListener::Init(const Transform& t, const Renderer& r) {
    transform = &t;
    renderer = &r;

    targetedClickListeners.push_back(this);
}


KeyListener::KeyListener(uint32_t key) : KeyListener({ key }) { }
KeyListener::KeyListener(std::initializer_list<uint32_t> list) : keys(list) {
	keyListeners.push_back(this);
}

KeyListener::~KeyListener() {
    keyListeners.erase(std::find(keyListeners.begin(), keyListeners.end(), this));
}

bool KeyListener::IsListeningToKey(uint32_t key) const {
	return std::find(keys.begin(), keys.end(), key) != keys.end();
}