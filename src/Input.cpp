#include "Input.h"
#include <algorithm>
#include "Application.h"
#include "Camera.h"
#include "FastboiCore.h"
#include <memory>
#include "SDL/SDL.h"
#include "Renderer.h"
#include "SDL/SDL_events.h"
#include "SDL/SDL_mouse.h"
#include "ScreenElement.h"
#include "Timer.h"
#include "Transform.h"
#include <unordered_map>

using namespace Fastboi;
using namespace Input;

const uint8_t* keyboardState;

Vec<int> mousePosition(0, 0);
bool blockTargetedMouseUp = false;

std::vector<KeyListener*> keyListeners;
std::vector<ClickListener*> untargetedClickListeners;
std::vector<TargetedClickListener*> targetedClickListeners;
std::vector<MouseWheelListener*> mouseWheelListeners;

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
        if (tcl->transform != nullptr) {
            if (tcl->transform->gameobject().HasComponent<ScreenElement>()) {
                Transform screenT = tcl->transform->gameobject().GetComponent<ScreenElement>().GetScreenTransform();
                Position searchPos = static_cast<Position>(e.pos);

                if (screenT.ContainsPoint(searchPos))
                    targets.push_back(tcl);
                
            } else {
                Position searchPos = Fastboi::GetCamera().ScreenToWorldPos(e.pos);

                if (tcl->transform->ContainsPoint(searchPos))
                    targets.push_back(tcl);
            }
        }
    }

    if (targets.size() > 0) {
        printf("Clicked on target!\n");
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
    SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

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
                
                // If we are blocking targeted mouse up, immediately move to dispatching untargeted mouse up
                // If !blockTargetedMouseUp, then check if we are targeting something.
                // If not, then do untargeted mouse up.
                if (blockTargetedMouseUp || !DispatchTargetedClick(mouseEvent)) {
                    DispatchUntargetedClick(mouseEvent);
                    blockTargetedMouseUp = false;
                }
                    
                break;
            }
            case SDL_MOUSEWHEEL: {
                SDL_MouseWheelEvent sdl_wheelEvent = event.wheel;

                Vec<int> scroll = Vec<int>(sdl_wheelEvent.x, sdl_wheelEvent.y);

                if (sdl_wheelEvent.direction == SDL_MOUSEWHEEL_FLIPPED)
                    scroll *= -1;

                MouseWheelEvent wheelEvent { .scroll = scroll };

                for (MouseWheelListener* listener : mouseWheelListeners) {
                    listener->signal.fire(wheelEvent);
                }

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

const Vec<int>& Input::GetMousePosition() {
    return mousePosition;
}

const uint8_t* Input::GetKeyboardState() {
    return keyboardState;
}

void Input::BlockTargetedMouseUp() {
    blockTargetedMouseUp = true;
}

ClickListener::ClickListener() {
    untargetedClickListeners.push_back(this);
}

ClickListener::~ClickListener() {
    untargetedClickListeners.erase(std::find(untargetedClickListeners.begin(), untargetedClickListeners.end(), this));
}

TargetedClickListener::TargetedClickListener() : TargetedClickListener(nullptr, nullptr) { };
TargetedClickListener::TargetedClickListener(Transform& t, Renderer& r) : TargetedClickListener(&t, &r) { };
TargetedClickListener::TargetedClickListener(Transform* t, Renderer* r) : transform(t), renderer(r) {
	targetedClickListeners.push_back(this);
}

TargetedClickListener::~TargetedClickListener() {
    targetedClickListeners.erase(std::find(targetedClickListeners.begin(), targetedClickListeners.end(), this));
}

void TargetedClickListener::Init(Transform& t, Renderer& r) {
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

MouseWheelListener::MouseWheelListener() {
    mouseWheelListeners.push_back(this);
};

MouseWheelListener::~MouseWheelListener() {
    mouseWheelListeners.erase(std::find(mouseWheelListeners.begin(), mouseWheelListeners.end(), this));
}