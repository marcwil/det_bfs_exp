# Deterministic BFS Experiments #

This repository contains the code for the experiments section of the paper **Deterministic Performance Guarantees for Bidirectional BFS on Real-World Networks**, [published at IWOCA 2023](https://link.springer.com/chapter/10.1007/978-3-031-34347-6_9).
See also the ArXiv version [here](https://arxiv.org/abs/2209.15300).

To reproduce the experiments, follow the following steps:
- you need to compile the C++ code as described below
- install the [run library](https://github.com/thobl/run)
- execute the experiments, using `python experiment.py stats experiments merge`
- alternatively, to just see the experiments that can be run, simply run `python experiment.py`
- go to `evaluation` and run `plot_for_paper.R` to create the plots in the paper (and a few more)

To also run experiments on generated graphs (not included in the paper, as the results don't give additional insights) run `python experiments.py gen_graph`.

## Docker and .devcontainer ##

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


