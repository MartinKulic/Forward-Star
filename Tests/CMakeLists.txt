find_package(Catch2 3 REQUIRED)
add_executable(tests_run Simple_insert_test.cpp
        Test_node.hpp)

target_link_libraries(tests_run PRIVATE Forward_Star)
target_link_libraries(tests_run PRIVATE Catch2::Catch2WithMain)

include(Catch)
catch_discover_tests(tests_run)
