#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <random>

#include "module.hpp"

std::random_device rd;
std::mt19937 gen(rd());

void read_ckt(std::ifstream& inFile,
	      std::vector<module*>& modules)
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
    module *currentModule = new module(moduleWidth, moduleHeight, moduleNum);
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
    for (auto j : edgeVec)
      modules[j]->pushSet(edgeVec);
  }
}

void printUsage()
{

}
