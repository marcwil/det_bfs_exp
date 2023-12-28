# Automatically find all source files in the src directory
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.hpp")

# create a library of the framework
configure_file(src/paths.h.in src/paths.h)
add_library(framework SHARED ${SOURCE_FILES})
target_link_libraries(framework girgs)
target_include_directories(framework PUBLIC src ${CMAKE_CURRENT_BINARY_DIR}/src)
target_compile_options(framework PUBLIC
    -Wall -Wextra -pedantic
    -fsanitize=address,undefined
    -fno-omit-frame-pointer
    -fno-sanitize-recover=all
)
target_link_options(framework PUBLIC
    -fsanitize=address,undefined
    -fno-omit-frame-pointer
)
