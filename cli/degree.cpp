#include <iostream>

#include "CLI11.hpp"
#include "graph.hpp"
#include "component_structure.hpp"
#include "bibfs.hpp"
#include "timer.hpp"
#include "random.hpp"

int main(int argc, char** argv) {
  CLI::App app{"Beating the Worst Case -- Degree"};

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
    std::cout << "deg,num" << std::endl;
  }
  if (only_header) {
    return 0;
  }

  Graph G(file_name);
  auto n = G.n();

  unsigned maxdeg = 0;
  for (unsigned i = 0; i < n; i++) {
    maxdeg = std::max(maxdeg, G.degree(i));
  }
  auto hist = std::vector<unsigned>(maxdeg+1, 0);
  for (unsigned i = 0; i < n; i++) {
    hist[G.degree(i)]++;
  }

  for (unsigned i = 0; i < maxdeg+1; i++) {
    if (hist[i] > 0) {
      std::cout << i << "," << hist[i] << std::endl;
    }
  }

  return 0;
}
