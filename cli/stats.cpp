#include <iostream>
#include <vector>

#include "CLI11.hpp"
#include "graph.hpp"
#include "component_structure.hpp"
#include "locality.hpp"
#include "timer.hpp"
#include "random.hpp"


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
    std::cout << "n,m,maxdeg,avgdeg,deg_cov,local_clustering,global_clustering,distance_locality" << std::endl;
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

  double local_clustering = average_local_clustering(H);

  auto global_cluster_info = global_clustering(H);

  auto average_distance_info = average_distance_locality(H, 100);

  std::cout << n << "," << m << "," << maxdeg << "," << avgdeg << "," << cov << ","
        << local_clustering << ","
        << global_cluster_info.global_clustering_coeff << ","
        << average_distance_info.distance_locality << std::endl;

  return 0;
}
