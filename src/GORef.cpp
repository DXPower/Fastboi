#include "GORef.h"
#include "Gameobject.h"

using namespace Fastboi;

GORef::GORef(Gameobject& go) : go(&go) { }

GORef& GORef::operator=(Gameobject& rebind) {
    go = &rebind;

    return *this;
}
