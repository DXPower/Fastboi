#pragma once

#include "Room.h"
#include <vector>

namespace Adventure {
    struct RoomObserver {
        GORef go;
        Room* curRoom = nullptr;
        Rigidbody* rb = nullptr;

        RoomObserver(GORef&& go) : go(std::move(go)) { }

        void Start();
        void Update();

        void ForceTeleport(const Position& pos);

        private:
        void ChangeRoom(Room& room);
    };

    struct RoomChangeEvent {
        Room& room;
        Gameobject& player;
    };;

    struct Level {
        private:
        static inline std::vector<std::vector<std::unique_ptr<Room>>> rooms;
        static inline Gameobject* player;

        public:
        static inline constexpr Position origin = Position::zero();
        static inline Vec<int> gcCoords = Vec<int>::zero();

        using Signal_t_g = void(const RoomChangeEvent&);
        inline static Signal<Signal_t_g> roomChangeSignal;

        static Room& AddRoom(const Layouts::Layout_t& layout);

        inline static Room& GetRoom(int x, int y) { return *rooms[y][x]; };
        inline static Room& GetRoom(const Vec<int>& coords) { return GetRoom(coords.x, coords.y); };

        inline static void Clear() { rooms.clear(); };

        inline static void SetPlayer(Gameobject& p) { player = &p; p.AddComponent<RoomObserver>(); };
        inline static Gameobject& GetPlayer() { return *player; };

        static Room& GetRoom(const Position& pos);
        static Room& GetRoomMovement(const Position& pos, const Velocity& v, Vec<int>& dirMovedOut);

        static void SetLevelSize(size_t x, size_t y);

        friend struct RoomObserver; // In charge of watching player and firing roomChangeSignal
    };
}