#include <vector>
#include <set>
#include <algorithm>

#include "module.hpp"

int module::count = 0;

int hypernet::DoubleHPWL(const std::vector<module*>& modules)
{
  int minX = 100000000, maxX = 0, minY = 100000000, maxY = 0;
  for (auto i : nodes) {
    minX = std::min(modules.at(i)->getDCX(), minX);
    maxX = std::max(modules.at(i)->getDCX(), maxX);
    minY = std::min(modules.at(i)->getDCY(), minY);
    maxY = std::max(modules.at(i)->getDCY(), maxY);
  }
  return maxX - minX + maxY - minY;
}
