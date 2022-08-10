add_definitions(-DBUILD_TESTING=${BUILD_TESTING})

file(GLOB_RECURSE TEST_SOURCES *.cpp)

add_executable(avogui_test ${TEST_SOURCES})

target_link_libraries(avogui_test PRIVATE avo)

find_package(Catch2 CONFIG REQUIRED)
target_link_libraries(avogui_test PRIVATE Catch2::Catch2WithMain)

add_test(NAME unit_tests COMMAND avogui_test)
