#ifndef UTIL_HPP
#define UTIL_HPP

void read_ckt(std::ifstream& inFile,
	      std::vector<module*>& modules);

void printUsage();


float kboltz(graph& myGraph,
	     const int initHPWL,
	     const int initArea,
	     const float alpha,
	     std::pair<std::vector<int>, std::vector<int>>& SP);

void printCoordinate(const std::vector<module*>& modules,
		     std::ofstream& outFile);

void annealing(graph& myGraph,
	       const float k,
	       const int num_moves,
	       const float alpha,
	       const int initHPWL,
	       const int initArea,
	       std::ofstream& outFile,
	       std::pair<std::vector<int>, std::vector<int>>& SP);
#endif
