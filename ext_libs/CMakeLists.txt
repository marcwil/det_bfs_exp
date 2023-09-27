
# disable some components of GTest we don't need
set(gtest_build_samples     OFF CACHE BOOL "")
set(gtest_build_tests       OFF CACHE BOOL "")
set(gmock_build_tests       OFF CACHE BOOL "")
set(BUILD_GMOCK             OFF CACHE BOOL "")
set(INSTALL_GTEST           OFF CACHE BOOL "")
add_subdirectory(googletest)

add_subdirectory(girgs)
target_compile_options(girgs PRIVATE -Wno-sign-compare -Wno-unused-function)
target_compile_options(hypergirgs PRIVATE -Wno-sign-compare)
target_compile_options(genhrg PRIVATE -Wno-sign-compare)

add_library(cli11 INTERFACE)
target_include_directories(cli11 INTERFACE cli11)

