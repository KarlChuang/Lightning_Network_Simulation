#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

#include "graph.h"
#include "simlib.h"

using namespace std;

vector<Edge> edges;

Edge::Edge(int i1, double f1, int i2, double f2) {
  id1 = i1;
  fund1 = f1;
  id2 = i2;
  fund2 = f2;
}

double Edge::getFund(int fromId) {
  if (fromId == id1) return fund1;
  else if (fromId == id2) return fund2;
  else {
    cout << "edge get fund error" << endl;
    exit(1);
  }
}

double Edge::getImbalance() {
  double imbalance = fund1 - fund2;
  if (imbalance < 0) imbalance = -imbalance;
  return imbalance;
}

void Edge::transferFund(int fromId, double transfer, double fee) {
  if (fromId == id1) {
    fund1 -= (fee + transfer);
    fund2 += (fee + transfer);
  } else if (fromId == id2) {
    fund1 += (fee + transfer);
    fund2 -= (fee + transfer);
  } else {
    cout << "edge add fund error" << endl;
    exit(1);
  }
}

int Edge::getAnotherId(int id) {
  if (id1 == id) return id2;
  else if (id2 == id) return id1;
  else {
    cout << "edge get another node id error" << endl;
    exit(1);
  }
  return -1;
}

void Node::addNeighbor(Node* nPtr, int edgeId) {
  Node::Neighbor newNeighbor = { nPtr, edgeId };
  neighbors.push_back(newNeighbor);
}

Graph::Graph(int num_n, double prob_channel, double min_channel_fund, double max_channel_fund, int s1, int s2) {
  nodeNum = num_n;
  totalImbalance = 0.0;
  totalFund = 0.0;

  // set _nodes
  _nodes = new Node[nodeNum];

  // set node ID
  for (int i = 0; i < nodeNum; i += 1) {
    _nodes[i].setId(i);
  }

  // set node neightbors and fund
  for (int i = 0; i < nodeNum; i += 1) {
    for (int j = i + 1; j < nodeNum; j += 1) {
      if (uniform(0.0, 1.0, s1) < prob_channel) {
        double fund1 = uniform(min_channel_fund, max_channel_fund, s2);
        double fund2 = uniform(min_channel_fund, max_channel_fund, s2);
        Edge edge(i, fund1, j, fund2); 
        edges.push_back(edge);
        _nodes[i].addNeighbor(&_nodes[j], edges.size() - 1);
        _nodes[j].addNeighbor(&_nodes[i], edges.size() - 1);
        double imbalance = fund1 - fund2;
        totalImbalance += ((imbalance > 0) ? imbalance : -imbalance);
        totalFund += (fund1 + fund2);
      }
    }
  }
}

void Graph::resetNodesAccWeight() {
  for (int i = 0; i < nodeNum; i += 1) {
    _nodes[i].accWeight = double(INT_MAX);
    _nodes[i].toEdgeId = -1;
    _nodes[i].toFee = 0.0;
  }
}

double getFee(double transfer, int type, double fromFund, double toFund) {
  if (fromFund < transfer) {
    return double(INT_MAX);
  }
  if (type == FEE_DEFAULT) {
    return 0.00000001 + 0.0000000003 * (transfer / 0.00000001);
  }
  if (type == FEE_OPTIMIZED) {
    if (fromFund <= toFund) {
      return 0.00000001 + 0.0000000003 * (transfer / 0.00000001);
    } else {
      double imbalance = fromFund - toFund;
      if (transfer <= (imbalance / 2)) {
        return 0.00000001 + 0.0000000001 * (transfer / 0.00000001);
      } else {
        return 0.00000001 + 0.0000000001 * ((imbalance / 2) / 0.00000001)
               + 0.0000000003 * ((transfer - (imbalance / 2)) / 0.00000001);
      }
    }
  }
  return double(INT_MAX);
}

void Node::setNeighborWeight(double transfer, int type, int from, double& reachAcc) {
  if (accWeight >= reachAcc) return;
  if (id == from) reachAcc = accWeight;
  for (int i = 0; i < neighbors.size(); i += 1) {
    if (accWeight >= neighbors[i].nodePtr->accWeight) continue;
    double toFund = edges[neighbors[i].edgeId].getFund(id);
    double fromFund = edges[neighbors[i].edgeId].getFund(neighbors[i].nodePtr->id);
    double fee = getFee(transfer, type, fromFund, toFund);
    if (accWeight + fee < neighbors[i].nodePtr->accWeight) {
      neighbors[i].nodePtr->accWeight = accWeight + fee;
      neighbors[i].nodePtr->toEdgeId = neighbors[i].edgeId;
      neighbors[i].nodePtr->toFee = fee;
      neighbors[i].nodePtr->setNeighborWeight(transfer + fee, type, from, reachAcc);
    }
  }
}

double Graph::getImbalanceRatio() {
  if (totalImbalance < 0 || totalFund < 0) {
    cout << "imbalance ratio error!" << endl;
    exit(1);
  }
  return totalImbalance / totalFund;
}

bool Graph::traverse(int from, int to, double transfer, int type) {
  resetNodesAccWeight();
  _nodes[to].accWeight = 0.0;
  double reachAcc = double(INT_MAX);
  _nodes[to].setNeighborWeight(transfer, type, from, reachAcc);
  if (_nodes[from].accWeight >= double(INT_MAX / 2)) return false;
  return true; // return can go to the node or not
}

bool Graph::sendPayment(int from, int to, double transfer, int type) {
  if (from < 0 || from >= nodeNum || to < 0 || to >= nodeNum) {
    cout << "Send payment node number error" << endl;
    exit(1);
  }

  // find optimal path
  if(!traverse(from, to, transfer, type)) {
    return false;
  }

  // set the network state according to optinal path
  Node* nPtr = &_nodes[from];
  while (1) {
    if (nPtr->toEdgeId < 0) break;
    double accFee = nPtr->accWeight;
    double fee = nPtr->toFee;
    double preImbalance = edges[nPtr->toEdgeId].getImbalance();
    edges[nPtr->toEdgeId].transferFund(nPtr->id, transfer + accFee - fee, fee);
    double newImbalance = edges[nPtr->toEdgeId].getImbalance();
    int nextId = edges[nPtr->toEdgeId].getAnotherId(nPtr->id);
    nPtr = &_nodes[nextId];
    totalImbalance += (newImbalance - preImbalance);
  }
  return true;
}
