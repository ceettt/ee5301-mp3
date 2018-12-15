#include <vector>
#include <set>
#include <algorithm>

#include "module.hpp"

int module::count = 0;

void module::pushSet(const std::vector<int>& hyperEdge)
{
  // insert each element to set
  for (auto i : hyperEdge)
    adjSet.insert(i);
}

int module::HPWL(const std::vector<module*>& modules)
{
  int minX = X, maxX = X, minY = Y, maxY = Y;
  for (auto i : adjSet) {
    minX = std::min(modules[i]->getX(), minX);
    maxX = std::max(modules[i]->getX(), maxX);
    minY = std::min(modules[i]->getY(), minY);
    maxY = std::max(modules[i]->getY(), maxY);
  }
  return maxX - minX + maxY - minY;
}
