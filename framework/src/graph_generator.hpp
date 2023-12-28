#pragma once

#include "graph.hpp"
#include "girgs/Generator.h"

struct GenOptions {
    int N;
    double ple;
    double deg;
    int dim;
    double T;
    bool torus;
};

class GraphGenerator {
   public:
    static Graph generateRandomGraph(const GenOptions &options);
};
