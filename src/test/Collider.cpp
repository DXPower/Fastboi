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
    Collider& collider = go.AddComponent<Collider>(&go);
    
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

    SECTION("Correct defualt isTrigger") {
        REQUIRE(collider.isTrigger == false);
    }

    SECTION("Correct defaut isFixed") {
        REQUIRE(collider.isFixed == false);
    }

    SECTION("isTrigger, isFixed constructor") {
        printf("Long section begin\n");
        Gameobject g1;
        Gameobject g2;
        Gameobject g3;
        Gameobject g4;
        Collider& c1 = g1.AddComponent<Collider>(&g1, false, false);
        Collider& c2 = g2.AddComponent<Collider>(&g2, true, true);
        Collider& c3 = g3.AddComponent<Collider>(&g3, true, false);
        Collider& c4 = g4.AddComponent<Collider>(&g4, false, true);

        REQUIRE(c1.isTrigger == false);
        REQUIRE(c2.isTrigger == true);
        REQUIRE(c3.isTrigger == true);
        REQUIRE(c4.isTrigger == false);
        
        REQUIRE(c1.isFixed == false);
        REQUIRE(c2.isFixed == true);
        REQUIRE(c3.isFixed == false);
        REQUIRE(c4.isFixed == true);

        printf("Long section end\n");
    }

    printf("Deleting\n");
}

#include <string>
std::vector<std::string> collisions;
void addCollision(const Fastboi::CollisionEvent& collision) {
    printf("Collison detected!\n");
    if (collision.type == Fastboi::CollisionEvent::BEGIN) {
        collisions.push_back(collision.collider.gameobject->name);
    } else if (collision.type == Fastboi::CollisionEvent::END) {
        printf("Collision end!\n");
        collisions.pop_back();
    }
}

TEST_CASE("Collider integration: new collisions", "[collider]") {
    Gameobject* a = Instantiate<Gameobject>();
    a->name = "Object A";
    a->AddComponent<Transform>(Position(0, 0), Size(10, 10), 0);
    Collider& cA = a->AddComponent<Collider>(a, false, true);
    cA.collisionSignal.connect<&addCollision>();

    Gameobject* b = Instantiate<Gameobject>();
    b->name = "Object B";
    b->AddComponent<Transform>(Position(3, 3), Size(10, 10), 0);
    Collider& cB = b->AddComponent<Collider>(b, false, true);
    cB.collisionSignal.connect<&addCollision>();

    a->Start();
    b->Start();

    Fastboi::Physics();

    a->Update();
    b->Update();

    SECTION("One tick gives one collision") {
        REQUIRE(collisions.size() == 2);
        REQUIRE(contains(collisions, "Object A"));
        REQUIRE(contains(collisions, "Object B"));
        
        SECTION("Two ticks still gives one collision") {
            Fastboi::Physics();
            a->Update();
            b->Update();

            REQUIRE(collisions.size() == 2);
            REQUIRE(contains(collisions, "Object A"));
            REQUIRE(contains(collisions, "Object B"));
            Destroy(a);
            Destroy(b);
        }
    }

    collisions.clear();
    Destroy(a);
    Destroy(b);
}

TEST_CASE("Collider integration: ending collisions", "[collider]") {
    collisions.clear();

    Gameobject* a = Instantiate<Gameobject>();
    a->name = "Object A";
    a->AddComponent<Transform>(Position(0, 0), Size(10, 10), 0);
    Collider& cA = a->AddComponent<Collider>(a, false, true);
    cA.collisionSignal.connect<&addCollision>();

    Gameobject* b = Instantiate<Gameobject>();
    b->name = "Object B";
    b->AddComponent<Transform>(Position(3, 3), Size(10, 10), 0);
    Collider& cB = b->AddComponent<Collider>(b, false, true);
    cB.collisionSignal.connect<&addCollision>();

    a->Start();
    b->Start();

    Fastboi::Physics();
    a->Update();
    b->Update();

    a->transform->position += 50;
    // cA->TransformChanged(*a->transform);

    Fastboi::Physics();
    a->Update();
    b->Update();
    
    Fastboi::Physics();
    a->Update();
    b->Update();

    Fastboi::Physics();
    a->Update();
    b->Update();

    REQUIRE(collisions.size() == 0);

    Destroy(a);
    Destroy(b);

    collisions.clear();
}

TEST_CASE("Collider integration: Two unfixed nontrigger colliders", "[collider]") {
    Gameobject* g1 = Instantiate<Gameobject>();
    Gameobject* g2 = Instantiate<Gameobject>();

    g1->name = "Object 1";
    g2->name = "Object 2";

    constexpr Position p1(0, 0);
    constexpr Position p2(3, 3);

    // Add transforms
    g1->AddComponent<Transform>(p1, Size(50, 50), 0);
    g2->AddComponent<Transform>(p2, Size(50, 50), 0);

    // Add colliders
    g1->AddComponent<Collider>(g1);
    g2->AddComponent<Collider>(g2);

    g1->Start();
    g2->Start();

    Fastboi::Physics();
    g1->Update();
    g2->Update();

    REQUIRE(g1->transform->position != p1);
    REQUIRE(g2->transform->position != p2);

    Destroy(g1);
    Destroy(g2);
}

TEST_CASE("Collider integration: One trigger unfixed; one nontrigger unfixed colliders", "[collider]") {
    Gameobject* g1 = Instantiate<Gameobject>(); // g1 is the unfixed trigger
    Gameobject* g2 = Instantiate<Gameobject>(); // g2 is the unfixed nontrigger

    g1->name = "Object 1";
    g2->name = "Object 2";

    constexpr Position p1(0, 0);
    constexpr Position p2(3, 3);

    // Add transforms
    g1->AddComponent<Transform>(p1, Size(50, 50), 0);
    g2->AddComponent<Transform>(p2, Size(50, 50), 0);

    // Add colliders
    g1->AddComponent<Collider>(g1, true, false);
    g2->AddComponent<Collider>(g2);

    g1->Start();
    g2->Start();

    Fastboi::Physics();
    g1->Update();
    g2->Update();

    REQUIRE(g1->transform->position == p1);
    REQUIRE(g2->transform->position == p2);

    Destroy(g1);
    Destroy(g2);
}
