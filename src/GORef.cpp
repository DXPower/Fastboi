#include "GORef.h"
#include "Gameobject.h"

using namespace Fastboi;

GORef::GORef(Gameobject& go) : go(&go) { }
GORef::GORef(Gameobject& go, ComponentBase& owner) : go(&go), owningComp(&owner) { }

GORef::GORef(const GORef&& mv) : go(mv.go), owningComp(mv.owningComp) {
    if (owningComp != nullptr)
        owningComp->internalGORef = this;
};

GORef& GORef::operator=(Gameobject& rebind) {
    go = &rebind;

    return *this;
}
