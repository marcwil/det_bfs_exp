#include <numeric>
#include <algorithm>
#include <filesystem>
#include <vector>

#include "gtest/gtest.h"

#include "component_structure.hpp"
#include "graph_generator.hpp"
#include "paths.h"
#include "expansion.hpp"
#include "graph.hpp"
#include "bibfs.hpp"

const std::filesystem::path test_input_dir{TEST_DATA_DIR};
const std::filesystem::path instances_dir{INSTANCES_DIR};

class ExpansionTest : public ::testing::Test
{
protected:
    // Objects declared here can be used in all tests in the test suite for ExpansionTest.
    Graph g; // Assuming Graph is a class that can be default constructed
    node s;  // Assuming node is a type that can be default constructed
    node t;  // Assuming node is a type that can be default constructed
    Expansion *expansion;

    void SetUp() override
    {
        // Initialize objects and state before each test
        std::filesystem::path arenas{instances_dir / "arenas-pgp"};
        g = Graph(arenas);
        s = 0;
        t = 40;
        expansion = new Expansion(g, s, t);
    }

    // TearDown() is invoked immediately after a test finishes.
    void TearDown() override
    {
        // Clean up after each test
        delete expansion;
        // delete g;
    }
};

TEST_F(ExpansionTest, TestGraphRead)
{
    EXPECT_EQ(g.n(), 10680);
    EXPECT_EQ(g.m(), 24316);
    EXPECT_EQ(g.degree(0), 1);
    EXPECT_EQ(g.degree(1), 2);
}

TEST_F(ExpansionTest, CumulativeCost)
{
    std::vector<unsigned> nums = {1, 2, 3, 4, 5, 6, 7, 8};
    std::vector<unsigned> cum = Expansion::cumulative(nums);
    EXPECT_EQ(cum.size(), nums.size() + 1);
    EXPECT_EQ(cum[0], 0);
    EXPECT_EQ(cum[1], 1);
    EXPECT_EQ(cum[2], 3);
    EXPECT_EQ(cum[3], 6);
    EXPECT_EQ(cum[4], 10);
    EXPECT_EQ(cum[5], 15);
    EXPECT_EQ(cum[6], 21);
    EXPECT_EQ(cum[7], 28);
    EXPECT_EQ(cum[8], 36);
}

// Test for a private attributes and methods
TEST_F(ExpansionTest, Eccentricity)
{
    //// Directly call private methods of Expansion
    // std::vector<unsigned> layers = expansion->cost_s;

    //// Make assertions to verify the behavior of the private method
    // EXPECT_EQ(layers.size(), 22);

    ComponentStructure CS(g);
    Graph H(g, CS.nodes_of_component(CS.largest_component()));
    for (unsigned i = 0; i < 100; i++)
    {
        node s = i;
        node t = (i + 100) % H.n();
        Expansion expansion(H, s, t);
        unsigned ecc = BFS::eccentricity(H, s);
        EXPECT_EQ(expansion.cost_s.size(), ecc + 1);
        EXPECT_EQ(expansion.cost_s[0], H.degree(s));
        unsigned neigh_degs =
            std::accumulate(H.neighbors(s).begin(), H.neighbors(s).end(), 0,
                            [&H](unsigned acc, node v)
                            { return acc + H.degree(v); });
        EXPECT_EQ(expansion.cost_s[1], neigh_degs);

        auto layers = expansion.compute_layers(s);
        auto layer_costs = expansion.layer_costs(layers);
        for (unsigned i = 0; i < layer_costs.size(); i++)
        {
            EXPECT_EQ(layer_costs[i], expansion.cost_s[i]);
        }
    }
}

TEST_F(ExpansionTest, TestGetDist)
{
    ComponentStructure CS(g);
    Graph H(g, CS.nodes_of_component(CS.largest_component()));
    for (unsigned i = 0; i < 100; i++)
    {
        node s = i;
        node t = (i + 100) % H.n();
        Expansion expansion(H, s, t);
        signed dist = expansion.get_dist();
        auto [uni_dist, uni_cost] = BFS::search(H, s, t);
        auto [bi_dist, bi_cost] = biBFS::search(H, s, t);
        EXPECT_EQ(dist, uni_dist);
        EXPECT_EQ(bi_dist, uni_dist);
    }
}

