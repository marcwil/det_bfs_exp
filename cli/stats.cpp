#include <iostream>
#include <vector>

#include "CLI11.hpp"
#include "framework/graph.hpp"
#include "framework/component_structure.hpp"
#include "framework/bibfs.hpp"
#include "framework/timer.hpp"
#include "framework/random.hpp"

double clustering_coefficient(const Graph &graph) {
  unsigned n = graph.n();
  unsigned m = graph.m();
  unsigned triangles = 0;
  unsigned total = 0;
  for (unsigned i = 0; i < n; i++) {
    for (unsigned j : graph.neighbors(i)) {
      if (j <= i) {
        continue;
      }
      for (unsigned k : graph.neighbors(i)) {
        // check if j and the k-th neighbor of i are connected
        if (j < k && std::find(graph.neighbors(j).begin(), graph.neighbors(j).end(), k) != graph.neighbors(j).end()) {
          triangles++;
        }
        total ++;
      }
    }
  }
  return 3 * triangles / (double) total;
}

int main(int argc, char** argv) {
  CLI::App app{"Beating the Worst Case -- BiBFS"};

  std::string file_name;
  app.add_option("input_file", file_name,
                 "Name of the file containing the graph")
      ->required()
      ->check(CLI::ExistingFile);

  bool no_header = false;
  app.add_flag("--noheader", no_header, "Do not print header");

  bool only_header = false;
  app.add_flag("--onlyheader", only_header, "Print only header");

  CLI11_PARSE(app, argc, argv);

  //file_name = "../../instances/small";
  //file_name = "../../instances/arenas-pgp";
  //file_name = CLI11_PARSE(app, argc, argv);

  if (!no_header) {
    std::cout << "n,m,maxdeg,avgdeg,deg_cov,clustering" << std::endl;
  }
  if (only_header) {
    return 0;
  }

  Graph G(file_name);
  ComponentStructure CS(G);
  
  Graph H(G, CS.nodes_of_component(CS.largest_component()));

  unsigned n = H.n();
  unsigned m = H.m();

  double avgdeg = 2.0 * m / n;

  //calculate maximum degree and coefficient of variation
  unsigned maxdeg = 0;
  unsigned deg_variance = 0;
  for (unsigned i = 0; i < n; i++) {
    unsigned deg = H.degree(i);
    if (deg > maxdeg) {
      maxdeg = deg;
    }
    deg_variance += (deg - avgdeg) * (deg - avgdeg);
  }
  double cov = sqrt(deg_variance / n) / avgdeg;

  double clust = clustering_coefficient(H);

  std::cout << n << "," << m << "," << maxdeg << "," << avgdeg << "," << cov << "," << clust << std::endl;

  return 0;
}
