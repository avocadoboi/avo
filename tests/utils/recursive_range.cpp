#include <avo/utils/recursive_range.hpp>

#include <catch.hpp>

//------------------------------

struct TestNode {
	std::vector<TestNode> children;
	int id;

	auto begin() const {
		return children.begin();
	}
	auto begin() {
		return children.begin();
	}
	auto end() const {
		return children.end();
	}
	auto end() {
		return children.end();
	}
};
static_assert(avo::utils::IsRecursiveRange<TestNode, false>);

struct TestNodeWithParent {
	std::vector<TestNodeWithParent> children;
	TestNodeWithParent* parent;
	int id;

	auto begin() const {
		return children.begin();
	}
	auto begin() {
		return children.begin();
	}
	auto end() const {
		return children.end();
	}
	auto end() {
		return children.end();
	}
};
static_assert(avo::utils::IsRecursiveRange<TestNodeWithParent, true>);

//------------------------------

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

//------------------------------

std::pair<TestNode, std::vector<int>> construct_test_without_parent_nodes() {
	// TODO: add designated initializers when MSVC doesn't crash.
	
	auto tree = TestNode{
		std::vector{
			TestNode{
				std::vector{
					TestNode{{}, 2},
					TestNode{{}, 4},
				},
				3,
			},
			TestNode{
				std::vector{
					TestNode{
						std::vector{
							TestNode{{}, -10},
						},
						-3,
					},
				},
				-5,
			},
			TestNode{{}, -1},
		},
		1,
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

//------------------------------

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

//------------------------------

TEST_CASE("avo::utils::view_parents") {
	auto root = TestNodeWithParent{};
	auto child_0 = TestNodeWithParent{.parent = &root};
	auto child_1 = TestNodeWithParent{.parent = &child_0};
	auto child_2 = TestNodeWithParent{.parent = &child_1};

	auto const parent_range = avo::utils::view_parents(child_2);
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
