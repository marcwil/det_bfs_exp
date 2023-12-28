#pragma once

#include <filesystem>
#include <istream>
#include <string>
#include <vector>

class GraphParserBase;

typedef unsigned node;

// Very basic static graph data structure. A graph is always undirected,
// unweighted and simple (no self-loops, no multiple edges).
class Graph {
 public:
  // Create an empty graph.
  Graph();

  // Load a graph from a file. Supported formats are basic edge lists as well as
  // the primary format used by Konect. The format is detected automatically.
  // Self-loops and multiple edges are removed.
  Graph(const std::filesystem::path& input_file);

  // Create a subgraph induced by all edges between given nodes (i.e., basically
  // the subgraph induced by the given nodes sans isolated vertices).
  Graph(const Graph& G, const std::vector<node>& subgraph_nodes);

  // Create a graph from a vector of edges
  Graph(const std::vector<std::pair<unsigned, unsigned>> &edges);


  // Number of nodes and edges, respectively.
  unsigned n() const;
  unsigned m() const;

  // The neighbors of a node.
  const std::vector<node>& neighbors(node v) const;

  bool has_edge(node u, node v) const;

  // Check if there is an edge between two nodes.
  // The degree of a node.
  unsigned degree(node v) const;

 private:
  void load_from_parser(GraphParserBase& parser);

  std::vector<std::vector<node>> m_adj_list;
  unsigned m_nr_nodes;
  unsigned m_nr_edges;

  void make_simple();
};
