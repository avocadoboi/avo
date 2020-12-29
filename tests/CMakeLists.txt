add_definitions(-DBUILD_TESTING=${BUILD_TESTING})

file(GLOB TEST_SOURCES *.cpp)

add_executable(avogui_test ${TEST_SOURCES})
target_include_directories(avogui_test PRIVATE "../external/catch2/")
target_link_libraries(avogui_test PRIVATE avogui)

add_test(NAME unit_tests COMMAND avogui_test)
