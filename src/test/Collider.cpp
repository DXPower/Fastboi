#include "catch.hpp"
#include "Gameobject.h"
#include "Transform.h"
#include "FastboiCore.h"

#define contains(container, val) (std::find(container.begin(), container.end(), val) != container.end())

using namespace Fastboi;

TEST_CASE("Colliders", "[collider]") {
    Gameobject go;
    printf("Getting shared ptrs\n");
    Transform& transform =  go.AddComponent<Transform>(Position(0, 0), Size(10, 10), 0);
    Collider& collider = go.AddComponent<Collider>(go);
    
    go.Start();

    SECTION("Beginning correct vertices positions") {
        printf("First section begin\n");
        circular_vector<Position> verts = collider.GetVertices();

        REQUIRE(verts.size() == 4);

        REQUIRE(contains(verts, Position(-5.f, -5.f)));
        REQUIRE(contains(verts, Position(5.f, 5.f)));
        REQUIRE(contains(verts, Position(5.f, -5.f)));
        REQUIRE(contains(verts, Position(-5.f, 5.f)));
        printf("First section End\n");
    }

    SECTION("Changing transform changes vertices") {
        printf("Second section begin\n");
        go.transform->position += Position(5.f, 5.f);
        go.Update();

        circular_vector<Position> verts = collider.GetVertices();

        REQUIRE(verts.size() == 4);
        REQUIRE(contains(verts, Position(0.f, 0.f)));
        REQUIRE(contains(verts, Position(10.f, 10.f)));
        REQUIRE(contains(verts, Position(10.f, 0.f)));
        REQUIRE(contains(verts, Position(0.f, 10.f)));
        printf("Second section end\n");
    }

    SECTION("Correct defualt IsTrigger") {
        REQUIRE(collider.IsTrigger() == false);
    }

    SECTION("Correct defaut IsFixed") {
        REQUIRE(collider.IsFixed() == false);
    }

    SECTION("Flags constructor") {
        Gameobject g1;
        Gameobject g2;
        Gameobject g3;
        Gameobject g4;
        Collider& c1 = g1.AddComponent<Collider>(g1);
        Collider& c2 = g2.AddComponent<Collider>(g2, Collider::FIXED | Collider::TRIGGER);
        Collider& c3 = g3.AddComponent<Collider>(g3, Collider::TRIGGER);
        Collider& c4 = g4.AddComponent<Collider>(g4, Collider::FIXED);

        REQUIRE(c1.IsTrigger() == false);
        REQUIRE(c2.IsTrigger() == true);
        REQUIRE(c3.IsTrigger() == true);
        REQUIRE(c4.IsTrigger() == false);
        
        REQUIRE(c1.IsFixed() == false);
        REQUIRE(c2.IsFixed() == true);
        REQUIRE(c3.IsFixed() == false);
        REQUIRE(c4.IsFixed() == true);
    }

    printf("Deleting\n");
}

#include <string>
std::vector<std::string> collisions;
void addCollision(const Fastboi::CollisionEvent& collision) {
    printf("Collison detected!\n");
    if (collision.type == Fastboi::CollisionEvent::BEGIN) {
        collisions.push_back(collision.collider.gameobject.name);
    } else if (collision.type == Fastboi::CollisionEvent::END) {
        printf("Collision end!\n");
        collisions.pop_back();
    }
}

