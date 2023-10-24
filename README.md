# Beating the Worst Case -- Framework #

This small framework serves as a starting point for the practical
course _"Beating the Worst Case in Practice: Unerwartet effiziente
Algorithmen"_.  As participant, you probably want to fork this
repository. As the repository contains submodules, you want to clone it with the `--recurse` flag.

``` console
git clone --recurse git@link_to_your_repository
```

The core of the framework is in [framework](framework/).
The [ext_libs](ext_libs/) folder contains the [CLI11](https://github.com/CLIUtils/CLI11) library for parsing command line options and the [GIRG](https://github.com/chistopher/girgs) library for generating graphs.
Unit test are located in [tests](tests/) and use [gtest](https://github.com/google/googletest).
Moreover, [instances](./instances) includes some small instances for initial
testing.  The folder [evaluation](./evaluation) contains example data and an [R script](https://en.wikipedia.org/wiki/R_(programming_language)) to visualize it.
You can use this directory to creat your own plots.


## Using the Framework ##

The repository contains a [.devcontainer](.devcontainer/devcontainer.json). If you have [Docker](https://www.docker.com/) installed, [VSCode](https://code.visualstudio.com/) can set up a fully working development environment on your platform inside a Docker container.
This can be especially useful for students working on windows.

If you don't want to use a Docker, you can build the project on your own using CMake.


## Compiling and Running ##

Compile with CMake:

``` console
mkdir debug
cd debug/
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j4
```

This compiles in debug mode, building an executable with full debug
information.  To compile in release mode with full optimization,
create a new folder called `release` and replace `Debug` with `Release` 
in the above commands.

``` console
mkdir release
cd release/
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j4
```

To see whether this worked, you can do a test run.  Still in the
`debug` directory, run the following command.

``` console
make test
```

It should produce an output similar to this:

``` console
Running tests...
Test project /home/jeanp/Documents/teaching/praktikum_beating_the_worst_case_framework/debug
    Start 1: graph.basic_io
1/4 Test #1: graph.basic_io ...................   Passed    0.01 sec
    Start 2: graph.component_structure
2/4 Test #2: graph.component_structure ........   Passed    0.01 sec
    Start 3: graph.subgraph
3/4 Test #3: graph.subgraph ...................   Passed    0.01 sec
    Start 4: timer.wait
4/4 Test #4: timer.wait .......................   Passed    0.10 sec

100% tests passed, 0 tests failed out of 4

Total Test time (real) =   0.14 sec
```

## Graph Features ##

The framework contains a very basic graph data structure; see
[graph.hpp](framework/include/framework/graph.hpp) for more details.  For
examples on how to load a graph from a file or how to create a
subgraph, see [run.cpp](cli/run.cpp).  For an example of how to use
the graph data structure, see
[component_structure.cpp](framework/src/component_structure.cpp).

In addition to the graph data structure itself, there is the feature
of computing the connected components of a graph; see
[component_structure.hpp](include/framework/component_structure.hpp)
for more details.  For an example of how to obtain the largest
connected component of a graph, see [run.cpp](cli/run.cpp).

## Supported Graph Formats ##

The following graph formats are supported. The loader should be able
to automatically figure out the format.

  * The graph format used by [konect.cc](http://konect.cc/networks/).  
    (TODO: verify if this still works)

  * The mtx format used by
    [networkrepository.com](https://networkrepository.com). Note that
    some graphs on
    [networkrepository.com](https://networkrepository.com) use a
    different format, which probably does not work.  There are plenty
    of graphs in the mtx format though.

  * A basic edge list with two integers per row.  Each integer
    represents a vertex and each row represents an edge between the
    two corresponding vertices.
	
The command line interface
[convert_to_edgelist.cpp](cli/convert_to_edgelist.cpp) reads a given
file and writes it as a basic edge list.

## Run Time Measurements ##

The `Timer` class provides the functionality of having globally
available named timers.  See [timer.hpp](include/framework/timer.hpp)
for more details, and [run.cpp](cli/run.cpp) for an example of how to
use it.

## Parsing Command Line Arguments ##

[CLI11](https://github.com/CLIUtils/CALI11) is used for parsing
command line arguments.  It also automatically creates a help output;
for an example, run `./run --help`.  For a very basic example on how
to use it, see [run.cpp](cli/run.cpp).  Also see the [CLI11 GitHub
page](https://github.com/CLIUtils/CLI11#usage) for additional
information on how to use it.

## Running Unit Tests ##

[gtest](https://github.com/google/googletest) is already included in
the framework, providing easy unit testing.  If you already compiled
the project, then running `make test` from the `debug` folder will
run the unit tests and output whether the tests succeeded or failed.
For an example of how to create test cases, see
[graph_tests.cpp](test/graph_tests.cpp).

## Adding Your Own Code ##

In addition to adding your header, source, main, and test files, you have to
tell CMake to use them.  For this, edit the
[CMakeLists.txt](CMakeLists.txt) files accordingly: 
  * if you want to add your own command line application add a `.cpp`
    file to the folder `cli/` and to `set(EXECUTABLE_FILES ...)` in the [cli/CMakeLists.txt](cli/CMakeLists.txt)
  * if you want to modify the framework, add header and source files to
    `framework/include/framework` and `framework/src/` and add them to
    `set(HEADERS ...)` and `set(SOURCE_FILES ...)` in [framework/CMakeLists.txt](framework/CMakeLists.txt)
  * if you want to add unit test, add them to `test/` and set them in
    `set(TEST_FILES ...)` in [test/CMakeLists.txt](test/CMakeLists.txt).

