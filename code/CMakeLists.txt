cmake_minimum_required(VERSION 3.10)

project(BeatingTheWorstCase)

# setting the build type to debug when in the debug directory and to
# release otherwise
get_filename_component(build_dir ${CMAKE_CURRENT_BINARY_DIR} NAME)
if (${build_dir} MATCHES "debug")
  set(CMAKE_BUILD_TYPE Debug)
else()
  set(CMAKE_BUILD_TYPE Release)
endif()

# c++ version 17
set(CMAKE_CXX_STANDARD 17)

# export compile_commands.json, which can be used by editors for auto
# completion (e.g., using lsp)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# show all warnings
if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# helping the compiler to find header files
include_directories(include lib)

# header, source and executable files
set(HEADERS
  lib/CLI11.hpp
  include/framework/graph.hpp
  include/framework/graph_parser.hpp
  include/framework/graph_writer.hpp
  include/framework/component_structure.hpp
  include/framework/timer.hpp
  include/framework/random.hpp
  )

set(SOURCE_FILES
  src/framework/graph.cpp
  src/framework/graph_parser.cpp
  src/framework/graph_writer.cpp
  src/framework/component_structure.cpp
  src/framework/timer.cpp
  src/framework/random.cpp
  )

set(EXECUTABLE_FILES
  cli/run.cpp
  cli/convert_to_edgelist.cpp
  )

# create a target for every executable linking all source files
add_library(code STATIC ${SOURCE_FILES} ${HEADERS})
foreach (file ${EXECUTABLE_FILES})
    get_filename_component(executable ${file} NAME_WE)
    add_executable(${executable} ${file})
    target_link_libraries(${executable} code)
endforeach ()

# unit tests using gtest
enable_testing()
set(GOOGLETEST_VERSION 1.10.0)
add_subdirectory(lib/googletest)

set(TEST_FILES
  test/graph_tests.cpp
  test/timer_tests.cpp
  test/main.cpp
  )

add_executable(unit_tests ${TEST_FILES})
add_test(NAME unit_tests COMMAND unit_tests)
target_link_libraries(unit_tests code gtest)
