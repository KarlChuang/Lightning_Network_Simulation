#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

#define FEE_DEFAULT   1
#define FEE_OPTIMIZED 2


using namespace std;

class Node {
  friend ostream &operator<<(ostream &, Node const &);
  struct Neighbor {
    Node* nodePtr;
    double fund;
  };

public:
  int id;
  vector<Neighbor> neighbors;
  void setId(int newId) { id = newId; }
  void addNeighbor(Node*, double);
  
  // Method for routing algorithm
  double accWeight;
  int fromNode;
  double fromFee;
  void setNeighborWeight(double, int);
};

class Graph {
  friend ostream &operator<<(ostream &, Graph const &);
  Node* _nodes;
  double totalImbalance;
  double totalFund;

public:
  int nodeNum;
  Graph(int, double, double, double, int, int);
  ~Graph() { delete [] _nodes; }
  void resetNodesAccWeight();
  bool traverse(int, int, double, int);
  bool sendPayment(int, int, double, int);
  double getImbalanceRatio() { return totalImbalance / totalFund; }
};

double getFee(double, int, double, double);
ostream &operator<<(ostream &, Node const &);
ostream &operator<<(ostream &, Graph const &);
