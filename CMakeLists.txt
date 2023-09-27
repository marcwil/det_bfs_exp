cmake_minimum_required(VERSION 3.20)

project(BeatingTheWorstCase)

# c++ version 17
set(CMAKE_CXX_STANDARD 17)

# export compile_commands.json, which can be used by editors for auto
# completion (e.g., using lsp)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

add_subdirectory(ext_libs)
add_subdirectory(framework)
add_subdirectory(cli)
add_subdirectory(test)

