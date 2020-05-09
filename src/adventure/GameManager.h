#pragma once

#include "Fastboi.h"
#include "Level.h"
#include "Dragon.h"
#include "Item.h"
#include "Player.h"

using namespace Fastboi;


namespace Adventure {
    struct GameManager {
        Dragon* yorgle = nullptr;
        Dragon* grundle = nullptr;
        Dragon* rhindle = nullptr;

        Item* blackKey = nullptr;
        Room* goldCastleInside = nullptr;

        private:
        bool hasSeenYorgle = false;

        public:
        void MoveDragons(const Room& room);
        void RoomChanged(const RoomChangeEvent& e);
        void WinGame() const;
    };

    inline GameManager manager;
}

// inline Adventure::GameManager Adventure::manager{};