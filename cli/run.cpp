#include <iostream>

#include "CLI11.hpp"
#include "component_structure.hpp"
#include "graph.hpp"
#include "timer.hpp"
#include "expansion.hpp"
#include "bibfs.hpp"
#include "random.hpp"

int main(int argc, char** argv) {
  CLI::App app{"Beating the Worst Case"};

  std::string file_name;
  app.add_option("input_file", file_name,
                 "Name of the file containing the graph")
      ->required()
      ->check(CLI::ExistingFile);

  
  unsigned s = 0;
  app.add_option("s", s,
                 "s")
                 ->default_val(0);
  unsigned t = 0;
  app.add_option("t", t,
                 "t")
                 ->default_val(0);

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
  std::cout << "number of edges: " << H.m() << std::endl;

 
  std::cout << "time report:" << std::endl;
  for (const auto& timer_name : Timer::existing_timers()) {
    std::cout << timer_name << ":\t" << Timer::time(timer_name).count() << " ms"
              << std::endl;
  }



  if (s == 0 && t == 0) {
    while (s == t) {
      s = Random::natural_number(H.n()-1);
      t = Random::natural_number(H.n()-1);
    }
  }
  std::cout << "s " << s << ", t " << t << std::endl;

  auto exp = Expansion(H, s, t);
  auto res = exp.evaluate(2.0, 0.1);

  // print exp.cost_s
  std::cout << "Expan layer costs s" << std::endl;
  for (auto cost : exp.cost_s) {
    std::cout << cost << " ";
  }
  std::cout << std::endl;

  // print exp.cost_t
  std::cout << "Expan layer costs t" << std::endl;
  for (auto cost : exp.cost_t) {
    std::cout << cost << " ";
  }
  std::cout << std::endl;

  std::cout << "Expan cost uni s " << res.cost_uni_s << std::endl;
  std::cout << "Expan cost uni s " << res.cost_uni_t << std::endl;
  std::cout << "Expan cost bi " << res.cost_bi << std::endl;
  std::cout << "Expan dist(s,t) " << res.dist << std::endl;

  // BFS cost
  auto [uni_dist, uni_cost] = BFS::search(H, s, t);
  std::cout << "BFS cost uni s " << uni_cost << std::endl;
  std::cout << "BFS dist s t " << uni_dist << std::endl;

  // BiBFS cost
  auto [bi_dist, bi_cost] = biBFS::search(H, s, t);
  std::cout << "BiBFS cost bi " << bi_cost << std::endl;
  std::cout << "BiBFS dist s t " << bi_dist << std::endl;

  return 0;
}
