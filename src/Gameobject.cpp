#include "Gameobject.h"
#include "ColorComp.h"
#include "Collider.h"
#include "FastboiCore.h"

using namespace Fastboi;

Gameobject::Gameobject() : Gameobject("") { }

Gameobject::Gameobject(const std::string& name)
 : transform(nullptr)
 , renderer(nullptr)
 , collider(nullptr)
 , name(name) { };

Gameobject::~Gameobject() {
    isDestroying = true;
    
    transform.reset();
    renderer.reset();
    collider.reset();
}

void Gameobject::Start() {
    printf("Starting go %s at %p\n", name.c_str(), this);
    if (renderer) {
        printf("Starting renderer %s\n", name.c_str());
        renderer->Start();
    }

    if (collider) {
        printf("Starting collider %s\n", name.c_str());
        collider->Start();
    }

    componentsLock = true;

    for (auto& [typekey, comp] : components) {
        printf("Starting comp %s\n", comp->typekey.name().cppstring().c_str());
        comp->Start();
        printf("Comp started!\n");
    }

    AddComponentsOnStack();

    componentsLock = false;
    isStarted = true;

    printf("All comps started!\n");
}

void Gameobject::Update() {
    // if (name == "Duplicate") {
    //     printf("Updating dupe!\n");

    //     printf("Renderer started: %i\n", renderer->isStarted);
    // }
    
    if (isEnabled && !isDeleted) {
        componentsLock = true;
        RemoveComponentsOnStack();

        if (collider) {
            collider->Update();
        }

        for (auto& [typeKey, comp] : components) {
            comp->Update();
        }

        AddComponentsOnStack();
        componentsLock = false;
    }
}

Gameobject& Gameobject::Duplicate() const {
    Gameobject& dup = Instantiate<Gameobject>(name);
    
    if (transform)
        dup.transform = std::make_unique<Transform>(*transform);

    if (collider) {
        dup.collider = std::make_unique<Collider>(*collider);
        dup.collider->gameobject.go = &dup;
    }

    if (renderer) {
        printf("Duplicating renderer\n");
        dup.renderer.reset(&renderer->Duplicate());
        dup.renderer->gameobject.go = &dup;
        printf("Duped renderer, new one at %p\n", dup.renderer.get());
    }

    for (const auto& [typekey, comp] : components) {
        printf("Duplicating component %s\n", comp->typekey.name().cppstring().c_str());
        ComponentBase& dupComp = comp->CreateEmpty();
        comp->Duplicate(dupComp, &dup);

        if (typekey == ctti::type_id<Fastboi::Components::ColorShiftComp>().hash()) {
            auto& maincs = GetComponent<Fastboi::Components::ColorShiftComp>();
            printf("MAIN ======= addr: %p, goref: %p\n", &maincs, &maincs.gameobject);

            Fastboi::Components::ColorShiftComp& cs = *reinterpret_cast<Fastboi::Components::ColorShiftComp*>(dupComp.Retrieve()); 
            printf("Internal go ref go: %p. Dup: %p. Owning comp: %p\n", cs.gameobject.go, &dup, cs.gameobject.owningComp);
            printf("ADDRESS OF SRC: %p, DUPCOMP: %p\n", comp.get(), &dupComp);
            printf("INT GOREF: %p. ADDRESS OF CS: %p, ADDRESS OF CS GOREF: %p\n", dupComp.internalGORef, &cs, &cs.gameobject);
        }
        
        dup.components.emplace(typekey, std::unique_ptr<ComponentBase>(&dupComp));
    }

    dup.name = "Duplicate";
    return dup;
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
}

void Gameobject::SetEnabled(bool f) {
    isEnabled = f;

    if (HasComponent<VelocityComp>())
        GetComponent<VelocityComp>().SetEnabled(f);

    if (collider)
        collider->SetEnabled(f);
}


// Fastboi::GameobjectAllocator allocator(4);

// void* Gameobject::operator new(std::size_t size) {
//     return static_cast<void*>(allocator.Allocate());
// }

// void Gameobject::operator delete(void* ptr, size_t size) {
//     return allocator.Deallocate(ptr);
// }