#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <random>
#include <cmath>
#include <algorithm>

#include "module.hpp"
#include "graph.hpp"

#define MAX_TEMP 4e4
#define FRZ_TEMP 0.1
#define INIT_RATE 0.95
#define COOL_RATE 0.95

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> dis(0,1);

void read_ckt(std::ifstream& inFile,
	      std::vector<module*>& modules,
	      std::vector<hypernet*>& nets)
{
  std::string line;
  std::getline(inFile, line);
  int numModules = std::stoi(line);
  modules.clear();
  modules.resize(numModules, nullptr);
  module::count = numModules;
  std::cout << "Total:\t" << numModules << " Detected." << std::endl;
  // read all modules
  for (auto i = 0; i < numModules; ++i) {
    std::getline(inFile, line);
    std::istringstream lineStream(line);
    int moduleNum, moduleWidth, moduleHeight;
    lineStream >> moduleNum >> moduleWidth >> moduleHeight;
    module *currentModule = new module(moduleNum, moduleWidth, moduleHeight);
    modules[moduleNum] = currentModule;
  }
  // read nets
  std::getline(inFile, line);
  if (!(line == "Nets" || line == "Nets\r")) {
    std::cerr << "Error reading circuit file." << std::endl;
    exit(1);
  }
  std::getline(inFile, line);
  int numEdges = std::stoi(line);
  for (auto i = 0; i < numEdges; ++i) {
    std::getline(inFile, line);
    std::vector<int> edgeVec;
    std::istringstream lineStream(line);
    // first number is size, others are edge elements
    int numElements;
    lineStream >> numElements;
    edgeVec.resize(numElements, -1);
    for (auto j = 0; j < numElements; ++j) 
      lineStream >> edgeVec[j];
    // push the current edge to each element in it
    hypernet *ptr = new hypernet(numElements, edgeVec);
    nets.push_back(ptr);
  }
}

bool accept_move(float dCost,
		 float k,
		 float T)
{
  if (dCost < 0) return true;
  float boltz = std::exp(-dCost/(k*T));
  float r = dis(gen);
  return r < boltz;
}

float cost(float HPWL, int area, float alpha)
{
  return alpha * area + (1 - alpha) * HPWL;
}


float kboltz(graph& myGraph,
	     const float initHPWL,
	     const int initArea,
	     const float alpha,
	     std::pair<std::vector<int>, std::vector<int>>& SP)
{
  auto& GammaP = SP.first;
  auto& GammaN = SP.second;
  int num_modules = module::count;
  float initCost = cost(initHPWL, initArea, alpha);
  int i = 0;
  float avgdCost = 0;
  const int attempts = 50; // measure 50 increasing steps
  while (i < 50) {
    int module1, module2;
    module1 = gen() % num_modules;
    do {
      module2 = gen() % num_modules;
    } while (module2 == module1);
    // type of operation 0 switch both 1 switch P 2 switch N
    int optype = gen()%3;
    auto m1IterP = std::find(GammaP.begin(), GammaP.end(), module1);
    auto m1IterN = std::find(GammaN.begin(), GammaP.end(), module1);
    auto m2IterP = std::find(GammaP.begin(), GammaP.end(), module2);
    auto m2IterN = std::find(GammaN.begin(), GammaP.end(), module2);
    // swap and update HCG VCG
    if (optype == 0 || optype == 1) {
      int temp = *m1IterP;
      *m1IterP = *m2IterP;
      *m2IterP = temp;
    }
    if (optype == 0 || optype == 2) {
      int temp = *m1IterN;
      *m1IterN = *m2IterN;
      *m2IterN = temp;
    }
    myGraph.update_graph(module1, SP);
    myGraph.update_graph(module2, SP);
    myGraph.pack();
    float currentHPWL = myGraph.getHPWL();
    int currentArea = myGraph.getArea();
    float currentCost = cost(currentHPWL, currentArea, alpha);
    float dCost = currentCost - initCost;
    if (dCost > 0) {
      avgdCost += dCost;
      ++i;
    }
    // swap and update HCG VCG
    if (optype == 0 || optype == 1) {
      int temp = *m1IterP;
      *m1IterP = *m2IterP;
      *m2IterP = temp;
    }
    if (optype == 0 || optype == 2) {
      int temp = *m1IterN;
      *m1IterN = *m2IterN;
      *m2IterN = temp;
    }
    myGraph.update_graph(module1, SP);
    myGraph.update_graph(module2, SP);
  }
  avgdCost /= attempts;
  return 0 - avgdCost / (std::log(INIT_RATE)*MAX_TEMP);
}

