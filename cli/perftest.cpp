#include <iostream>

#include "CLI11.hpp"
#include "graph.hpp"
#include "component_structure.hpp"
#include "bibfs.hpp"
#include "timer.hpp"
#include "random.hpp"

int main(int argc, char** argv) {
  CLI::App app{"Beating the Worst Case -- PerfTest"};

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

  
  bool debug_inputs = false;
  if (debug_inputs) {
    file_name = "../../instances/arenas-pgp";
    samples = 10;
    no_header = false;
    only_header = false;
  } else {
    CLI11_PARSE(app, argc, argv);
  }
  //CLI11_PARSE(app, argc, argv);

  //file_name = "../../instances/small";
  //file_name = "../../instances/arenas-pgp";
  //file_name = CLI11_PARSE(app, argc, argv);

  if (!no_header) {
    std::cout << "dist,cost,time" << std::endl;
  }
  if (only_header) {
    return 0;
  }

  Graph G(file_name);
  ComponentStructure CS(G);
  Graph H(G, CS.nodes_of_component(CS.largest_component()));

  PerfBiBFS bibfs(H);

  for(unsigned int i=0; i<samples; i++){
    unsigned s = 0;
    unsigned t = 0;
    while (s == t) {
      s = Random::natural_number(H.n()-1);
      t = Random::natural_number(H.n()-1);
    }
    auto [bi_dist, bi_cost, bi_time] = bibfs.query(s, t);
    //auto [bi_dist2, bi_cost2] = biBFS::search(H, s, t);
    //if (bi_dist != bi_dist2) {
    //  std::cout << "ERROR: bi_dist != bi_dist2" << std::endl;
    //  return 1;
    //}
    std::cout << bi_dist << "," << bi_cost << "," << bi_time << std::endl;
  }
 
  return 0;
}
