#include "testing_header.hpp"

class SomeComponent {
public:
    int value() const noexcept {
        return _value;
    }
    avo::Node& get_node() noexcept {
        return _node;
    }

    SomeComponent(avo::Node& parent, avo::Id const id, int const value) :
        _node{parent, id, *this},
        _value{value}
    {}

private:
    avo::Node _node;
    int _value;
};

class App {
public:
    App() : _node{*this}
    {
        _other_components.emplace_back(_component_0.get_node(), avo::Id{3}, 10);
        _other_components.emplace_back(_component_0.get_node(), avo::Id{4}, 11);
        _other_components.emplace_back(_component_1.get_node(), avo::Id{4}, 12);
        _other_components.emplace_back(_component_1.get_node(), avo::Id{5}, 13);
    }

    avo::Node const& get_node() const noexcept {
        return _node;
    }

private:
    avo::Node _node;

    std::vector<SomeComponent> _other_components;
    SomeComponent _component_0{_node, avo::Id{1}, 3};
    SomeComponent _component_1{_node, avo::Id{2}, 8};
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
    REQUIRE(avo::find_component_by_id<SomeComponent>(app.get_node(), avo::Id{1}).value() == 3);
    REQUIRE(avo::find_component_by_id<SomeComponent>(app.get_node(), avo::Id{5}).value() == 13);
    REQUIRE(app.get_node()[0].component<SomeComponent>().value() == 3);
    REQUIRE(app.get_node()[1].component<SomeComponent>().value() == 8);
}
