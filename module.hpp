#ifndef MODULE_HPP
#define MODULE_HPP

#include <set>
#include <vector>

enum GraphType {HCG, VCG};

class module {
private:
  int label;
  int width;
  int height;
  std::set<int> adjSet;

  int X;
  int Y;
public:
  static int count;
  // flags used for topo sort
  module(int l, int w, int h): label(l), width(w), height(h) {}
  int getWidth() const {
    return width;
  }
  int getHeight() const {
    return height;
  }
  int getX() const {
    return X;
  }
  int getY() const {
    return Y;
  }
  void setCoordinate(int _wC, int _hC) {
    X = _wC;
    Y = _hC;
  }
  void pushSet(const std::vector<int>& hyperEdge);
  int HPWL(const std::vector<module*>& modules);
};




#endif
