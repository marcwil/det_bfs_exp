include(GoogleTest)

# Automatically find all test files in the directory
file(GLOB TEST_FILES "*.cpp")

add_executable(unit_tests ${TEST_FILES})
target_link_libraries(unit_tests framework gtest_main)
gtest_discover_tests(unit_tests)
