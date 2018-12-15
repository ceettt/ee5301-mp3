#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <chrono>
#include <exception>
#include <random>
#include <algorithm>
#include <utility>
#include <numeric>

#include "graph.hpp"
#include "module.hpp"
#include "util.hpp"

extern std::mt19937 gen;

const float preset_alpha = 0.5;

int main(const int argc, const char *argv[])
{
  using Time = std::chrono::high_resolution_clock;
  using us = std::chrono::microseconds;
  using fsec = std::chrono::duration<float>;
  
  std::vector<std::string> args(argv, argv+argc);

  std::vector<module*> modules;
  
  auto start = Time::now();

  float alpha;
  
  try {
    // set alpha
    if (args.at(1) == "-a") 
      alpha = 1;
    else if (args.at(1) == "-w")
      alpha = 0;
    else if (args.at(1) == "-c")
      alpha = preset_alpha;
    else {
      printUsage();
      exit(1);
    }
    alpha = alpha;
    std::ifstream ckt_file(args.at(2));
    if (!ckt_file.is_open()) {
      std::cout << "Cannot open file:\t" << args.at(1) << std::endl;
      exit(1);
    }
    read_ckt(ckt_file, modules);
    ckt_file.close();
    // generate random sequence pair
    std::vector<int> GammaP(module::count);
    // fill with 0~count-1
    std::iota(GammaP.begin(), GammaP.end(), 0);
    std::vector<int> GammaN(GammaP);
    // shuffle the two vector to get the sequence pair
    std::shuffle(GammaP.begin(), GammaP.end(), gen);
    std::shuffle(GammaN.begin(), GammaN.end(), gen);
    auto SP = std::make_pair(GammaP, GammaN);
    graph myGraph(modules.size(), modules);
    myGraph.construct_graph(SP);
    myGraph.pack();
    std::cout << "Initial area: " << myGraph.getArea() << std::endl
	      << "Initial HPWL: " << myGraph.getHPWL() << std::endl;
    
  } catch (const std::out_of_range& e) {
    std::cerr << "Not enough parameters." << std::endl;
    printUsage();
    exit(1);
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    exit(1);
  }

  // timing
  auto stop = Time::now();
  fsec fs = stop - start;
  us d = std::chrono::duration_cast<us>(fs);
  std::cout << "Program took \t" << fs.count() << "s" << std::endl
	    << "\tor \t" << d.count() << "us" << std::endl;
  return 0;
}