TEST_CASE("Collider integration: new collisions", "[collider]") {
    Gameobject& a = Instantiate<Gameobject>();
    a.name = "new coll Object A";
    a.AddComponent<Transform>(Position(0, 0), Size(10, 10), 0);
    Collider& cA = a.AddComponent<Collider>(a, Collider::FIXED);
    cA.collisionSignal.connect<&addCollision>();

    Gameobject& b = Instantiate<Gameobject>();
    b.name = "new coll Object B";
    b.AddComponent<Transform>(Position(3, 3), Size(10, 10), 0);
    Collider& cB = b.AddComponent<Collider>(b, Collider::FIXED);
    cB.collisionSignal.connect<&addCollision>();

    a.Start();
    b.Start();

    Fastboi::Physics();

    a.Update();
    b.Update();

    SECTION("One tick gives one collision") {
        REQUIRE(collisions.size() == 2);
        REQUIRE(contains(collisions, "new coll Object A"));
        REQUIRE(contains(collisions, "new coll Object B"));
        
        SECTION("Two ticks still gives one collision") {
            Fastboi::Physics();
            a.Update();
            b.Update();

            REQUIRE(collisions.size() == 2);
            REQUIRE(contains(collisions, "new coll Object A"));
            REQUIRE(contains(collisions, "new coll Object B"));
            Fastboi::Tick();
        }
    }

    collisions.clear();

    Destroy(a);
    Destroy(b);

    Fastboi::Tick();

    printf("Destroyed new collision testers\n");
}

TEST_CASE("Collider integration: ending collisions", "[collider]") {
    collisions.clear();

    Gameobject& a = Instantiate<Gameobject>();
    a.name = "ending coll Object A";
    a.AddComponent<Transform>(Position(0, 0), Size(10, 10), 0);
    Collider& cA = a.AddComponent<Collider>(a);
    cA.collisionSignal.connect<&addCollision>();

    Gameobject& b = Instantiate<Gameobject>();
    b.name = "ending coll Object B";
    b.AddComponent<Transform>(Position(3, 3), Size(10, 10), 0);
    Collider& cB = b.AddComponent<Collider>(b);
    cB.collisionSignal.connect<&addCollision>();

    a.Start();
    b.Start();

    Fastboi::Physics();
    a.Update();
    b.Update();

    a.transform->position += 50;
    // cA->TransformChanged(*a->transform);

    Fastboi::Physics();
    a.Update();
    b.Update();
    
    Fastboi::Physics();
    a.Update();
    b.Update();

    Fastboi::Physics();
    a.Update();
    b.Update();

    REQUIRE(collisions.size() == 0);

    Destroy(a);
    Destroy(b);

    Fastboi::Tick();
    collisions.clear();
}

TEST_CASE("Collider integration: Two unfixed nontrigger colliders", "[collider]") {
    Gameobject& g1 = Instantiate<Gameobject>();
    Gameobject& g2 = Instantiate<Gameobject>();

    g1.name = "Object 1";
    g2.name = "Object 2";

    constexpr Position p1(0, 0);
    constexpr Position p2(3, 3);

    // Add transforms
    g1.AddComponent<Transform>(p1, Size(50, 50), 0);
    g2.AddComponent<Transform>(p2, Size(50, 50), 0);

    // Add colliders
    g1.AddComponent<Collider>(g1);
    g2.AddComponent<Collider>(g2);

    g1.Start();
    g2.Start();

    Fastboi::Physics();
    g1.Update();
    g2.Update();

    REQUIRE(g1.transform->position != p1);
    REQUIRE(g2.transform->position != p2);

    Destroy(g1);
    Destroy(g2);

    Fastboi::Tick();
}

TEST_CASE("Collider integration: One trigger unfixed; one nontrigger unfixed colliders", "[collider]") {
    Gameobject& g1 = Instantiate<Gameobject>(); // g1 is the unfixed trigger
    Gameobject& g2 = Instantiate<Gameobject>(); // g2 is the unfixed nontrigger

    g1.name = "Object 1";
    g2.name = "Object 2";

    constexpr Position p1(0, 0);
    constexpr Position p2(3, 3);

    // Add transforms
    g1.AddComponent<Transform>(p1, Size(50, 50), 0);
    g2.AddComponent<Transform>(p2, Size(50, 50), 0);

    // Add colliders
    g1.AddComponent<Collider>(g1, Collider::TRIGGER);
    g2.AddComponent<Collider>(g2);

    printf("Colliders added\n");

    g1.Start();
    g2.Start();

    Fastboi::Physics();
    g1.Update();
    g2.Update();

    REQUIRE(g1.transform->position == p1);
    REQUIRE(g2.transform->position == p2);

    Destroy(g1);
    Destroy(g2);

    Fastboi::Tick();
}
