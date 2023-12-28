#include <iostream>

#include "CLI11.hpp"
#include "graph.hpp"
#include "component_structure.hpp"
#include "expansion.hpp"
#include "timer.hpp"
#include "random.hpp"

std::vector<double> parse_double_list(std::string str)
{
  std::vector<double> res;
  std::stringstream ss(str);
  double num;
  while (ss >> num)
  {
    res.push_back(num);
    if (ss.peek() == ',')
    {
      ss.ignore();
    }
  }
  return res;
}

int main(int argc, char **argv)
{
  CLI::App app{"Beating the Worst Case -- Expansion"};

  std::string file_name;
  app.add_option("input_file", file_name,
                 "Name of the file containing the graph")
      ->required()
      ->check(CLI::ExistingFile);

  unsigned samples = 10;
  app.add_option("-s,--samples", samples, "Number of samples")
      ->default_val(10);

  std::string base_str = "2.0";
  app.add_option("-b,--base", base_str, "Expansion base (str, comma separated)")
      ->default_val("2.0");

  std::string alpha_str = "0.0";
  app.add_option("-a,--alpha", alpha_str, "Alpha value (str, comma separated)")
      ->default_val("0.0");

  bool no_header = false;
  app.add_flag("--noheader", no_header, "Do not print header");

  bool only_header = false;
  app.add_flag("--onlyheader", only_header, "Print only header");

  bool rho_eval = false;
  app.add_flag("--rhoeval", rho_eval, "Evaluate optimal rho");

  if (argc == 1)
  {
    file_name = "../../instances/medium_real/inf-openflights.edges";
    samples = 1000;
    base_str = "2.0";
    alpha_str = "0.0";
    rho_eval = true;
  }
  else
  {
    CLI11_PARSE(app, argc, argv);
  }

  auto bases = parse_double_list(base_str);
  auto alphas = parse_double_list(alpha_str);

  if (!no_header)
  {
    if (rho_eval)
    {
      std::cout << RhoExpansionResult::CSVHeader() << std::endl;
    }
    else
    {
      std::cout << ExpansionResult::CSVHeader() << std::endl;
    }
  }
  if (only_header)
  {
    return 0;
  }

  Graph G(file_name);
  ComponentStructure CS(G);

  Graph H(G, CS.nodes_of_component(CS.largest_component()));

  for (unsigned int i = 0; i < samples; i++)
  {
    unsigned s = 0;
    unsigned t = 0;
    while (s == t)
    {
      s = Random::natural_number(H.n() - 1);
      t = Random::natural_number(H.n() - 1);
    }
    auto exp = Expansion(H, s, t);
    if (rho_eval)
    {
      for (auto base : bases)
      {
        auto res = exp.rho_evaluate(base);
        std::cout << res.toCSVString() << std::endl;
      }
    }
    else
    {
      for (auto base : bases)
      {
        for (auto alpha : alphas)
        {
          auto res = exp.evaluate(base, alpha);
          std::cout << res.toCSVString() << std::endl;
        }
      }
    }

  }

  return 0;
}