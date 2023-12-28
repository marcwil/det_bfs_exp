#include <iostream>
#include <fstream>

#include "CLI11.hpp"
#include "graph_generator.hpp"
#include "component_structure.hpp"
#include "graph_writer.hpp"
#include "timer.hpp"

int main(int argc, char** argv) {
  CLI::App app{"GIRG generator"};

  std::string file_name;
  app.add_option("-o", file_name,
                 "Name of the output file for the graph.")
      ->required();
  int num_nodes;
  app.add_option("-n", num_nodes, "Number of nodes in the generated graph")
      ->required();

  // options for graph generation (see graph_generator.hpp)
  double powerlaw_exponent;
  app.add_option("--ple", powerlaw_exponent,
                 "Powerlaw exponent of the degree distribution")
      ->default_val(2.5);

  double degree;
  app.add_option("--deg", degree, "Average degree of the generated graph")
      ->default_val(10);
    
  int dimension;
  app.add_option("--dim", dimension, "Dimension of the generated graph")
      ->default_val(2);
    
  double temp;
  app.add_option("--temp", temp, "Temperature of the generated graph")
      ->default_val(0.0);
  
  bool torus;
  app.add_flag("--torus", torus, "Generate a torus graph (default: false))")
      ->default_val(false);


  CLI11_PARSE(app, argc, argv);
    
  // create option struct for graph generation
  GenOptions options {
    num_nodes,
    powerlaw_exponent,
    degree,
    dimension,
    temp,
    torus
  };

  Graph G = GraphGenerator::generateRandomGraph(options);
  ComponentStructure CS(G);
  Graph H(G, CS.nodes_of_component(CS.largest_component()));

  std::ofstream output;
  output.open(file_name);
  write_edge_list(H, output);

  std::cerr << "number of nodes: " << H.n() << std::endl;
  std::cerr << "number of edges: " << H.m() << std::endl;
  return 0;
}
