#include "testing_header.hpp"

template<typename _Node> requires 
    std::same_as<decltype(_Node::children), std::vector<_Node>> &&
    std::same_as<decltype(_Node::id), int> 
void test_flatten_with_node_type(_Node& tree, std::vector<int> const& expected_ids) {
    auto const& const_tree = tree;
    
    CHECK(std::ranges::equal(avo::utils::flatten(const_tree), expected_ids, {}, &_Node::id));
    CHECK(std::ranges::equal(const_tree | avo::utils::flatten, expected_ids, {}, &_Node::id));

    CHECK(std::ranges::equal(avo::utils::flatten(tree), expected_ids, {}, &_Node::id));
    CHECK(std::ranges::equal(tree | avo::utils::flatten, expected_ids, {}, &_Node::id));

    CHECK(std::ranges::equal(
        tree | avo::utils::flatten | std::views::transform([](_Node& n){ return n.id*2; }), 
        expected_ids | std::views::transform([](int const id){ return id*2; })
    ));

    for (auto& node : tree | avo::utils::flatten) {
        node.id = 2;
    }
    CHECK(std::ranges::all_of(tree | avo::utils::flatten, [](int id){ return id == 2; }, &_Node::id));
}

std::pair<TestNode, std::vector<int>> construct_test_without_parent_nodes() {
    auto tree = TestNode{
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

    return {
        std::move(tree),
        std::vector{1, 3, 2, 4, -5, -3, -10, -1}
    };
}

TEST_CASE("avo::utils::flatten with nodes without parents") {
    auto [tree, expected_ids] = construct_test_without_parent_nodes();
    test_flatten_with_node_type(tree, expected_ids);
}

std::pair<std::unique_ptr<TestNodeWithParent>, std::vector<int>> construct_test_with_parent_nodes() {
    auto root = std::unique_ptr<TestNodeWithParent>{new TestNodeWithParent{.id=1}};

    root->children.insert(root->end(), {
        TestNodeWithParent{.parent = root.get(), .id = 2}, 
        TestNodeWithParent{.parent = root.get(), .id = 3}, 
        TestNodeWithParent{.parent = root.get(), .id = 4}
    });

    auto& child_0 = root->children[0];
    child_0.children.insert(child_0.end(), {
        TestNodeWithParent{.parent = &child_0, .id = 5}, 
        TestNodeWithParent{.parent = &child_0, .id = 6}
    });

    auto& child_0_0 = child_0.children[0];
    child_0_0.children.insert(child_0_0.end(), {
        TestNodeWithParent{.parent = &child_0_0, .id = 7}, 
        TestNodeWithParent{.parent = &child_0_0, .id = 8}
    });

    auto& child_2 = root->children[2];
    child_2.children.insert(child_2.end(), {
        TestNodeWithParent{.parent = &child_2, .id = 9}, 
        TestNodeWithParent{.parent = &child_2, .id = 10}, 
        TestNodeWithParent{.parent = &child_2, .id = 11}
    });

    auto& child_2_2 = child_2.children[2];
    child_2_2.children.push_back(TestNodeWithParent{.parent = &child_2_2, .id = 12});

    return {
        std::move(root),
        std::vector{1, 2, 5, 7, 8, 6, 3, 4, 9, 10, 11, 12}
    };
}

TEST_CASE("avo::utils::flatten with nodes with stored parent") {
    auto [tree, expected_ids] = construct_test_with_parent_nodes();
    test_flatten_with_node_type(*tree, expected_ids);
}
