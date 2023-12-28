#include <algorithm>
#include <iostream>

#include "expansion.hpp"
#include "component_structure.hpp"
#include <cassert>

Expansion::Expansion(Graph _g, node _s, node _t, bool evaluate_rho)
{
  this->g = _g;
  this->s = _s;
  this->t = _t;
  this->evaluate_rho = evaluate_rho;

  this->cost_s = this->compute_cost(s, t);
  this->cost_t = this->compute_cost(t, s);
  this->exp_s = this->expansion(this->cost_s);
  this->exp_t = this->expansion(this->cost_t);

  // cumulative cost_s and cost_t
  this->cost_s_cum = this->cumulative(cost_s);
  this->cost_t_cum = this->cumulative(cost_t);
}

std::vector<std::vector<unsigned>> Expansion::compute_layers(node from)
{
  auto layers = std::vector<std::vector<unsigned>>();
  auto dists = std::vector<signed>(g.n(), -1);
  auto queue = std::queue<node>();
  unsigned current_layer;

  queue.push(from);
  dists[from] = 0;
  current_layer = -1;

  while (!queue.empty())
  {
    auto node = queue.front();
    queue.pop();
    if (dists[node] != current_layer)
    {
      // new layer begins
      current_layer++;
      layers.push_back(std::vector<unsigned>());
    }
    layers[current_layer].push_back(node);
    for (auto neigh : g.neighbors(node))
    {
      if (dists[neigh] == -1)
      {
        dists[neigh] = dists[node] + 1;
        queue.push(neigh);
      }
    }
  }
  return layers;
}

std::vector<unsigned> Expansion::layer_costs(std::vector<std::vector<unsigned>> &layers)
{
  auto costs = std::vector<unsigned>();
  for (auto l : layers)
  {
    unsigned cost = 0;
    for (auto n : l)
    {
      cost += g.degree(n);
    }
    costs.push_back(cost);
  }
  return costs;
}

std::vector<unsigned> Expansion::compute_cost(node from, node to)
{
  auto costs = std::vector<unsigned>();
  auto dists = std::vector<signed>(g.n(), -1);
  auto queue = std::queue<node>();
  unsigned current_layer;

  queue.push(from);
  dists[from] = 0;
  current_layer = -1;

  while (!queue.empty())
  {
    auto node = queue.front();
    queue.pop();
    if (dists[node] != current_layer)
    {
      // new layer begins
      current_layer++;
      costs.push_back(0);
    }
    for (auto neigh : g.neighbors(node))
    {
      costs[current_layer]++;
      if (dists[neigh] == -1)
      {
        dists[neigh] = dists[node] + 1;
        queue.push(neigh);
        if (neigh == to)
          this->dist = dists[neigh];
      }
    }
  }
  return costs;
}

std::vector<double> Expansion::expansion(const std::vector<unsigned> &nums)
{
  auto res = std::vector<double>();
  for (unsigned i = 1; i < nums.size(); i++)
  {
    double ratio = (double)nums[i] / (double)nums[i - 1];
    res.push_back(ratio);
  }
  return res;
}
signed Expansion::get_dist()
{
  return this->dist;
}

ExpansionResult Expansion::evaluate(double base, double alpha)
{
  double m = this->g.m();
  double cheap = std::pow((double)m, alpha);

  auto cost_uni_s = this->uni_cost(this->cost_s);
  auto cost_uni_t = this->uni_cost(this->cost_t);
  auto cost_bi = this->bi_cost();

  // num of "cheap" exploration steps (>= 0)
  auto cheap_s_len = cheap_prefix_length(cheap, this->cost_s);
  cheap_s_len = std::min(cheap_s_len, static_cast<unsigned int>(this->dist));
  auto cheap_t_len = cheap_prefix_length(cheap, this->cost_t);
  cheap_t_len = std::min(cheap_t_len, static_cast<unsigned int>(this->dist));

  // num of "expanding" exploration steps (expansion > base)
  auto expan_s = high_prefix_length(base, this->exp_s);
  expan_s = std::min(expan_s, static_cast<unsigned int>(this->dist));
  auto expan_t = high_prefix_length(base, this->exp_t);
  expan_t = std::min(expan_t, static_cast<unsigned int>(this->dist));

  auto expan_s_prime = high_prefix_length_skip(base, this->exp_s, cheap_s_len);
  expan_s_prime = std::min(expan_s_prime, static_cast<unsigned int>(this->dist));
  auto expan_t_prime = high_prefix_length_skip(base, this->exp_t, cheap_t_len);
  expan_t_prime = std::min(expan_t_prime, static_cast<unsigned int>(this->dist));

  // relevant distance
  signed rel_dist = this->dist - cheap_s_len - cheap_t_len; // TODO?
  // expansion overlap
  signed overlap = expan_s + expan_t - this->dist; // TODO?

  // largest expansion between s and t
  double bplus_s = *std::max_element(this->exp_s.begin(), this->exp_s.begin() + this->dist);
  double bplus_t = *std::max_element(this->exp_t.begin(), this->exp_t.begin() + this->dist);
  double bplus = std::max(bplus_s, bplus_t);
  signed S1 = expan_s;
  signed S2 = this->dist - S1 - cheap_t_len;
  signed T1 = expan_t;
  signed T2 = this->dist - T1 - cheap_s_len;
  double rho_prime = (double)std::max(S2, T2) / (double)std::min(S1, T1);

  ExpansionResult res;
  res.b = base;
  res.alpha = alpha;
  res.s = this->s;
  res.t = this->t;
  res.cost_uni_s = cost_uni_s;
  res.cost_uni_t = cost_uni_t;
  res.cost_bi = cost_bi;
  res.dist = this->dist;
  res.rel_dist = rel_dist;
  res.expan_s = expan_s;
  res.expan_t = expan_t;
  res.expan_s_prime = expan_s_prime;
  res.expan_t_prime = expan_t_prime;
  res.cheap_s = cheap_s_len;
  res.cheap_t = cheap_t_len;
  res.overlap = overlap;
  res.T1 = T1;
  res.T2 = T2;
  res.S1 = S1;
  res.S2 = S2;
  res.bplus = bplus;
  res.rho_prime = rho_prime;

  return res;
}

