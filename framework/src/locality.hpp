#pragma once

#include "graph.hpp"

struct global_cluster_info {
    int num_triplets;
    int num_closed_triplets;
    double global_clustering_coeff;
};

struct average_distance_info {
    double average_distance;
    double average_detour;
    double distance_locality;
};

double average_local_clustering(const Graph& G);

global_cluster_info global_clustering(const Graph& G);

average_distance_info average_distance_locality(const Graph& G, int num_samples);
