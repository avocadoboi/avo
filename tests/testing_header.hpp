#include <catch.hpp>

#include <Avo.hpp>

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