double Expansion::calc_rho(signed expan_s, signed expan_t, signed cheap_s, signed cheap_t)
{
  signed S1 = expan_s; // std::max(expan_s, current_cheap_s);
  signed T1 = expan_t; // std::max(expan_t, current_cheap_t);

  signed S2 = this->dist - S1 - cheap_t;
  signed T2 = this->dist - T1 - cheap_s;

  signed high2 = std::max(S2, T2);
  signed low1 = std::min(S1, T1);

  double ratio = (double)high2 / (double)low1;
  return ratio;
}

double Expansion::rho_conv(double param)
{
  return 1.0 / (1.0 + param);
}

RhoExpansionResult Expansion::rho_evaluate(double base)
{
  // calculate expan_s and expan_t
  signed expan_s = static_cast<signed>(high_prefix_length(base, this->exp_s));
  expan_s = std::min(expan_s, this->dist);
  signed expan_t = static_cast<signed>(high_prefix_length(base, this->exp_t));
  expan_t = std::min(expan_t, this->dist);

  // get highest expanison bplus
  double bplus_s = *std::max_element(this->exp_s.begin(), this->exp_s.begin() + this->dist);
  double bplus_t = *std::max_element(this->exp_t.begin(), this->exp_t.begin() + this->dist);
  double bplus = std::max(bplus_s, bplus_t);

  RhoExpansionResult opt;
  opt.b = base;
  opt.bplus = bplus;
  opt.s = this->s;
  opt.t = this->t;
  opt.dist = this->dist;
  opt.optimal_alpha = 0.0;
  opt.optimal_S1 = 0;
  opt.optimal_S2 = 0;
  opt.optimal_T1 = 0;
  opt.optimal_T2 = 0;
  opt.optimal_cheap_s = 0;
  opt.optimal_cheap_t = 0;
  // rho and rhomax are already converted to [0, 1]
  opt.optimal_rho = 0;
  opt.optimal_rhomax = 0;

  signed current_cheap_s = 0;
  signed current_cheap_t = 0;
  double optimal_rhodiff = -std::numeric_limits<double>::infinity();

  while (current_cheap_s + current_cheap_t <= this->dist)
  {
    unsigned cum_s = cost_s_cum[current_cheap_s];
    unsigned cum_t = cost_t_cum[current_cheap_t];

    double alpha_s = 0;
    if (cum_s > 0)
      alpha_s = std::log(cum_s) / std::log(this->g.m());

    double alpha_t = 0;
    if (cum_t > 0)
      alpha_t = std::log(cum_t) / std::log(this->g.m());
    double alpha = std::max(alpha_s, alpha_t);

    signed S1 = expan_s; // std::max(expan_s, current_cheap_s);
    signed T1 = expan_t; // std::max(expan_t, current_cheap_t);

    signed S2 = this->dist - S1 - current_cheap_t;
    signed T2 = this->dist - T1 - current_cheap_s;

    signed high2 = std::max(S2, T2);
    signed low1 = std::min(S1, T1);

    double ratio = this->calc_rho(expan_s, expan_t, current_cheap_s, current_cheap_t);
    double converted = this->rho_conv(ratio);
    if (low1 == 0)
    {
      converted = 0.0;
    }
    if (high2 <= 0)
    {
      converted = 1.0;
    }

    double base_logratio = std::log(bplus) / std::log(base);
    double max_ratio = (1.0 - alpha) / (1.0 - alpha + alpha * base_logratio);
    double converted_max = this->rho_conv(max_ratio);
    double diff = converted - converted_max;

    if (diff > optimal_rhodiff)
    {
      opt.optimal_alpha = alpha;
      opt.optimal_S1 = S1;
      opt.optimal_S2 = S2;
      opt.optimal_T1 = T1;
      opt.optimal_T2 = T2;
      opt.optimal_cheap_s = current_cheap_s;
      opt.optimal_cheap_t = current_cheap_t;
      opt.optimal_rho = converted;
      opt.optimal_rhomax = converted_max;
      optimal_rhodiff = diff;
    }

    if (cost_s[current_cheap_s] <= cost_t[current_cheap_t])
      current_cheap_s++;
    else
      current_cheap_t++;
  }

  if (opt.optimal_rho == -std::numeric_limits<double>::infinity())
  {
    std::cerr << "optimal_rho is -infinity" << std::endl;

    // print opt
    std::cerr << "opt.b: " << opt.b << std::endl;
    std::cerr << "opt.bplus: " << opt.bplus << std::endl;
    std::cerr << "opt.s: " << opt.s << std::endl;
    std::cerr << "opt.t: " << opt.t << std::endl;
    std::cerr << "opt.dist: " << opt.dist << std::endl;
    std::cerr << "opt.optimal_alpha: " << opt.optimal_alpha << std::endl;
    std::cerr << "opt.optimal_S1: " << opt.optimal_S1 << std::endl;
    std::cerr << "opt.optimal_S2: " << opt.optimal_S2 << std::endl;
    std::cerr << "opt.optimal_T1: " << opt.optimal_T1 << std::endl;
    std::cerr << "opt.optimal_T2: " << opt.optimal_T2 << std::endl;
    std::cerr << "opt.optimal_cheap_s: " << opt.optimal_cheap_s << std::endl;
    std::cerr << "opt.optimal_cheap_t: " << opt.optimal_cheap_t << std::endl;
    std::cerr << "opt.optimal_rho: " << opt.optimal_rho << std::endl;
    std::cerr << "opt.optimal_rhomax: " << opt.optimal_rhomax << std::endl;
    std::cerr << "optimal_rhodiff: " << optimal_rhodiff << std::endl;
    std::cerr << "cost_s: ";
    for (auto c : cost_s)
      std::cerr << c << " ";
    std::cerr << std::endl;
    std::cerr << "cost_t: ";
    for (auto c : cost_t)
      std::cerr << c << " ";
    std::cerr << std::endl;

    exit(1);
  }

  return opt;
}

