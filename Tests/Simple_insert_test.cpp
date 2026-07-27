//
// Created by martin on 18. 7. 2026.
//
#include <catch2/catch_test_macros.hpp>
#include "fStar.hpp" // You can now include your library directly!
#include "Test_node.hpp"

TEST_CASE("Library sanity check") {

    Test_Node t(0);

    FStar<Test_Node> star;

    REQUIRE(true);
}