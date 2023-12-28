# Automatically find all executable files in the directory
file(GLOB_RECURSE EXECUTABLE_FILES "*.cpp")

foreach (file ${EXECUTABLE_FILES})
    get_filename_component(executable ${file} NAME_WE)
    add_executable(${executable} ${file})
    target_link_libraries(${executable} PRIVATE framework cli11)
endforeach ()
