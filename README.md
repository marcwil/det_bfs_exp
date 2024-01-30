# Deterministic BFS Experiments #

This repository contains the code for the experiments section of the paper **Deterministic Performance Guarantees for Bidirectional BFS on Real-World Networks**, [published at IWOCA 2023](https://link.springer.com/chapter/10.1007/978-3-031-34347-6_9).
See also the ArXiv version [here](https://arxiv.org/abs/2209.15300).

To reproduce the experiments, follow the following steps:
- you need to compile the C++ code as described below
- install the [run](https://github.com/thobl/run) python library
- download the instances using `scripts/download_instances.sh`
- execute the experiments, using `python experiment.py stats experiments merge`
- alternatively, to just see the experiments that can be run, simply run `python experiment.py`
  ``` console
  >>> python experiment.py
                                   todo   skipped     total
    gengraph
     ├─ girg_range                    0       180       180
     └─ girg_scale                    0       720       720
    stats
     ├─ stats                         0      1472      1472
     └─ degree                        0      1472      1472
    experiments
     ├─ bibfs_cost                    0      1472      1472
     ├─ expansion                     0      7360      7360
     └─ rho_expansion                 0      7360      7360
    merge
     ├─ merge_bfs                     1         0         1
     ├─ merge_degree                  1         0         1
     ├─ merge_expan                   1         0         1
     └─ merge_rho_expan               1         0         1
    clean
     ├─ remove_merged_bfs             1         0         1
     ├─ remove_merged_degree          1         0         1
     └─ remove_merged_expan           1         0         1
    
    running the experiments:
    time for gathering 20043 runs: 0.38 seconds
    time for running the experiments: 0.02 seconds
    
    
    WARNING: Did not try to run experiments as none were passed via the command line.
  ```
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
Test project /home/marcus/work/forschung/bibfs_parameters/det_bfs_exp/build
      Start  1: ExpansionTest.TestGraphRead
 1/16 Test  #1: ExpansionTest.TestGraphRead ..........   Passed    0.16 sec
      Start  2: ExpansionTest.CumulativeCost
 2/16 Test  #2: ExpansionTest.CumulativeCost .........   Passed    0.16 sec
      Start  3: ExpansionTest.Eccentricity
 3/16 Test  #3: ExpansionTest.Eccentricity ...........   Passed    4.04 sec
      Start  4: ExpansionTest.TestGetDist
 4/16 Test  #4: ExpansionTest.TestGetDist ............   Passed    3.00 sec
      Start  5: ExpansionTest.Cheap
 5/16 Test  #5: ExpansionTest.Cheap ..................   Passed    0.19 sec
      Start  6: ExpansionTest.Expan
 6/16 Test  #6: ExpansionTest.Expan ..................   Passed    0.19 sec
      Start  7: ExpansionTest.RelDist
 7/16 Test  #7: ExpansionTest.RelDist ................   Passed    3.02 sec
      Start  8: ExpansionTest.Sanity
 8/16 Test  #8: ExpansionTest.Sanity .................   Passed   46.66 sec
      Start  9: ExpansionTest.BasicRho
 9/16 Test  #9: ExpansionTest.BasicRho ...............   Passed    0.20 sec
      Start 10: ExpansionTest.RhoRandom
10/16 Test #10: ExpansionTest.RhoRandom ..............   Passed    1.49 sec
      Start 11: graph.basic_io
11/16 Test #11: graph.basic_io .......................   Passed    0.06 sec
      Start 12: graph.component_structure
12/16 Test #12: graph.component_structure ............   Passed    0.06 sec
      Start 13: graph.subgraph
13/16 Test #13: graph.subgraph .......................   Passed    0.04 sec
      Start 14: clustering_coeff.global_clustering
14/16 Test #14: clustering_coeff.global_clustering ...   Passed    0.05 sec
      Start 15: clustering_coeff.detour_distance
15/16 Test #15: clustering_coeff.detour_distance .....   Passed    0.36 sec
      Start 16: timer.wait
16/16 Test #16: timer.wait ...........................   Passed    0.14 sec

100% tests passed, 0 tests failed out of 16

Total Test time (real) =  59.85 sec
```


