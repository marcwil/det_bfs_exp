#pragma once

#include <vector>
#include <queue>
#include <iostream>

#include "graph.hpp"
#include "timer.hpp"

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

class PerfBiBFS
{
    struct State
    {
        std::queue<node> q;
        std::vector<signed> dist;
        std::vector<unsigned> timestamp;
        signed next_cost;
        signed total_cost;
        signed layer;
        unsigned curr_time;
        void set_dist(node v, signed d)
        {
            dist[v] = d;
            timestamp[v] = curr_time;
        }
        signed get_dist(node v)
        {
            if (timestamp[v] != curr_time)
            {
                return -1;
            }
            return dist[v];
        }
    };
    void init_state(State &state);
    void re_init_state(State &state, node v);
    const Graph &g;
    State stateA;
    State stateB;
    public:
    // constructor: initialize with graph
    PerfBiBFS(const Graph &g);
    // shortest path query, returns pair of distance, cost (number of edge traversals), and tim
    std::tuple<signed, unsigned, double> query(node s, node t);
};