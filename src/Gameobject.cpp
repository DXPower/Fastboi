#include "Gameobject.h"
#include "Collider.h"
#include "FastboiCore.h"

Gameobject::Gameobject() : Gameobject("") { }

Gameobject::Gameobject(const char* name)
 : transform(nullptr)
 , renderer(nullptr)
 , collider(nullptr)
 , name(name) { };

Gameobject::~Gameobject() {
    Fastboi::GameobjectDeleteEvent delEvent{*this};
    deleteSignal.fire(delEvent);

    isDestroying = true;
    
    transform.reset();
    renderer.reset();
    collider.reset();
}

void Gameobject::Start() {
    printf("Starting %s\n", name);
    if (renderer) {
        renderer->Start();
    }

    if (collider) {
        collider->Start();
    }
    
    for (auto& [typeKey, comp] : components) {
        comp->Start();
    }

    isStarted = true;
}

void Gameobject::Update() {
    if (!isDeleted) {
        if (collider) 
            collider->Update();
            
        for (auto& [typeKey, comp] : components) {
            comp->Update();
        }
    }
}

void Gameobject::Destroy() {
    isDeleted = true;
    
    if (renderer)
        renderer->Destroy();

    if (collider)
        collider->Destroy();
}