#pragma once

#include <vector>
#include <queue>

#include "graph.hpp"

class BFS
{
public:
    // returns pair of distance and cost (number of edge traversals)
    static std::pair<signed, unsigned> search(const Graph &g, node s, node t);
    static unsigned eccentricity(const Graph &g, node s);
};

class biBFS
{
    struct State
    {
        std::queue<node> q;
        std::vector<signed> dist;
        signed next_cost;
        signed total_cost;
        signed layer;
    };
    static void init_state(State &state, const Graph &g, node s);
public:
    // returns pair of distance and cost (number of edge traversals)
    static std::pair<signed, unsigned> search(const Graph &g, node s, node t, bool detour=false);
};
