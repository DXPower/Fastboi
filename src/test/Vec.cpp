#include "catch.hpp"
#include "Vec.h"

using namespace Fastboi;

TEST_CASE("Vector creation and operations", "[vec]") {
    Vecf a(3.f, 4.f);
    Vec<int> b(173, 29543);
    Vecb c(14, 0);

    REQUIRE((a.x == 3.f && a.y == 4.f));
    REQUIRE((b.x == 173 && b.y == 29543));
    REQUIRE((c.x == 14 && c.y == 0));

    SECTION("Addition") {
        Vec<int> bb(-13, 100);
        Vec<int> r = b + bb;
        bb += Vec<int>(10, 15);
        
        REQUIRE((r.x == 160 && r.y == 29643));
        REQUIRE((bb.x == -3 && bb.y == 115));
    }

    SECTION("Subtraction") {
        Vecf aa(10.f, 0.2f);
        Vecf r = a - aa;
        aa -= Vecf(2.f, -.2f);

        REQUIRE((r.x == -7.f && r.y == 3.8f));
        REQUIRE((aa.x == 8.f && aa.y == .4f));
    }

    SECTION("Multiplication") {
        Vecb cc(4, 9);
        Vecb r = c * cc;
        cc *= Vecf(0, 2);

        REQUIRE((r.x == 56 && r.y == 0));
        REQUIRE(cc.x == 0);
        REQUIRE(cc.y == 18);
    }

    SECTION("Division") {
        Vecf aa(2.f, .25f);
        Vecf r = a / aa;
        aa /= Vecf(1.f, .5f);

        REQUIRE((r.x == 1.5f && r.y == 16.f));
        REQUIRE(aa.x == 2.f);
        REQUIRE(aa.y == .5f);
    }

    SECTION("Casting") {
        Vec<int> r = (Vec<int>) a;

        REQUIRE(r.x == 3);
        REQUIRE(r.y == 4);
    }

    SECTION("Dot Product") {
        Vec<int> l(5, 2);
        Vec<int> r(3, 4);
        Vec<int> rr(-2, 5);
        int d = Vec<int>::dotProduct(l, r);
        int dd = Vec<int>::dotProduct(l, rr);

        REQUIRE(d == 23);
        REQUIRE(dd == 0); 
    }
}