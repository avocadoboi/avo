#include <avo/node.hpp>

#include <catch.hpp>

class SomeComponent {
public:
	int value() const noexcept {
		return value_;
	}
	avo::Node& get_node() noexcept {
		return node_;
	}

	SomeComponent(avo::Node& parent, avo::Id const id, int const value) :
		node_{parent, id, *this},
		value_{value}
	{}

private:
	avo::Node node_;
	int value_;
};

class App {
public:
	App() : node_{*this}
	{
		other_components_.emplace_back(component_0_.get_node(), avo::Id{3}, 10);
		other_components_.emplace_back(component_0_.get_node(), avo::Id{4}, 11);
		other_components_.emplace_back(component_1_.get_node(), avo::Id{4}, 12);
		other_components_.emplace_back(component_1_.get_node(), avo::Id{5}, 13);
	}

	avo::Node const& get_node() const noexcept {
		return node_;
	}
	avo::Node& get_node() noexcept {
		return node_;
	}

private:
	avo::Node node_;

	std::vector<SomeComponent> other_components_;
	SomeComponent component_0_{node_, avo::Id{1}, 3};
	SomeComponent component_1_{node_, avo::Id{2}, 8};
};

TEST_CASE("Nodes with IDs") {
	auto const app = App{};

	REQUIRE(app.get_node().size() == 2);

	auto const ids = std::array{
		avo::Id{},
		avo::Id{1},
		avo::Id{3},
		avo::Id{4},
		avo::Id{2},
		avo::Id{4},
		avo::Id{5},
	};

	constexpr auto id_from_node = [](avo::Node const& node){ return node.id(); };
	REQUIRE(std::ranges::equal(app.get_node(), std::array{avo::Id{1}, avo::Id{2}}, {}, id_from_node));
	REQUIRE(std::ranges::equal(app.get_node() | avo::utils::flatten, ids, {}, id_from_node));

	REQUIRE(app.get_node().find_by_id(avo::Id{4})->id() == avo::Id{4});
	REQUIRE(std::ranges::distance(app.get_node().find_all_by_id(avo::Id{4})) == 2);

	// Note: cannot be const because of the constant-time amortization for begin() on filter views.
	auto found_components = avo::find_components_by_id<SomeComponent>(app.get_node(), avo::Id{4});
	REQUIRE(std::ranges::distance(found_components) == 2);
	REQUIRE(std::ranges::equal(
		std::array{11, 12}, 
		std::views::transform(found_components, [](auto& component) {
			return component.value();
		})
	));
	
	REQUIRE(avo::find_component_by_id<SomeComponent>(app.get_node(), avo::Id{1})->value() == 3);
	REQUIRE(avo::find_component_by_id<SomeComponent>(app.get_node(), avo::Id{5})->value() == 13);

	REQUIRE(avo::find_component_by_id<SomeComponent>(app.get_node(), avo::Id{11}) == nullptr);

	REQUIRE(app.get_node()[0].component<SomeComponent>()->value() == 3);
	REQUIRE(app.get_node()[1].component<SomeComponent>()->value() == 8);
}

