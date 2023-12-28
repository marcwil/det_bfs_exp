#pragma once

#include <vector>
#include <queue>
#include <cmath>

#include "graph.hpp"

// Macro defining all fields
#define EXPANSION_RESULT_FIELDS \
    X(double, b)                \
    X(double, alpha)            \
    X(unsigned, s)              \
    X(unsigned, t)              \
    X(unsigned, cost_uni_s)     \
    X(unsigned, cost_uni_t)     \
    X(unsigned, cost_bi)        \
    X(unsigned, dist)           \
    X(unsigned, expan_s)        \
    X(unsigned, expan_t)        \
    X(unsigned, expan_s_prime)  \
    X(unsigned, expan_t_prime)  \
    X(unsigned, cheap_s)        \
    X(unsigned, cheap_t)        \
    X(signed, rel_dist)         \
    X(signed, overlap)          \
    X(signed, T1)               \
    X(signed, T2)               \
    X(signed, S1)               \
    X(signed, S2)               \
    X(double, bplus)            \
    X(double, rho_prime)

struct ExpansionResult
{
// Define fields using the macro
#define X(type, name) type name;
    EXPANSION_RESULT_FIELDS
#undef X

    // Method to serialize to CSV
    std::string toCSVString() const
    {
        std::stringstream ss;
#define X(type, name) ss << name << ",";
        EXPANSION_RESULT_FIELDS
#undef X
        std::string result = ss.str();
        return result.substr(0, result.size() - 1); // Remove trailing comma
    }

    // Method to generate CSV header
    static std::string CSVHeader()
    {
        std::stringstream ss;
#define X(type, name) ss << #name << ",";
        EXPANSION_RESULT_FIELDS
#undef X
        std::string header = ss.str();
        return header.substr(0, header.size() - 1); // Remove trailing comma
    }
};

#define RHOEXP_RESULT_FIELDS   \
    Y(double, b)               \
    Y(double, bplus)           \
    Y(unsigned, s)             \
    Y(unsigned, t)             \
    Y(unsigned, dist)          \
    Y(double, optimal_alpha)   \
    Y(signed, optimal_T1)      \
    Y(signed, optimal_T2)      \
    Y(signed, optimal_S1)      \
    Y(signed, optimal_S2)      \
    Y(signed, optimal_cheap_s) \
    Y(signed, optimal_cheap_t) \
    Y(double, optimal_rho)     \
    Y(double, optimal_rhomax)

struct RhoExpansionResult
{
// Define fields using both macros
#define Y(type, name) type name;
    RHOEXP_RESULT_FIELDS
#undef Y

    // Method to serialize to CSV
    std::string toCSVString() const
    {
        std::stringstream ss;
#define Y(type, name) ss << name << ",";
        RHOEXP_RESULT_FIELDS
#undef Y
        std::string result = ss.str();
        return result.substr(0, result.size() - 1); // Remove trailing comma
    }

    // Method to generate CSV header
    static std::string CSVHeader()
    {
        std::stringstream ss;
#define Y(type, name) ss << #name << ",";
        RHOEXP_RESULT_FIELDS
#undef Y
        std::string header = ss.str();
        return header.substr(0, header.size() - 1); // Remove trailing comma
    }
};

class Expansion
{
private:
    Graph g;
    node s;
    node t;
    bool evaluate_rho;
    std::vector<unsigned> cost_s;
    std::vector<unsigned> cost_t;
    std::vector<unsigned> cost_s_cum;
    std::vector<unsigned> cost_t_cum;
    std::vector<double> exp_s;
    std::vector<double> exp_t;
    signed dist = -1;
    std::vector<unsigned> compute_cost(node from, node to);
    static std::vector<double> expansion(const std::vector<unsigned> &nums);
    unsigned high_prefix_length(double base, std::vector<double> &nums);
    unsigned high_prefix_length_skip(double threshold, std::vector<double> &nums, unsigned skip);
    unsigned cheap_prefix_length(unsigned threshold, std::vector<unsigned> &nums);
    unsigned uni_cost(std::vector<unsigned> &nums);
    unsigned bi_cost();

public:
    Expansion(const Graph _g, node _s, node _t, bool evaluate_rho = false);
    signed get_dist();
    ExpansionResult evaluate(double base, double alpha);
    double calc_rho(signed expan_s, signed expan_t, signed cheap_s, signed cheap_t);
    double rho_conv(double param);
    RhoExpansionResult rho_evaluate(double base);
    std::vector<std::vector<unsigned>> compute_layers(node from);
    std::vector<unsigned> layer_costs(std::vector<std::vector<unsigned>> &layers);

    static std::vector<unsigned> cumulative(std::vector<unsigned> &nums);
    friend class ExpansionTest_Eccentricity_Test; // Grant access to the test class
    friend class ExpansionTest_Cheap_Test;        // Grant access to the test class
    friend class ExpansionTest_Expan_Test;        // Grant access to the test class
    friend class ExpansionTest_Expan_Test;        // Grant access to the test class
};
