#include "graph_writer.hpp"

void write_edge_list(const Graph& G, std::ostream& out) {
  for (node u = 0; u < G.n(); ++u) {
    for (node v : G.neighbors(u)) {
      if (u <= v) {
        out << u << " " << v << std::endl;
      }
    }
  }
}
