add_definitions(-DBUILD_TESTING=${BUILD_TESTING})

file(GLOB_RECURSE TEST_SOURCES *.cpp)

add_executable(avogui_test ${TEST_SOURCES})
target_include_directories(avogui_test PRIVATE "../external/catch2/")
# target_precompile_headers(avogui_test PRIVATE <catch.hpp>)
target_link_libraries(avogui_test PRIVATE avo)

add_test(NAME unit_tests COMMAND avogui_test)
