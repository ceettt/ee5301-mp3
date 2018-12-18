#include <algorithm>
#include <iostream>

#include "graph.hpp"
#include "module.hpp"

void graph::construct_graph(const std::pair<std::vector<int>,
			    std::vector<int>>& SP)
{
  const std::vector<int>& GammaP = SP.first;
  const std::vector<int>& GammaN = SP.second;
  // assign
  for (unsigned int i = 0; i < size; ++i) {
    //set weight
    int width = modules[i]->getWidth();
    int height = modules[i]->getHeight();
    // set adjMatrix
    for (unsigned int j = 0; j < size; ++j) {
      HCG[i][j] = -1;
      VCG[i][j] = -1;
      if (i != j) {
	auto iIterP = std::find(GammaP.cbegin(), GammaP.cend(), i);
	auto iIterN = std::find(GammaN.cbegin(), GammaN.cend(), i);
	auto jIterP = std::find(GammaP.cbegin(), GammaP.cend(), j);
	auto jIterN = std::find(GammaN.cbegin(), GammaN.cend(), j);
	if ((iIterP < jIterP) && (iIterN < jIterN))
	  HCG[i][j] = width;
	else if ((iIterP > jIterP) && (iIterN < jIterN))
	  VCG[i][j] = height;
      }
    }
  }
}

void graph::update_graph(unsigned int i,
			 const std::pair<std::vector<int>,
			 std::vector<int>>& SP)
{
  const std::vector<int>& GammaP = SP.first;
  const std::vector<int>& GammaN = SP.second;

  auto iIterP = std::find(GammaP.cbegin(), GammaP.cend(), i);
  auto iIterN = std::find(GammaN.cbegin(), GammaN.cend(), i);

  for (unsigned int j = 0; j < size; ++j) {
    if (i != j) {
      HCG[i][j] = -1;
      VCG[i][j] = -1;
      HCG[j][i] = -1;
      VCG[j][i] = -1;
      auto jIterP = std::find(GammaP.cbegin(), GammaP.cend(), j);
      auto jIterN = std::find(GammaN.cbegin(), GammaN.cend(), j);
      if ((iIterP < jIterP) && (iIterN < jIterN))
	HCG[i][j] = modules[i]->getWidth();
      else if ((iIterP > jIterP) && (iIterN > jIterN))
	HCG[j][i] = modules[j]->getWidth();
      else if ((iIterP > jIterP) && (iIterN < jIterN))
	VCG[i][j] = modules[i]->getHeight();
      else if ((iIterP < jIterP) && (iIterN > jIterN))
	VCG[j][i] = modules[j]->getHeight();
    }
  }
}
// topological visit a node and return the longest path of current node
int graph::visit(int i, bool isHorizonal)
{
  std::vector<std::vector<int>> &graph = isHorizonal? HCG : VCG;
  std::vector<int> &lPathVector = isHorizonal? wC : hC;
  if (fPerm[i] == 1) {
    return lPathVector[i];
  }
  if (fTemp[i] == 1) {
    std::cerr << "Current MAP contain LOOP. Exiting." << std::endl;
    exit(1);
  }
  fTemp[i] = 1;
  int longest_path = 0;
  for (unsigned int j = 0; j < size; ++j) {
    if (graph[j][i] != -1) { // there is an edge
      int current_path = graph[j][i] + visit(j, isHorizonal);
      longest_path = std::max(current_path, longest_path);
    }
  }
  fTemp[i] = 0;
  fPerm[i] = 1;
  lPathVector[i] = longest_path;
  return longest_path;
}

void graph::pack()
{
  // reset all flags and coordinate
  std::fill(fTemp.begin(), fTemp.end(), 0);
  std::fill(fPerm.begin(), fPerm.end(), 0);
  std::fill(wC.begin(), wC.end(), -1);
  std::fill(hC.begin(), hC.end(), -1);

  // toposort the HCG and VCG and get longest path
  chipWidth = 0;
  for (unsigned int i = 0; i < size; ++i) {
    int current_path = modules[i]->getWidth() + visit(i, true);
    chipWidth = std::max(current_path, chipWidth);
  }
  chipHeight = 0;
  std::fill(fTemp.begin(), fTemp.end(), 0);
  std::fill(fPerm.begin(), fPerm.end(), 0);
  for (unsigned int i = 0; i < size; ++i) {
    int current_path = modules[i]->getHeight() + visit(i, false);
    // assign coordinate
    modules[i]->setCoordinate(wC[i], hC[i]);
    chipHeight = std::max(current_path, chipHeight);
  }
}