unsigned Expansion::uni_cost(std::vector<unsigned> &nums)
/**
 * Returns the cost of the unidirectional search by summing up the first dist
 * elements of nums.
 */
{
  unsigned res = 0;
  for (signed i = 0; i < this->dist; i++)
    res += nums[i];
  return res;
}

unsigned Expansion::bi_cost()
/**
 * Returns the cost of the balanced bidirectional search.
 */
{
  unsigned total_cost = 0;
  signed dist_s = 0;
  signed dist_t = 0;
  while (dist_s + dist_t < this->dist)
  {
    unsigned cost_s = this->cost_s[dist_s];
    unsigned cost_t = this->cost_t[dist_t];
    if (cost_s <= cost_t)
    {
      total_cost += cost_s;
      dist_s++;
    }
    else
    {
      total_cost += cost_t;
      dist_t++;
    }
  }
  return total_cost;
}

unsigned Expansion::cheap_prefix_length(unsigned threshold, std::vector<unsigned> &nums)
/**
 * Returns the length of the prefix of nums that is below threshold.
 */
{
  unsigned res = 0;
  unsigned sum = 0;
  for (unsigned i = 0; i < nums.size(); i++)
  {
    sum += nums[i];
    if (sum <= threshold)
      res++;
    else
      break;
  }
  return res;
}

unsigned Expansion::high_prefix_length(double threshold, std::vector<double> &nums)
/**
 * Returns the length of the prefix of nums that is above the threshold.
 */
{
  return high_prefix_length_skip(threshold, nums, 0);
}

unsigned Expansion::high_prefix_length_skip(double threshold, std::vector<double> &nums, unsigned skip)
{
  /**
   * Returns the length of the prefix of nums that includes the first skip
   * elements and all subsequent elements that are above the threshold.
   */
  unsigned res = skip;
  for (unsigned i = skip; i < nums.size(); i++)
  {
    if (nums[i] >= threshold)
      res++;
    else
      break;
  }
  return res;
}

std::vector<unsigned> Expansion::cumulative(std::vector<unsigned> &nums)
{
  std::vector<unsigned> res;
  unsigned sum = 0;
  res.push_back(sum);
  for (auto num : nums)
  {
    sum += num;
    res.push_back(sum);
  }
  return res;
}