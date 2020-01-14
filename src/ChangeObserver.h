#pragma once
#include "Events.h"
#include <functional>
#include <memory>
#include <stdint.h>

#define likely(x)      __builtin_expect(!!(x), 1) 
#define unlikely(x)    __builtin_expect(!!(x), 0) 

template<class Watch>
class ChangeObserver {
    private:
    const Watch* watch;
    Watch prevValue;

    public:
    using Signal_f_t = void(const Watch&);
    Fastboi::Signal<Signal_f_t> signal;

    ChangeObserver(const Watch* ptr) : watch(ptr) {
        prevValue = *ptr;
    };

    ~ChangeObserver() {
        signal.disconnect_all();
    }

    void Fire() {
        signal.fire(*watch);
    }

    void Update() {
        if (unlikely(prevValue != *watch)) {
            Fire();

            prevValue = *watch;
        }
    }
};

#undef likely
#undef unlikely