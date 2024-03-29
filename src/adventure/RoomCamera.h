#include "Fastboi.h"
#include "Level.h"
#include "Room.h"
#include <iostream>

namespace Adventure {
    struct RoomCamera {
        GORef gameobject;        

        RoomCamera(GORef&& go) : gameobject(std::move(go)) {
            Level::roomChangeSignal.connect<&RoomCamera::RoomChanged>(*this);
        }

        void Start() {
            Camera& camera = gameobject().AddComponent<Camera>(*gameobject().transform, 0.8f);
            Fastboi::SetCamera(camera);
            
            std::cout << "Starting room camera";
        }

        void RoomChanged(const RoomChangeEvent& e) const {
            std::cout << "Room camera changed";
            gameobject().transform->position = e.room.GetCenter();
        };
    };
}