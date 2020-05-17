#include "GORef.h"
#include "Gameobject.h"

using namespace Fastboi;

GORef::GORef(Gameobject& go) : go(&go) { }
GORef::GORef(Gameobject& go, detail::ComponentBase& owner) : go(&go), owningComp(&owner) { }

GORef::GORef(const GORef& copy) : go(copy.go), owningComp(copy.owningComp) {
    if (owningComp != nullptr && !owningComp->isDuplicating)
        Application::ThrowRuntimeException("Attempt to copy-construct component in non-duplicating context"
            , Application::COPY_COMP_NO_DUPE
            , "Hint: use std::move(GORef) when storing GORef in component cstr");
}

GORef::GORef(const GORef&& mv) : go(mv.go), owningComp(mv.owningComp) {
    if (owningComp != nullptr)
        owningComp->internalGORef = this;
};

GORef::~GORef() {
    if (owningComp != nullptr && owningComp->internalGORef == this)
        owningComp->internalGORef = nullptr;
}

GORef& GORef::operator=(Gameobject& rebind) {
    go = &rebind;

    return *this;
}
