
set(EXECUTABLE_FILES
    run.cpp
    convert_to_edgelist.cpp
    graph_generator.cpp
    bfs.cpp
    stats.cpp
)

foreach (file ${EXECUTABLE_FILES})
    get_filename_component(executable ${file} NAME_WE)
    add_executable(${executable} ${file})
    target_link_libraries(${executable} PRIVATE framework cli11)
endforeach ()

