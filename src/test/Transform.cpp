#include "catch.hpp"
#include "Transform.h"
#include "Gameobject.h"
#include "FastboiCore.h"

using namespace Fastboi;

TEST_CASE("Transform isolated", "[transform]") {
    SECTION("Default construction") {
        Transform transform;

        REQUIRE(transform.position.x == 0);
        REQUIRE(transform.position.y == 0);
        REQUIRE(transform.size.x == 0);
        REQUIRE(transform.size.y == 0);
        REQUIRE(transform.rotation == 0_deg);
    }

    SECTION("Position construction") {
        Transform transform(Position(15.f, -20.f));

        REQUIRE(transform.position.x == 15.f);
        REQUIRE(transform.position.y == -20.f);
        REQUIRE(transform.size.x == 0);
        REQUIRE(transform.size.y == 0);
        REQUIRE(transform.rotation == 0_deg);
    }

    SECTION("Total construction") {
        Transform transform(Position(15.f, -20.f), Size(13.f, 45.f), 95._deg);

        REQUIRE(transform.position.x == 15.f);
        REQUIRE(transform.position.y == -20.f);
        REQUIRE(transform.size.x == 13.f);
        REQUIRE(transform.size.y == 45.f);
        REQUIRE(transform.rotation == 95._deg);
    }

    SECTION("Equality") {
        Transform a(Position(25.f, 15.f), Size(13.f, 45.f), 95._deg);
        Transform b(Position(25.f, 15.f), Size(13.f, 45.f), 95._deg);
        Transform c(Position(24.f, 15.f), Size(13.f, 45.f), 95._deg);
        Transform d(Position(24.f, 15.f), Size(12.f, 45.f), 95._deg);
        Transform e(Position(24.f, 15.f), Size(12.f, 45.f), 94._deg);

        REQUIRE(a == b);
        REQUIRE(a != c);
        REQUIRE(a != d);
        REQUIRE(a != e);
    }
}

TEST_CASE("Transform integration w/ Gameobject", "[transform]") {
    Gameobject go;

    SECTION("Default construction") {
        go.AddComponent<Transform>();

        REQUIRE(go.GetComponent<Transform>().position.x == 0);
        REQUIRE(go.GetComponent<Transform>().position.y == 0);
        REQUIRE(go.GetComponent<Transform>().size.x == 0);
        REQUIRE(go.GetComponent<Transform>().size.y == 0);
        REQUIRE(go.GetComponent<Transform>().rotation == 0_deg);
    }

    SECTION("Position construction") {
        go.AddComponent<Transform>(Position(25.f, -50.f));

        REQUIRE(go.GetComponent<Transform>().position.x == 25.f);
        REQUIRE(go.GetComponent<Transform>().position.y == -50.f);
        REQUIRE(go.GetComponent<Transform>().size.x == 0);
        REQUIRE(go.GetComponent<Transform>().size.y == 0);
        REQUIRE(go.GetComponent<Transform>().rotation == 0_deg);
    }

    SECTION("Total construction") {
        go.AddComponent<Transform>(Position(25.f, -50.f), Size(12.f, 40.f), 35._deg);

        REQUIRE(go.GetComponent<Transform>().position.x == 25.f);
        REQUIRE(go.GetComponent<Transform>().position.y == -50.f);
        REQUIRE(go.GetComponent<Transform>().size.x == 12.f);
        REQUIRE(go.GetComponent<Transform>().size.y == 40.f);
        REQUIRE(go.GetComponent<Transform>().rotation == 35._deg);
    }

    Destroy(go);
}