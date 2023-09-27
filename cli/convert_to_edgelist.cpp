#include <iostream>

#include "CLI11.hpp"
#include "framework/component_structure.hpp"
#include "framework/graph.hpp"
#include "framework/graph_writer.hpp"

int main(int argc, char** argv) {
  CLI::App app{"Converting Konect or mtx format to simple edge list"};

  std::filesystem::path file_name_in;
  app.add_option("input_file", file_name_in,
                 "Name of the file containing the graph.")
      ->required()
      ->check(CLI::ExistingFile);

  CLI11_PARSE(app, argc, argv);

  std::filesystem::path file_name_out = file_name_in;
  file_name_out.replace_extension(".el");  

  std::cout << file_name_out << std::endl;
  
  Graph G(file_name_in);
  
  std::ofstream out(file_name_out);
  write_edge_list(G, out);

  return 0;
}
