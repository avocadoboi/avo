#include <avo/id.hpp>

#include <catch.hpp>

TEST_CASE("avo::Id formatting") {
    REQUIRE(fmt::format("{}", avo::Id{91}) == "91");
    REQUIRE(fmt::format("{:>10}", avo::Id{91}) == "        91");
}
