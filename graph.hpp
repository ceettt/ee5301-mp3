#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <utility>
#include "module.hpp"

class graph {
private:
  std::size_t size;
  std::vector<std::vector<int>> HCG, VCG; // adjMatrix
  std::vector<int> fTemp, fPerm, wC, hC;
  std::vector<module*> modules;
  std::vector<hypernet*> nets;
  int chipWidth;
  int chipHeight;

  int visit(int i, bool isHorizonal);
public:
  graph(std::size_t __size,
	const std::vector<module*>& __m,
	const std::vector<hypernet*>& __n): size(__size) {
    modules = __m;
    nets = __n;
    HCG.resize(__size);
    VCG.resize(__size);
    fTemp.resize(__size);
    fPerm.resize(__size);
    wC.resize(__size);
    hC.resize(__size);
    for (std::size_t i = 0; i < __size; ++i) {
      HCG[i].resize(__size);
      VCG[i].resize(__size);
    }
  }
  ~graph() {
    HCG.clear();
    VCG.clear();
  }
  void construct_graph(const std::pair<std::vector<int>, std::vector<int>>& SP);
  void update_graph(unsigned int i,
		    const std::pair<std::vector<int>, std::vector<int>>& SP);
  void pack();
  int getWidth() const {
    return chipWidth;
  }
  int getHeight() const {
    return chipHeight;
  }
  int getArea() const {
    return chipWidth * chipHeight;
  }
  float getHPWL() const {
    int DHPWL = 0;
    for (auto i: nets)
      DHPWL += i->DoubleHPWL(modules);
    return (float)DHPWL / 2.0;
  }
};




#endif
