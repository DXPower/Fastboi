#include "Gameobject.h"
#include "Collider.h"
#include "FastboiCore.h"

using namespace Fastboi;

Gameobject::Gameobject() : Gameobject(nullptr) { }

Gameobject::Gameobject(const char* name)
 : transform(nullptr)
 , renderer(nullptr)
 , collider(nullptr)
 , name(name) { };

Gameobject::~Gameobject() {
    // Fastboi::GameobjectDeleteEvent delEvent{*this};
    // deleteSignal.fire(delEvent);

    isDestroying = true;
    
    printf("Destroying gameobject...\n");
    transform.reset();
    renderer.reset();
    collider.reset();
}

void Gameobject::Start() {
    printf("Starting %s...", name);
    if (renderer) {
        renderer->Start();
    }
    printf("Started renderer\n");

    if (collider) {
        collider->Start();
    }

    printf("Started collider\n");
    
    componentsLock = true;

    printf("Addr of components: %p\n", &components);
    auto compsPtr = &components;
    
    for (auto& [typekey, comp] : components) {
        printf("Printing type key of starting comp...\n");
        printf("Hash: %lX.\n", typekey);
        printf("Comp ptr: %p.\n", comp.get());
        printf("Comp ptr ptr: %p.\n", comp->Retrieve());
        
        comp->Start();
        Fastboi::Print("Started boi\n");
    }

    AddComponentsOnStack();

    componentsLock = false;
    isStarted = true;
    printf("Started comps!\n");
}

void Gameobject::Update() {
    if (isEnabled && !isDeleted) {
        componentsLock = true;
        AddComponentsOnStack();

        if (collider) 
            collider->Update();
            
        for (auto& [typeKey, comp] : components) {
            comp->Update();
        }

        RemoveComponentsOnStack();
        componentsLock = false;
    }
}


// The components loop cannot be modified while we are looping through it. Thus, we need to have a buffer-stack to
// store components we need to add. However, a component can add a component when it is Start()'ed, so we use a
// stack data structure and a while loop to keep adding+starting components ad infinem.
void Gameobject::AddComponentsOnStack() {
    while (componentsToAdd.size() != 0) {
        auto p = std::move(componentsToAdd.top());
        componentsToAdd.pop();

        p.second->Start();
        
        components.insert(std::move(p));
    }
}

// The components loop cannot be modified while we are looping through it. Thus, we need to have a buffer-stack to
// store components we need to remove. However, a component can remove a component when it is destroyed'ed, so we use a
// stack data structure and a while loop to keep removing components ad infinem.
void Gameobject::RemoveComponentsOnStack() {
    while (componentsToRemove.size() != 0) {
        auto typekey = componentsToRemove.top();
        componentsToRemove.pop();

        components.erase(typekey);
    }
}

void Gameobject::Destroy() {
    isDeleted = true;
    
    if (renderer)
        renderer->Destroy();

    if (collider)
        collider->Destroy();

    Fastboi::Print("Gameobejct destroyed\n");
}

void Gameobject::SetEnabled(bool f) {
    isEnabled = f;

    if (HasComponent<VelocityComp>())
        GetComponent<VelocityComp>().SetEnabled(f);

    if (collider)
        collider->SetEnabled(f);
}