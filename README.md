# Beating the Worst Case -- Framework #

This framework provides a very basic functionality for working with
graphs as well as a class for run time measurements.  It additionally
contains the [CLI11](https://github.com/CLIUtils/CLI11) library for
parsing command line options and
[gtest](https://github.com/google/googletest) for unit tests.

## Compiling and Running ##

Compile with CMake:

``` console
mkdir debug
cd debug/
cmake ..
make
```

This compiles in debug mode, building an executable with full debug
information.  To compile in release mode with full optimization,
replace `debug` with `release` in the above commands.

To see whether this worked, you can do a test run.  Still in the
`debug` directory, run the following command.

``` console
./run ../../instances/dolphins
```

It should produce an output similar to this:

``` console
number of nodes: 62
number ed edges: 159
time report:
total time:	0.68202 ms
load graph:	0.362121 ms
components:	0.030384 ms
subgraph:	0.287149 ms
```

## Graph Features ##

The framework contains a very basic graph data structure; see
[graph.hpp](include/framework/graph.hpp) for more details.  For
examples on how to load a graph from a file or how to create a
subgraph, see [run.cpp](cli/run.cpp).  For an example of how to use
the graph data structure, see
[component_structure.cpp](src/framework/component_structure.cpp).

In addition to the graph data structure itself, there is the feature
of computing the connected components of a graph; see
[component_structure.hpp](include/framework/component_structure.hpp)
for more details.  For an example of how to obtain the largest
connected component of a graph, see [run.cpp](cli/run.cpp).

## Supported Graph Formats ##

The following graph formats are supported. The loader should be able
to automatically figure out the format.

  * The graph format used by [konect.cc](https://konect.cc).  The page
    is unfortunately down though.

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
the project, then running `unit_tests` from the `debug` folder will
run the unit tests and output whether the tests succeeded or failed.
For an example of how to create test cases, see
[graph_tests.cpp](test/graph_tests.cpp).

## Adding Your Own Code ##

In addition to adding your header, source, main, and test files to
`include/`, `src/`, `cli/`, and `test/`, respectively, you have to
tell CMake to use them.  For this, edit the
[CMakeLists.txt](CMakeLists.txt) accordingly: Add header files to the
`set(HEADERS ...)`, add normal source files to `set(SOURCE_FILES
...)`, add main files (source files containing a `main()`) to
`set(EXECUTABLE_FILES ...)`, and add your test files to
`set(TEST_FILES ...)`.  Then just rerun `make` in the `debug` folder.

