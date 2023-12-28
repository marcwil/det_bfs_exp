#include "graph_parser.hpp"

#include <sstream>
#include <string>

GraphParserBase::GraphParserBase()
    : m_next_unused_node(0),
      m_label_has_node(2),
      m_node_by_label(2),
      m_next_unused_label(2, 0) {}

node GraphParserBase::node_by_label(unsigned label, unsigned partition) {
  // label larger than largest previous label -> resize
  if (label >= m_next_unused_label[partition]) {
    m_next_unused_label[partition] = label + 1;
    m_node_by_label[partition].resize(m_next_unused_label[partition]);
    m_label_has_node[partition].resize(m_next_unused_label[partition], false);
  }

  // label has not been seen before -> associate new node with it
  if (!m_label_has_node[partition][label]) {
    m_node_by_label[partition][label] = m_next_unused_node++;
    m_label_has_node[partition][label] = true;
  }

  // return the node associated with the label
  return m_node_by_label[partition][label];
}

GraphParserFile::GraphParserFile(const std::filesystem::path& input_file)
    : GraphParserBase(), m_file(input_file), m_bipartite(false) {
  bool mtx_format = false;
  // check if we have the konect format with a bipartite graph
  if (m_file.peek() == '%') {
    std::string format;
    std::getline(m_file, format);
    if (format.find("bip") != std::string::npos) {
      m_bipartite = true;
    } else if (format.find("MatrixMarket") != std::string::npos) {
      mtx_format = true;
    }
  }

  // gobble comment lines at the beginning
  while (m_file.peek() == '%') {
    m_file.ignore(std::numeric_limits<std::streamsize>::max(),
                  m_file.widen('\n'));
  }

  // throw away the first line if in mtx format
  if (mtx_format) {
    m_file.ignore(std::numeric_limits<std::streamsize>::max(),
                  m_file.widen('\n'));
  }
}

bool GraphParserFile::has_next() { return m_file.peek() != EOF; }

std::pair<node, node> GraphParserFile::next() {
  // parse node labels from the next line
  std::string line;
  std::getline(m_file, line);
  std::stringstream line_ss(line);
  unsigned u_label, v_label;
  line_ss >> u_label;
  line_ss >> v_label;
  
  // convert labels to node ids
  if (!m_bipartite) {
    return {node_by_label(u_label), node_by_label(v_label)};
  } else {
    return {node_by_label(u_label, 0), node_by_label(v_label, 1)};
  }
}

GraphParserEdgeList::GraphParserEdgeList(
    std::vector<std::pair<unsigned, unsigned>> node_label_pairs)
    : m_node_label_pairs(std::move(node_label_pairs)), m_next_pos(0) {}

bool GraphParserEdgeList::has_next() {
  return m_next_pos < m_node_label_pairs.size();
}

std::pair<node, node> GraphParserEdgeList::next() {
  auto [u_label, v_label] = m_node_label_pairs[m_next_pos];
  m_next_pos++;
  return {node_by_label(u_label), node_by_label(v_label)};
}
