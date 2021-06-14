
add_executable(window_test window_test.cpp)
target_link_libraries(window_test avo)
target_include_directories(window_test PRIVATE "../external/magic_enum/")

add_executable(key_state_test key_state_test.cpp)
target_link_libraries(key_state_test avo)

add_executable(atomic_flag_deadlock atomic_flag_deadlock.cpp)
target_link_libraries(atomic_flag_deadlock avo)
