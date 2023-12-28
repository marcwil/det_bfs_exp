#include "locality.hpp"

#include <numeric>
#include <vector>

#include "bibfs.hpp"
#include "random.hpp"

double average_local_clustering(const Graph& G) {
    std::vector<double> loc_clust(G.n());
    std::vector<bool> marked(G.n(), false);
    unsigned nr_vertices = 0;
    for (node u = 0; u < G.n(); ++u) {
        // skip degree 1 vertices
        if (G.degree(u) == 1) {
            continue;
        }
        nr_vertices++;

        // mark neighbors of u
        for (node v : G.neighbors(u)) {
            marked[v] = true;
        }

        // count edges between neighbors
        unsigned nr_edges = 0;
        for (node v : G.neighbors(u)) {
            for (node w : G.neighbors(v)) {
                if (marked[w] && v < w) {
                    nr_edges++;
                }
            }
        }

        // unmark neighbors
        for (node v : G.neighbors(u)) {
            marked[v] = false;
        }

        // local clustering
        unsigned deg = G.degree(u);
        unsigned potential_edges = deg * (deg - 1) / 2;
        loc_clust[u] = (double)nr_edges / potential_edges;
    }

    // average
    return std::accumulate(loc_clust.begin(), loc_clust.end(), 0.0) / nr_vertices;
}

global_cluster_info global_clustering(const Graph& G) {
    int num_triplets = 0;
    int num_closed_triplets = 0;

    global_cluster_info result;

    for (node u = 0; u < G.n(); u++) {
        // mark neighbors of u
        for (node v : G.neighbors(u)) {
            for (node w : G.neighbors(v)) {
                if (w == u) continue;
                num_triplets++;
                for (node x : G.neighbors(w)) {
                    if (x == u) num_closed_triplets++;
                }
            }
        }
    }

    result.num_triplets = num_triplets / 2;
    result.num_closed_triplets = num_closed_triplets / 2;
    result.global_clustering_coeff = (double)result.num_closed_triplets / (double)result.num_triplets;

    return result;
}

average_distance_info average_distance_locality(const Graph& G, int num_samples) {
    // calculate average distance
    double average_distance = 0;

    for (int i = 0; i < num_samples; i++) {
        node s = Random::natural_number(G.n() - 1);
        node t = Random::natural_number(G.n() - 1);

        if (!G.has_edge(s, t)) {
            // found a non edge
            auto [dist, cost] = biBFS::search(G, s, t);
            average_distance += dist;  
        } else {
            i--;  // retry
        }
    }
    average_distance /= (double)num_samples;

    // calculate detour distances
    double average_detour_distance = 0;

    unsigned num_retries = 0;
    unsigned max_retries = num_samples;
    for (int i = 0; i < num_samples; i++) {
        node random_node = Random::natural_number(G.n() - 1);
        int random_neighbor_idx = Random::natural_number(G.degree(random_node) - 1);  // graph connected -> at least one neighbor
        node random_neighbor = G.neighbors(random_node)[random_neighbor_idx];

        auto [dist, cost] = biBFS::search(G, random_node, random_neighbor, true);
        if (dist == -1) {
            i--; // bridge -> retry
            num_retries++;
            // if too many retries -> return locality 0
            if (num_retries > max_retries) {
                average_distance_info res;
                res.average_distance = average_distance;
                res.average_detour = G.n();
                res.distance_locality = 0.0;
                return res;
            }
        } else {
            average_detour_distance += dist;
        }
    }
    average_detour_distance /= num_samples;

    // TODO handle bridges -> currently treated as distance n
    average_distance_info result;
    result.average_distance = average_distance;
    result.average_detour = average_detour_distance;
    if (average_distance - 2.0 <= 0.0) {
        result.distance_locality = 0.0;
    } else {
        result.distance_locality = std::max(0.0, 1.0 - (average_detour_distance - 2.0) / (average_distance - 2.0));
    }

    return result;
}