TEST_F(ExpansionTest, Cheap)
{
    Expansion exp(g, s, t);
    std::vector<unsigned> nums = {1, 2, 3, 4, 5, 6, 7, 8};
    EXPECT_EQ(exp.cheap_prefix_length(5, nums), 2);
    EXPECT_EQ(exp.cheap_prefix_length(6, nums), 3);

    nums = {2, 2, 2, 2, 2, 2, 2, 2};
    EXPECT_EQ(exp.cheap_prefix_length(1, nums), 0);
    EXPECT_EQ(exp.cheap_prefix_length(2, nums), 1);
    EXPECT_EQ(exp.cheap_prefix_length(6, nums), 3);
}

TEST_F(ExpansionTest, Expan)
{
    Expansion exp(g, s, t);
    exp.cost_s = {1, 2, 4, 8, 16, 32, 64, 128, 200};
    exp.cost_t = {1, 2, 4, 8, 10, 32, 64, 128, 200, 201};
    auto exp_s = exp.expansion(exp.cost_s);
    auto exp_t = exp.expansion(exp.cost_t);
    exp.exp_s = exp_s;
    exp.exp_t = exp_t;

    auto res2 = exp.evaluate(2.0, 0.0);
    auto res11 = exp.evaluate(1.1, 0.0);
    EXPECT_EQ(res2.expan_s, 7);
    EXPECT_EQ(res2.expan_t, 3);
    EXPECT_EQ(res11.expan_s, 8);
    EXPECT_EQ(res11.expan_t, 8);
}

TEST_F(ExpansionTest, RelDist)
{
    ComponentStructure CS(g);
    Graph H(g, CS.nodes_of_component(CS.largest_component()));
    for (unsigned i = 0; i < 100; i++)
    {
        node s = i;
        node t = (i + 100) % H.n();
        Expansion expansion(H, s, t);
        auto res = expansion.evaluate(2.0, 0.0);
        EXPECT_LE(res.rel_dist, res.dist);
        EXPECT_GE(res.rel_dist, 0);
    }
}

TEST_F(ExpansionTest, Sanity)
{
    std::vector<double> ple = {25, 5, 3, 2.3, 2.1};
    std::vector<double> temp = {0, 0.01, 0.3, 0.99};
    std::vector<int> dim = {1, 2};
    std::vector<GenOptions> options;
    for (auto p : ple)
    {
        for (auto t : temp)
        {
            for (auto d : dim)
            {
                options.push_back({5000, p, 15, d, t, true});
            }
        }
    }

    for (auto opt : options)
    {
        Graph G = GraphGenerator::generateRandomGraph(opt);
        ComponentStructure CS(G);
        Graph H(G, CS.nodes_of_component(CS.largest_component()));
        if (H.n() < 100)
        {
            continue;
        }
        for (unsigned i = 0; i < 50; i++)
        {
            node s = i;
            node t = (i + 100) % H.n();
            Expansion expansion(H, s, t);
            auto res = expansion.evaluate(2.0, 0.25);

            EXPECT_LE(res.cheap_s, res.dist);
            EXPECT_GE(res.cheap_s, 0);
            EXPECT_LE(res.cheap_t, res.dist);
            EXPECT_GE(res.cheap_t, 0);

            EXPECT_LE(res.expan_s, res.dist);
            EXPECT_GE(res.expan_s, 0);
            EXPECT_LE(res.expan_t, res.dist);
            EXPECT_GE(res.expan_t, 0);

            EXPECT_GE(res.expan_s_prime, res.expan_s);
            EXPECT_GE(res.expan_t_prime, res.expan_t);
            EXPECT_LE(res.expan_s_prime, res.dist);
            EXPECT_LE(res.expan_t_prime, res.dist);

            EXPECT_LE(res.rel_dist, static_cast<signed>(res.dist));
            EXPECT_GE(res.rel_dist, -static_cast<signed>(res.dist));

            EXPECT_LE(res.overlap, static_cast<signed>(res.dist));
            EXPECT_GE(res.overlap, -static_cast<signed>(res.dist));
        }
    }
}

