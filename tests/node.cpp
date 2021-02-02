#include "testing_header.hpp"

class App {
public:

private:
    avo::Node _node{this};
};

TEST_CASE("Nodes with IDs") {
    auto root = avo::Node{};

    root.add_child();
}
