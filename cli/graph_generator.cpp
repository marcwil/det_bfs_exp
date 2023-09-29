#include <iostream>
#include <fstream>

#include "CLI11.hpp"
#include "framework/graph_generator.hpp"
#include "framework/component_structure.hpp"
#include "framework/graph_writer.hpp"
#include "framework/timer.hpp"

int main(int argc, char** argv) {
  CLI::App app{"GIRG generator"};

  std::string file_name;
  app.add_option("-o", file_name,
                 "Name of the file containing the graph.")
      ->required();
  int num_nodes;
  app.add_option("-n", num_nodes, "Number of nodes in the generated graph")
      ->required();

  CLI11_PARSE(app, argc, argv);
    
  // create option struct for graph generation
  GenOptions options {
    num_nodes,
    2.8,
    15,
    2,
    0.1,
    false,
  };

  Graph G = GraphGenerator::generateRandomGraph(options);
  ComponentStructure CS(G);
  Graph H(G, CS.nodes_of_component(CS.largest_component()));

  std::ofstream output;
  output.open(file_name);
  write_edge_list(H, output);

  std::cout << "number of nodes: " << H.n() << std::endl;
  std::cout << "number ed edges: " << H.m() << std::endl;
  return 0;
}
