#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>

#define FEE_DEFAULT   1
#define FEE_OPTIMIZED 2

using namespace std;

class Edge {
public:
  int id1;
  double fund1;
  int id2;
  double fund2;
  Edge(int, double, int, double);
  double getFund(int);
  double getImbalance();
  double getTotal() { return fund1 + fund2; }
  void transferFund(int, double, double);
  int getAnotherId(int);
};

extern vector<Edge> edges;

class Node {
  friend ostream &operator<<(ostream &, Node const &);
  struct Neighbor {
    Node* nodePtr;
    int edgeId;
  };

public:
  int id;
  int part;
  vector<Neighbor> neighbors;
  void setId(int newId) { id = newId; }
  void setPart(int partId) { part = partId; }
  void addNeighbor(Node*, int);
  
  // Method for routing algorithm
  double accWeight;
  int toEdgeId;
  double toFee;
  void setNeighborWeight(double, int, int, double&);
  void setNeighborPart();
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
  int sendPayment(int, int, double, int);
  double getImbalanceRatio();
};

extern double fee_low;
extern double fee_high;
extern double fee_default;
double getFee(double, int, double, double);
ostream &operator<<(ostream &, Node const &);
ostream &operator<<(ostream &, Graph const &);
