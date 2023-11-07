#include <iostream>

#include "CLI11.hpp"
#include "framework/graph.hpp"
#include "framework/component_structure.hpp"
#include "framework/bibfs.hpp"
#include "framework/timer.hpp"
#include "framework/random.hpp"

int main(int argc, char** argv) {
  CLI::App app{"Beating the Worst Case -- BiBFS"};

  std::string file_name;
  app.add_option("input_file", file_name,
                 "Name of the file containing the graph")
      ->required()
      ->check(CLI::ExistingFile);

  unsigned samples;
  app.add_option("-s,--samples", samples, "Number of samples")
      ->default_val(100);

  bool no_header = false;
  app.add_flag("--noheader", no_header, "Do not print header");

  bool only_header = false;
  app.add_flag("--onlyheader", only_header, "Print only header");

  CLI11_PARSE(app, argc, argv);

  //file_name = "../../instances/small";
  //file_name = "../../instances/arenas-pgp";
  //file_name = CLI11_PARSE(app, argc, argv);

  if (!no_header) {
    std::cout << "dist,cost_uni,cost_bi" << std::endl;
  }
  if (only_header) {
    return 0;
  }

  Graph G(file_name);
  ComponentStructure CS(G);
  
  Graph H(G, CS.nodes_of_component(CS.largest_component()));

  for(unsigned int i=0; i<samples; i++){
    unsigned s = 0;
    unsigned t = 0;
    while (s == t) {
      s = Random::natural_number(H.n()-1);
      t = Random::natural_number(H.n()-1);
    }
    auto [uni_dist, uni_cost] = BFS::search(H, s, t);
    auto [bi_dist, bi_cost] = biBFS::search(H, s, t);
    if (uni_dist != bi_dist) {
      std::cout << "ERROR: uni_dist != bi_dist" << std::endl;
      return 1;
    }
    std::cout << bi_dist << "," << uni_cost << "," << bi_cost << std::endl;
  }

  return 0;
}
