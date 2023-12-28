#include "framework/graph.hpp"

#include <algorithm>

#include "framework/graph_parser.hpp"

Graph::Graph() : m_nr_nodes(0), m_nr_edges(0) {}

Graph::Graph(const std::filesystem::path& input_file) : Graph() {
  GraphParserFile parser(input_file);
  load_from_parser(parser);
}

Graph::Graph(const Graph& G, const std::vector<node>& nodes) : Graph() {
  std::vector<bool> keep_node(G.n(), false);
  for (node v : nodes) {
    keep_node[v] = true;
  }

  std::vector<std::pair<node, node>> edges;
  for (node u = 0; u < G.n(); ++u) {
    for (node v : G.neighbors(u)) {
      if (keep_node[u] && keep_node[v]) {
        edges.push_back({u, v});
      }
    }
  }

  GraphParserEdgeList parser(std::move(edges));
  load_from_parser(parser);
}

Graph::Graph(const std::vector<std::pair<unsigned, unsigned>>& edges) : Graph() {
  GraphParserEdgeList parser(edges);
  load_from_parser(parser);
}

void Graph::load_from_parser(GraphParserBase& parser) {
    while (parser.has_next()) {
        auto [u, v] = parser.next();
        node max = std::max(u, v);
        if (max >= m_nr_nodes) {
            m_nr_nodes = max + 1;
            m_adj_list.resize(m_nr_nodes);
        }

        m_adj_list[u].push_back(v);
        m_adj_list[v].push_back(u);
        m_nr_edges++;
    }

    make_simple();
}

unsigned Graph::n() const { return m_nr_nodes; }

unsigned Graph::m() const { return m_nr_edges; }

const std::vector<node>& Graph::neighbors(node v) const {
  return m_adj_list[v];
}

bool Graph::has_edge(node u, node v) const {
  auto& neighbors = m_adj_list[u];
  return std::find(neighbors.begin(), neighbors.end(), v) != neighbors.end();
}

unsigned Graph::degree(node v) const { return neighbors(v).size(); }

void Graph::make_simple() {
  m_nr_edges = 0;
  for (node v = 0; v < n(); ++v) {
    auto& neighbors = m_adj_list[v];
    std::sort(neighbors.begin(), neighbors.end());
    // remove multiple edges
    neighbors.erase(std::unique(neighbors.begin(), neighbors.end()),
                    neighbors.end());
    // remove self-loops
    auto range = std::equal_range(neighbors.begin(), neighbors.end(), v);
    neighbors.erase(range.first, range.second);

    // count edges
    m_nr_edges += neighbors.size();
  }
  m_nr_edges /= 2;
}
