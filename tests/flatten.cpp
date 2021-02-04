#include "testing_header.hpp"

struct TestNode {
    std::vector<TestNode> children;
    int id;

    auto begin() const {
        return children.begin();
    }
    auto end() const {
        return children.end();
    }
};

TEST_CASE("avo::utils::flatten with nodes without parent function") {
    auto const tree = TestNode{
        .children = std::vector{
            TestNode{
                .children = std::vector{
                    TestNode{.id = 2},
                    TestNode{.id = 4},
                },
                .id = 3,
            },
            TestNode{
                .children = std::vector{
                    TestNode{
                        .children = std::vector{
                            TestNode{.id = -10},
                        },
                        .id = -3
                    },
                },
                .id = -5,
            },
            TestNode{
                .id = -1
            },
        },
        .id = 1,
    };

    auto const expected_ids = std::array{
        1, 3, 2, 4, -5, -3, -10, -1
    };

    CHECK(std::ranges::equal(avo::utils::flatten(tree), expected_ids, {}, &TestNode::id));
    CHECK(std::ranges::equal(tree | avo::utils::flatten, expected_ids, {}, &TestNode::id));
}

struct TestNodeWithParent {
    std::vector<TestNode> children;
    TestNodeWithParent* parent;
    int id;

    auto begin() {
        return children.begin();
    }
    auto begin() const {
        return children.begin();
    }
    auto end() {
        return children.end();
    }
    auto end() const {
        return children.end();
    }
};
