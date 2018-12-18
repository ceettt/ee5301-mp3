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
  std::vector<hypernet*> nets;
  std::string result_filename = "_Chen_Tonglin.out2";
  std::string step_filename = "step.csv";
  
  auto start = Time::now();

  float alpha;
  
  try {
    result_filename = args.at(2) + result_filename;
    // set alpha
    if (args.at(1) == "-a") {
      result_filename += "a";
      alpha = 1;
    }
    else if (args.at(1) == "-w") {
      result_filename += "w";
      alpha = 0;
    }
    else if (args.at(1) == "-c") {
      result_filename += "c";
      alpha = preset_alpha;
    }
    else {
      printUsage();
      exit(1);
    }
    alpha = alpha;
    std::ifstream ckt_file(args.at(2));
    if (!ckt_file.is_open()) {
      std::cerr << "Cannot open file:\t" << args.at(2) << std::endl;
      exit(1);
    }
    read_ckt(ckt_file, modules, nets);
    ckt_file.close();
    std::ofstream result_file(result_filename);
    if (!result_file.is_open()) {
      std::cerr << "Cannot open file:\t" << result_filename << std::endl;
      exit(1);
    }
    std::ofstream step_file(step_filename);
    if (!step_file.is_open()) {
      std::cerr << "Cannot open file:\t" << result_filename << std::endl;
      exit(1);
    }
    // generate random sequence pair
    std::vector<int> GammaP(module::count);
    // fill with 0~count-1
    std::iota(GammaP.begin(), GammaP.end(), 0);
    std::vector<int> GammaN(GammaP);
    // shuffle the two vector to get the sequence pair
    std::shuffle(GammaP.begin(), GammaP.end(), gen);
    std::shuffle(GammaN.begin(), GammaN.end(), gen);
    auto SP = std::make_pair(GammaP, GammaN);
    graph myGraph(modules.size(), modules, nets);
    myGraph.construct_graph(SP);
    myGraph.pack();
    int initArea = myGraph.getArea();
    float initHPWL = myGraph.getHPWL();
    result_file << "Initial area:  " << initArea << std::endl
		<< "Initial HPWL:  " << initHPWL << std::endl
		<< "Initial Width: " << myGraph.getWidth() << std::endl
		<< "Initial Height:" << myGraph.getHeight() << std::endl;
    step_file << "Temperature,accepted_moves,rejected_moves,area,HPWL,Cost"
	      << std::endl;
    float k = kboltz(myGraph, initHPWL, initArea, alpha, SP);
    std::cout << "Sampled k:" << k << std::endl;
    result_file << "Sampled k:" << k << std::endl;
    annealing(myGraph, k, modules.size(), alpha, initHPWL, initArea,
	      step_file, SP);
    result_file << std::endl << "Final Width: " << myGraph.getWidth()
		<< std::endl << "Final Height:" << myGraph.getHeight()
		<< std::endl << "Final Area:  " << myGraph.getArea()
		<< std::endl << "Final HPWL:  " << myGraph.getHPWL()
		<< std::endl << std::endl;
    printCoordinate(modules, result_file);
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