void annealing(graph& myGraph,
	       const float k,
	       const int num_moves,
	       const float alpha,
	       const float initHPWL,
	       const int initArea,
	       std::ofstream& outFile,
	       std::pair<std::vector<int>, std::vector<int>>& SP)
{
  auto& GammaP = SP.first;
  auto& GammaN = SP.second;
  float currentHPWL = initHPWL;
  int currentArea = initArea;
  float currentCost = cost(initHPWL, initArea, alpha);
  int num_modules = module::count;
  float T = MAX_TEMP;
  while (T > FRZ_TEMP) {
    int accepted_moves = 0, rejected_moves = 0;
    for (int i = 0; i < num_moves; ++i) {
      int module1, module2;
      module1 = gen() % num_modules;
      do {
	module2 = gen() % num_modules;
      } while (module2 == module1);
      // type of operation 0 switch both 1 switch P 2 switch N
      int optype = gen()%3;
      auto m1IterP = std::find(GammaP.begin(), GammaP.end(), module1);
      auto m1IterN = std::find(GammaN.begin(), GammaP.end(), module1);
      auto m2IterP = std::find(GammaP.begin(), GammaP.end(), module2);
      auto m2IterN = std::find(GammaN.begin(), GammaP.end(), module2);
      if (optype == 0 || optype == 1) {
	int temp = *m1IterP;
	*m1IterP = *m2IterP;
	*m2IterP = temp;
      }
      if (optype == 0 || optype == 2) {
	int temp = *m1IterN;
	*m1IterN = *m2IterN;
	*m2IterN = temp;
      }
      myGraph.update_graph(module1, SP);
      myGraph.update_graph(module2, SP);
      myGraph.pack();
      float newHPWL = myGraph.getHPWL();
      int newArea = myGraph.getArea();
      float newCost = cost(newHPWL, newArea, alpha);
      if (accept_move(newCost - currentCost, k, T)) {
	currentHPWL = newHPWL;
	currentArea = newArea;
	currentCost = newCost;
	++accepted_moves;
      } else { // not accepted, swap back
	if (optype == 0 || optype == 1) {
	  int temp = *m1IterP;
	  *m1IterP = *m2IterP;
	  *m2IterP = temp;
	}
	if (optype == 0 || optype == 2) {
	  int temp = *m1IterN;
	  *m1IterN = *m2IterN;
	  *m2IterN = temp;
	}
	myGraph.update_graph(module1, SP);
	myGraph.update_graph(module2, SP);
	++rejected_moves;
      }
    }
    std::cout << "Current Temperature:" << T << std::endl;
    outFile << T << "," << accepted_moves << ","
	    << rejected_moves << ","
	    << currentArea << ","
	    << currentHPWL << ","
	    << currentCost << std::endl;
    T *= COOL_RATE;
  }
}

void printUsage()
{
  std::cerr << "USAGE: ./floorplan -<option> [filepath] " << std::endl;
  std::cerr << "Options: -a   floorplan based only on area" << std::endl
	    << "         -w   floorplan baesd only on wire length" << std::endl
	    << "         -c   floorplan based on both wire length and area"
	    << std::endl;
}

void printCoordinate(const std::vector<module*>& modules,
		     std::ofstream& outFile)
{
  outFile << "label\tX\t\tY" << std::endl;
  for (auto i: modules)
    outFile << i->getLabel() << "\t\t"
	    << i->getX() << "\t\t"
	    << i->getY() << std::endl;
}
