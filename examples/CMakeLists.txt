
add_executable(window_test window_test.cpp)
target_link_libraries(window_test avo)
target_include_directories(window_test PRIVATE "../external/magic_enum/")

add_executable(basic_graphics basic_graphics.cpp)
target_link_libraries(basic_graphics PRIVATE avo)

add_executable(key_state_test key_state_test.cpp)
target_link_libraries(key_state_test PRIVATE avo)
