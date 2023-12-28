#include "graph_generator.hpp"

#include "random.hpp"

Graph GraphGenerator::generateRandomGraph(const GenOptions& options) {
    const double T = options.T;
    const double alpha = T > 0 ? 1 / T : std::numeric_limits<double>::infinity();

    int wSeed = Random::natural_number(100000);
    int pSeed = Random::natural_number(100000);
    int sSeed = Random::natural_number(100000);

    auto girgWeights = girgs::generateWeights(options.N, options.ple, wSeed, false);
    auto girgPositions = girgs::generatePositions(options.N, options.dim, pSeed);
    girgs::scaleWeights(girgWeights, options.deg, options.dim, alpha);

    if (!options.torus) {
        // scale all positions with 0.5 to prevent wrapping of the torus
        for (auto& pos : girgPositions) {
            for (auto& coordinate : pos) {
                coordinate *= 0.5;
            }
        }
        // scale all weights to accommodate for the lower distances
        double factor = std::pow(0.5, options.dim);
        for (auto& weight : girgWeights) {
            weight *= factor;
        }
    }

    std::vector<std::pair<int, int>> tmp_edges = girgs::generateEdges(girgWeights, girgPositions, alpha, sSeed);
    // convert to unsigned ints
    std::vector<std::pair<unsigned, unsigned>> edges;
    for(auto e: tmp_edges){
        edges.push_back(std::make_pair(e.first, e.second));
    }
    Graph result(edges);
    return result;
}