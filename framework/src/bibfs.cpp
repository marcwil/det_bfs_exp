#include <algorithm>
#include <cassert>
#include "bibfs.hpp"
//#include "bibfs.hpp"


std::pair<signed, unsigned> BFS::search(const Graph &g, node s, node t)
{
  unsigned n = g.n();
  std::vector<signed> dist(n, -1);
  std::queue<node> q;
  unsigned cost = 0;

  // init BFS queue and distance vector
  dist[s] = 0;
  q.push(s);

  while (!q.empty()) {
    node v = q.front(); q.pop();

    // if target found, return distance
    if (v == t) {
      return std::make_pair(dist[v], cost);
    }

    for (node w : g.neighbors(v)) {
      cost++;
      if (dist[w] == -1) {
        // update distance and add to queue
        dist[w] = dist[v] + 1;
        // if target found, return distance
        if (w == t) {
          return std::make_pair(dist[w], cost);
        }

        q.push(w);
      }
    }
  }

  return std::make_pair(-1, cost);
}

unsigned BFS::eccentricity(const Graph & g, node s)
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
    for (node w : g.neighbors(v)) {
      res++;
      if (dist[w] == -1) {
        // update distance and add to queue
        dist[w] = dist[v] + 1;
        q.push(w);
      }
    }
  }

  return *std::max_element(dist.begin(), dist.end());
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

std::pair<signed, unsigned> biBFS::search(const Graph &g, node s, node t, bool detour)
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
      // if detour: ignore edge between s and t
      if (detour && ((v == s && w == t) || (v == t && w == s)))
      {
        continue;
      }

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

PerfBiBFS::PerfBiBFS(const Graph &g) : g(g)
{
  // initialize states
  init_state(stateA);
  init_state(stateB);

  Timer::create_timer("PerfBiBFS");
}

void PerfBiBFS::init_state(State &state)
{
  state.q = std::queue<node>();
  state.curr_time = 0;
  state.timestamp = std::vector<unsigned>(g.n(),0);
  state.dist = std::vector<signed>(g.n());
  state.next_cost = 0;
  state.total_cost = 0;
  state.layer = 0;
}

void PerfBiBFS::re_init_state(State &state, node v)
{
  assert(state.dist.size() == g.n());
  assert(state.q.empty());
  state.q.push(v);

  state.curr_time++;
  if (state.curr_time == 0)  // overflow
  {
    state.curr_time = 1;
    std::fill(state.timestamp.begin(), state.timestamp.end(), 0);
  }

  state.set_dist(v, 0);

  state.next_cost = g.degree(v);
  state.total_cost = 0;
  state.layer = 0;
}

std::tuple<signed, unsigned, double> PerfBiBFS::query(node s, node t)
{
  Timer::create_timer("PerfBiBFS");
  Timer::start_timer("PerfBiBFS");
  //init state
  re_init_state(stateA, s);
  re_init_state(stateB, t);


  State *state = (stateA.next_cost <= stateB.next_cost) ? &stateA : &stateB;

  while (!stateA.q.empty() && !stateB.q.empty())
  {
    auto next_layer = state->get_dist(state->q.front());
    if (next_layer != state->layer)  // layer complete
    {
      state->layer++;
      // select cheaper state
      state = (stateA.next_cost <= stateB.next_cost) ? &stateA : &stateB;
      state->next_cost = 0;
    }

    node v = state->q.front(); state->q.pop();
    auto dist_v = state->get_dist(v);
    for (node w : g.neighbors(v))
    {
      state->total_cost ++;
      if (state->get_dist(w) == -1)
      {
        state->set_dist(w, dist_v + 1);
        state->q.push(w);
        state->next_cost += g.degree(w);
      }
      // abort if found
      if (stateA.get_dist(w) != -1 && stateB.get_dist(w) != -1)
      {
        auto dist = stateA.get_dist(w) + stateB.get_dist(w);

        // empty queues
        while (!stateA.q.empty()) stateA.q.pop();
        while (!stateB.q.empty()) stateB.q.pop();
        Timer::stop_timer("PerfBiBFS");
        double time = Timer::time("PerfBiBFS").count();
        return std::make_tuple(dist, stateA.total_cost + stateB.total_cost, time);
      }
    }
  }
  
  Timer::stop_timer("PerfBiBFS");
  double time = Timer::time("PerfBiBFS").count();
  return std::make_tuple(-1, stateA.total_cost + stateB.total_cost, time);
}
