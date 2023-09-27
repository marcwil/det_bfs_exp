
include(GoogleTest)

set(TEST_FILES
  graph_tests.cpp
  timer_tests.cpp
  )

add_executable(unit_tests ${TEST_FILES})
target_link_libraries(unit_tests framework gtest_main)
gtest_add_tests(TARGET unit_tests)