#include <iostream>

#include "graph.h"

using namespace std;

ostream &operator<<(ostream &os, Node const &m) {
  os << m.id << " ->";
  for (int i = 0; i < m.neighbors.size(); i += 1) {
    os << ' ' << m.neighbors[i].nodePtr->id
       << "(" << edges[m.neighbors[i].edgeId].getFund(m.id) << ")";
  }
  return os;
}

ostream &operator<<(ostream &os, Graph const &m) {
  os << "node: " << m.nodeNum << endl;
  os << "link:" << endl;
  for (int i = 0; i < m.nodeNum; i += 1) {
    os << m._nodes[i] << endl;
  }
  os << endl;
  return os;
}
