#include "testing_header.hpp"

TEST_CASE("avo::utils::parents") {
    auto root = TestNodeWithParent{};
    auto child_0 = TestNodeWithParent{.parent = &root};
    auto child_1 = TestNodeWithParent{.parent = &child_0};
    auto child_2 = TestNodeWithParent{.parent = &child_1};

    auto const parent_range = avo::utils::parents(child_2);
    auto iterator = parent_range.begin();
    REQUIRE(*iterator == &child_1);
    REQUIRE(*iterator == &child_1);
    REQUIRE(iterator != parent_range.end());
    ++iterator;
    REQUIRE(*iterator == &child_0);
    REQUIRE(*iterator == &child_0);
    REQUIRE(iterator != parent_range.end());
    ++iterator;
    REQUIRE(*iterator == &root);
    REQUIRE(*iterator == &root);
    REQUIRE(iterator != parent_range.end());
    ++iterator;
    REQUIRE(iterator == parent_range.end());
}
