#include <chrono>
#include <thread>

#include "locality.hpp"
#include "gtest/gtest.h"

TEST(clustering_coeff, global_clustering) {
    std::vector<std::pair<unsigned, unsigned>> edges1 = {{0, 1}, {0, 2}, {1, 2}};
    Graph K3(edges1);
    global_cluster_info result1 = global_clustering(K3);
    EXPECT_EQ(3, result1.num_closed_triplets);
    EXPECT_EQ(3, result1.num_triplets);
    EXPECT_DOUBLE_EQ(1, result1.global_clustering_coeff);

    std::vector<std::pair<unsigned, unsigned>> edges2 = {{0, 1}, {0, 2}, {0, 3}, {1, 2}, {1, 3}, {2, 3}};
    Graph K4(edges2);
    global_cluster_info result2 = global_clustering(K4);
    EXPECT_EQ(12, result2.num_closed_triplets);
    EXPECT_EQ(12, result2.num_triplets);
    EXPECT_DOUBLE_EQ(1, result2.global_clustering_coeff);

    std::vector<std::pair<unsigned, unsigned>> edges3 = {{0, 1}, {0, 2}};
    Graph P2(edges3);
    global_cluster_info result3 = global_clustering(P2);
    EXPECT_EQ(0, result3.num_closed_triplets);
    EXPECT_EQ(1, result3.num_triplets);
    EXPECT_DOUBLE_EQ(0, result3.global_clustering_coeff);

    std::vector<std::pair<unsigned, unsigned>> edges4 = {{0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}};
    Graph Star5(edges4);
    global_cluster_info result4 = global_clustering(Star5);
    EXPECT_EQ(0, result4.num_closed_triplets);
    EXPECT_EQ(15, result4.num_triplets);
    EXPECT_DOUBLE_EQ(0, result4.global_clustering_coeff);

    std::vector<std::pair<unsigned, unsigned>> edges5 = {{0, 1}, {0, 2}, {1, 2}, {1, 4}, {2, 4}};
    Graph Triangle2(edges5);
    global_cluster_info result5 = global_clustering(Triangle2);
    EXPECT_EQ(6, result5.num_closed_triplets);
    EXPECT_EQ(8, result5.num_triplets);
    EXPECT_DOUBLE_EQ(6.0 / 8.0, result5.global_clustering_coeff);
}

TEST(clustering_coeff, detour_distance) {
    // only bridges. cant calculate for P4
    // std::vector<std::pair<unsigned, unsigned>> edges1 = {{0, 1}, {1, 2}, {2, 3}};
    // Graph P4(edges1);
    // average_distance_info result1 = average_distance_locality(P4, 10000);
    // EXPECT_NEAR((2.0 + 2.0 + 3.0) / 3.0, result1.average_distance, 0.1);
    // EXPECT_NEAR(4.0, result1.average_detour, 0.1);

    std::vector<std::pair<unsigned, unsigned>> edges2 = {{0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 0}};
    Graph C6(edges2);
    average_distance_info result2 = average_distance_locality(C6, 10000);
    EXPECT_NEAR((2.0 + 2.0 + 3.0) / 3.0, result2.average_distance, 0.1);
    EXPECT_NEAR(5.0, result2.average_detour, 0.1);
}
