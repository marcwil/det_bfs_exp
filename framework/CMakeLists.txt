
# header, source and executable files
set(HEADERS
    include/framework/graph.hpp
    include/framework/graph_parser.hpp
    include/framework/graph_writer.hpp
    include/framework/component_structure.hpp
    include/framework/timer.hpp
    include/framework/random.hpp
)

set(SOURCE_FILES
    src/graph.cpp
    src/graph_parser.cpp
    src/graph_writer.cpp
    src/component_structure.cpp
    src/timer.cpp
    src/random.cpp
)


# create a target for every executable linking all source files
configure_file(include/framework/paths.h.in include/framework/paths.h)
add_library(framework SHARED ${HEADERS} ${SOURCE_FILES})
target_include_directories(framework PUBLIC include lib ${CMAKE_CURRENT_BINARY_DIR}/include)
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

