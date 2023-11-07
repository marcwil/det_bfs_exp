#pragma once

#include <vector>
#include <queue>

#include "framework/graph.hpp"

class BFS
{
public:
    static std::pair<signed, unsigned> search(const Graph &g, node s, node t);
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
    static std::pair<signed, unsigned> search(const Graph &g, node s, node t);
};
