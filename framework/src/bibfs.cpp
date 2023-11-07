#include "framework/bibfs.hpp"
//#include "bibfs.hpp"


std::pair<signed, unsigned> BFS::search(const Graph &g, node s, node t)
{
  unsigned n = g.n();
  std::vector<signed> dist(n, -1);
  std::queue<node> q;
  unsigned res = 0;

  // init BFS queue and distance vector
  dist[s] = 0;
  q.push(s);

  while (!q.empty()) {
    node v = q.front(); q.pop();

    // if target found, return distance
    if (v == t) {
      return std::make_pair(dist[v], res);
    }

    for (node w : g.neighbors(v)) {
      res++;
      if (dist[w] == -1) {
        // update distance and add to queue
        dist[w] = dist[v] + 1;
        q.push(w);
      }
    }
  }

  return std::make_pair(-1, res);
}

void biBFS::init_state(State &state, const Graph &g, node v)
{
  state.q = std::queue<node>();
  state.q.push(v);
  state.dist = std::vector<signed>(g.n(), -1);
  state.dist[v] = 0;
  state.next_cost = g.degree(v);
  state.total_cost = 0;
  state.layer = 0;
}

std::pair<signed, unsigned> biBFS::search(const Graph &g, node s, node t)
{
  //init state
  State stateA;
  State stateB;
  init_state(stateA, g, s);
  init_state(stateB, g, t);

  State *state = (stateA.next_cost <= stateB.next_cost) ? &stateA : &stateB;

  while (!stateA.q.empty() && !stateB.q.empty())
  {
    auto next_layer = state->dist[state->q.front()];
    if (next_layer != state->layer)  // layer complete
    {
      state->layer++;
      // select cheaper state
      state = (stateA.next_cost <= stateB.next_cost) ? &stateA : &stateB;
      state->next_cost = 0;
    }

    node v = state->q.front(); state->q.pop();
    for (node w : g.neighbors(v))
    {
      state->total_cost ++;
      if (state->dist[w] == -1)
      {
        state->dist[w] = state->dist[v] + 1;
        state->q.push(w);
        state->next_cost += g.degree(w);
      }
      // abort if found
      if (stateA.dist[w] != -1 && stateB.dist[w] != -1)
      {
        auto dist = stateA.dist[w] + stateB.dist[w];
        return std::make_pair(dist, stateA.total_cost + stateB.total_cost);
      }
    }
  }
  
  return std::make_pair(-1, stateA.total_cost + stateB.total_cost);
}
