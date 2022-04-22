#include "Fastboi.h"
#include "Level.h"
#include "Room.h"

namespace Adventure {
    struct RoomCamera {
        GORef gameobject;        

        RoomCamera(GORef&& go) : gameobject(std::move(go)) {
            Camera& camera = gameobject().AddComponent<Camera>(*gameobject().transform, 0.8f);
            Fastboi::SetCamera(camera);

            Level::roomChangeSignal.connect<&RoomCamera::RoomChanged>(*this);
        }

        void Start(Gameobject& go) {
            std::cout << "Starting room camera";
        }

        void RoomChanged(const RoomChangeEvent& e) const {
            std::cout << "Room camera changed";
            gameobject().transform->position = e.room.GetCenter();
        };
    };
}