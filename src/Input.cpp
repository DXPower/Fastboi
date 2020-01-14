#include "Input.h"
#include <algorithm>
#include "Application.h"
#include <memory>
#include "SDL/SDL.h"
#include "Renderer.h"
#include "Transform.h"
#include <unordered_map>

using namespace Fastboi;
using namespace Input;

const uint8_t* keyboardState;

std::vector<KeyListener*> keyListeners;
std::vector<ClickListener*> untargetedClickListeners;
std::vector<TargetedClickListener*> targetedClickListeners;

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
        if (tcl->transform->ContainsPoint(e.pos)) {
            targets.push_back(tcl);
        }
    }

    if (targets.size() > 0) {
        // Sort by z-index and get the topmost transform
        std::sort(targets.begin(), targets.end(), tclCompare);

        TargetedClickEvent te(*targets[0]->transform, e.type, e.button, e.clicks, e.pos);
        targets[0]->signal->fire(te);

        return true;
    }

    return false;
}

void DispatchUntargetedClick(const ClickEvent& e) {
    for (const ClickListener* cl : untargetedClickListeners) {
        cl->signal->fire(e);
    }
}

void Input::PollEvents() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                Application::quit = true;
                break;
            case SDL_KEYDOWN: {
                SDL_KeyboardEvent sdl_keyEvent = event.key;

                if (event.key.repeat) break;

                const KeyEvent k = KeyEvent(sdl_keyEvent.keysym.scancode, KeyEvent::KeyEventType::DOWN);

                for (const KeyListener* listener : keyListeners) {
                    if (listener->key == k.key)
                        listener->signal->fire(k);
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
}

bool Input::IsKeyDown(uint8_t key) {
    return keyboardState[key] == 1;
}

ClickListener::ClickListener() : signal(std::make_unique<Signal<EventSignature>>()) {
    untargetedClickListeners.push_back(this);
}

ClickListener::~ClickListener() {
    untargetedClickListeners.erase(std::find(untargetedClickListeners.begin(), untargetedClickListeners.end(), this));
}

TargetedClickListener::TargetedClickListener()
 : transform(nullptr)
 , signal(std::make_unique<Signal<EventSignature>>())
{ };

TargetedClickListener::~TargetedClickListener() {
    targetedClickListeners.erase(std::find(targetedClickListeners.begin(), targetedClickListeners.end(), this));
}

void TargetedClickListener::Init(const Transform* t, const Renderer* r) {
    transform = t;
    renderer = r;

    targetedClickListeners.push_back(this);
}

bool TargetedClickListener::operator==(const TargetedClickListener& other) const {
    return transform == other.transform && signal == other.signal;
}

KeyListener::KeyListener(uint32_t key)
 : key(key)
 , signal(std::make_unique<Signal<EventSignature>>()) {

     keyListeners.push_back(this);
}

KeyListener::~KeyListener() {
    keyListeners.erase(std::find(keyListeners.begin(), keyListeners.end(), this));
}