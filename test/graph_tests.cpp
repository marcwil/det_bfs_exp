#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>

#include "component_structure.hpp"
#include "graph.hpp"
#include "paths.h"
#include "gtest/gtest.h"

const std::filesystem::path input_dir{TEST_DATA_DIR};

TEST(graph, basic_io) {
  const std::vector<std::string> file_names{"butterfly_graph_edge_list",
                                            "butterfly_graph_konect",
                                            "butterfly_graph_edge_list_multi"};
  for (const auto& file_name : file_names) {
    std::filesystem::path file{input_dir / file_name};
    Graph G(file);

    EXPECT_EQ(G.n(), 5);
    EXPECT_EQ(G.m(), 6);
    
    unsigned nr_deg_2{0}, nr_deg_4{0}, nr_deg_other{0};
    for (node v = 0; v < G.n(); ++v) {
      if (G.degree(v) == 2) {
        nr_deg_2++;
      } else if (G.degree(v) == 4) {
        nr_deg_4++;
      } else {
        nr_deg_other++;
      }
    }
    EXPECT_EQ(nr_deg_2, 4);
    EXPECT_EQ(nr_deg_4, 1);
    EXPECT_EQ(nr_deg_other, 0);
  }
}

TEST(graph, component_structure) {
  const std::vector<std::string> file_names{"butterfly_k3_loop",
                                            "butterfly_k3_loop_shuffle"};
  for (const auto& file_name : file_names) {
    std::filesystem::path file{input_dir / file_name};
    Graph G(file);
    ComponentStructure CS(G);

    EXPECT_EQ(CS.n(CS.largest_component()), 5);
    EXPECT_EQ(CS.nr_components(), 3);

    std::vector<unsigned> comp_sizes{CS.n(0), CS.n(1), CS.n(2)};
    std::sort(comp_sizes.begin(), comp_sizes.end());
    EXPECT_EQ(comp_sizes[0], 1);
    EXPECT_EQ(comp_sizes[1], 3);
    EXPECT_EQ(comp_sizes[2], 5);

    for (component c = 0; c < CS.nr_components(); ++c) {
      for (node v : CS.nodes_of_component(c)) {
        EXPECT_EQ(CS.component_of_node(v), c);
      }
    }
  }
}

TEST(graph, subgraph) {
  const std::vector<std::string> file_names{"butterfly_k3_loop",
                                            "butterfly_k3_loop_shuffle"};
  for (const auto& file_name : file_names) {
    std::filesystem::path file{input_dir / file_name};
    Graph G(file);
    ComponentStructure CS(G);

    Graph G_butterfly, G_K3, G_loop;
    for (component c = 0; c < CS.nr_components(); ++c) {
      if (CS.n(c) == 1) {
        G_loop = Graph(G, CS.nodes_of_component(c));
      } else if (CS.n(c) == 3) {
        G_K3 = Graph(G, CS.nodes_of_component(c));
      } else {
        G_butterfly = Graph(G, CS.nodes_of_component(c));
      }
    }

    EXPECT_EQ(G_butterfly.n(), 5);
    EXPECT_EQ(G_butterfly.m(), 6);
    EXPECT_EQ(G_K3.n(), 3);
    EXPECT_EQ(G_K3.m(), 3);
    EXPECT_EQ(G_loop.n(), 0);
    EXPECT_EQ(G_loop.m(), 0);

    EXPECT_EQ(G_K3.degree(0), 2);
    EXPECT_EQ(G_K3.degree(1), 2);
    EXPECT_EQ(G_K3.degree(2), 2);
  }
}