TEST_F(ExpansionTest, BasicRho)
{
    Expansion exp(g, 23, 123);
    auto res = exp.rho_evaluate(2.0);
    EXPECT_NEAR(res.bplus, 53.323529411764703, 1e-6);
    EXPECT_NEAR(res.optimal_alpha, 0.3912552545589047, 1e-6);
    EXPECT_NEAR(res.optimal_rho, 1, 1e-6);
    EXPECT_NEAR(res.optimal_rhomax, 0.82416403727422349, 1e-6);
    EXPECT_EQ(res.optimal_S1, 2);
    EXPECT_EQ(res.optimal_S2, 0);
    EXPECT_EQ(res.optimal_T1, 2);
    EXPECT_EQ(res.optimal_T2, 0);
    EXPECT_EQ(res.optimal_cheap_s, 1);
    EXPECT_EQ(res.optimal_cheap_t, 1);

    auto other_res = exp.evaluate(2.0, res.optimal_alpha);
    double other_rho = other_res.rho_prime;
    double other_converted = exp.rho_conv(other_rho);
    EXPECT_NEAR(res.optimal_rho, other_converted, 1e-6);
    EXPECT_EQ(res.optimal_cheap_s, other_res.cheap_s);
    EXPECT_EQ(res.optimal_cheap_t, other_res.cheap_t);

    Expansion exp2(g, 42, 12);
    auto res2 = exp2.rho_evaluate(2.0);
    EXPECT_NEAR(res2.bplus, 55.777777777777779, 1e-6);
    EXPECT_NEAR(res2.optimal_alpha, 0.0, 1e-6);
    EXPECT_NEAR(res2.optimal_rho, 1, 1e-6);
    EXPECT_NEAR(res2.optimal_rhomax, 0.5, 1e-6);
    EXPECT_EQ(res2.optimal_S1, 2);
    EXPECT_EQ(res2.optimal_S2, 0);
    EXPECT_EQ(res2.optimal_T1, 2);
    EXPECT_EQ(res2.optimal_T2, 0);
    EXPECT_EQ(res2.optimal_cheap_s, 0);
    EXPECT_EQ(res2.optimal_cheap_t, 0);
}

TEST_F(ExpansionTest, RhoRandom)
{
    GenOptions opt = {5000, 2.0, 15, 2, 0.25, true};
    Graph G = GraphGenerator::generateRandomGraph(opt);
    ComponentStructure CS(G);
    Graph H(G, CS.nodes_of_component(CS.largest_component()));
    for (unsigned i = 0; i < 50; i++)
    {
        node s = i;
        node t = (i + 100) % H.n();
        Expansion expansion(H, s, t);
        // for b in [1.1, 1.25, 1.5, 2, 4]:
        for (double b: {1.1, 1.25})
        {
            std::cout << "s: " << s << " t: " << t << " b: " << b << std::endl;

            auto rhores = expansion.rho_evaluate(b);
            // multiply alphaby  1.0001 to avoid numerical issues
            auto res = expansion.evaluate(b, rhores.optimal_alpha*1.0001);
            double other_rho = expansion.rho_conv(res.rho_prime);

            // print res
            std::cout << "b: " << b << " bplus: " << rhores.bplus << " alpha: " << rhores.optimal_alpha << " rho: " << rhores.optimal_rho << " rhomax: " << rhores.optimal_rhomax << " S1: " << rhores.optimal_S1 << " S2: " << rhores.optimal_S2 << " T1: " << rhores.optimal_T1 << " T2: " << rhores.optimal_T2 << " cheap_s: " << rhores.optimal_cheap_s << " cheap_t: " << rhores.optimal_cheap_t << std::endl;
            std::cout << "b: " << b << " bplus: " << res.bplus << " alpha: " << res.alpha << " rho (converted): " << expansion.rho_conv(res.rho_prime) << " S1: " << res.S1 << " S2: " << res.S2 << " T1: " << res.T1 << " T2: " << res.T2 << " cheap_s: " << res.cheap_s << " cheap_t: " << res.cheap_t << std::endl;

            EXPECT_NEAR(rhores.optimal_rho, other_rho, 1e-6);
            EXPECT_EQ(rhores.optimal_cheap_s, res.cheap_s);
            EXPECT_EQ(rhores.optimal_cheap_t, res.cheap_t);
        }
    }
}

// TEST_F(ExpansionTest, TestEvaluate)
//{
//     Expansion expansion(g, s, t);
//     double base = 1.0;
//     double alpha = 1.0;
//     ExpansionResult result = expansion.evaluate(base, alpha);
//     // Add assertions here based on your expected ExpansionResult
//     // For example, if ExpansionResult has a member 'value' that should be 0.0 initially:
//     // EXPECT_DOUBLE_EQ(result.value, 0.0);
// }