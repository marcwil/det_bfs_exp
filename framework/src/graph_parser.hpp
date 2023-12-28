#pragma once

#include <filesystem>
#include <fstream>
#include <vector>

#include "graph.hpp"

// Utility class for parsing a graph from a list of edges, where each edge is
// given as a pair of node labels.  The node labels are assumed to be
// non-negative integers.  The abstract base class takes care of translating
// these labels to node IDs in the range [0, n).  This translation can be used
// by the derived classes by calling node_by_label.  The derived classes then
// only have to implement has_next() and next().
class GraphParserBase {
 public:
  GraphParserBase();

  // Return true if and only if there is a next edge to read.
  virtual bool has_next() = 0;

  // Return the next edge as a pair of nodes.
  virtual std::pair<node, node> next() = 0;

 protected:
  node m_next_unused_node;

  // Translate a label (the labels can have gaps or, e.g., start at 1) to a
  // node. For bipartite graphs reusing labels for the second partition, call it
  // with partition = 0 for the first and partition = 1 for the second
  // partition.
  node node_by_label(unsigned label, unsigned partition = 0);

  std::vector<std::vector<bool>> m_label_has_node;
  std::vector<std::vector<node>> m_node_by_label;
  std::vector<unsigned> m_next_unused_label;
};

// Graph parser implementation for parsing a graph from a file.
class GraphParserFile : public GraphParserBase {
 public:
  GraphParserFile(const std::filesystem::path& input_file);

  bool has_next();
  std::pair<node, node> next();

 private:
  std::ifstream m_file;
  bool m_bipartite;
};

// Graph parser implementation for parsing a graph from a list of edges, e.g.,
// to clean up the node IDs when creating a subgraph.
class GraphParserEdgeList : public GraphParserBase {
 public:
  GraphParserEdgeList(
      std::vector<std::pair<unsigned, unsigned>> node_label_pairs);

  bool has_next();
  std::pair<node, node> next();

 private:
  std::vector<std::pair<unsigned, unsigned>> m_node_label_pairs;
  unsigned m_next_pos;
};
