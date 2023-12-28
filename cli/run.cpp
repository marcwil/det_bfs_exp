#include <iostream>

#include "CLI11.hpp"
#include "component_structure.hpp"
#include "graph.hpp"
#include "timer.hpp"

int main(int argc, char** argv) {
  CLI::App app{"Beating the Worst Case"};

  std::string file_name;
  app.add_option("input_file", file_name,
                 "Name of the file containing the graph")
      ->required()
      ->check(CLI::ExistingFile);

  CLI11_PARSE(app, argc, argv);

  Timer::create_timer("total_time");
  Timer::create_timer("load_graph");
  Timer::create_timer("components");
  Timer::create_timer("subgraph");

  Timer::start_timer("total_time");
  
  Timer::start_timer("load_graph");
  Graph G(file_name);
  Timer::stop_timer("load_graph");

  Timer::start_timer("components");
  ComponentStructure CS(G);
  Timer::stop_timer("components");
  
  Timer::start_timer("subgraph");
  Graph H(G, CS.nodes_of_component(CS.largest_component()));
  Timer::stop_timer("subgraph");

  Timer::stop_timer("total_time");

  std::cout << "number of nodes: " << H.n() << std::endl;
  std::cout << "number ed edges: " << H.m() << std::endl;
 
  std::cout << "time report:" << std::endl;
  for (const auto& timer_name : Timer::existing_timers()) {
    std::cout << timer_name << ":\t" << Timer::time(timer_name).count() << " ms"
              << std::endl;
  }
  return 0;
}
