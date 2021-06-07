#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

#include "graph.h"
#include "simlib.h"

using namespace std;

void Node::addNeighbor(Node* nPtr, double fund) {
  Node::Neighbor newNeighbor = { nPtr, fund };
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
        _nodes[i].addNeighbor(&_nodes[j], fund1);
        double fund2 = uniform(min_channel_fund, max_channel_fund, s2);
        _nodes[j].addNeighbor(&_nodes[i], fund2);
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
    _nodes[i].fromNode = -1;
    _nodes[i].fromFee = 0.0;
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
        int fee_unit = int(transfer / 0.00000001);
        return 0.00000001 + 0.0000000001 * ((imbalance / 2) / 0.00000001)
               + 0.0000000003 * ((transfer - (imbalance / 2)) / 0.00000001);
      }
    }
  }
  return double(INT_MAX);
}

void Node::setNeighborWeight(double transfer, int type) {
  for (int i = 0; i < neighbors.size(); i += 1) {
    double fundTo;
    int nodeIdx = -1;
    for (int j = 0; j <= neighbors[i].nodePtr->neighbors.size(); j += 1) {
      if (neighbors[i].nodePtr->neighbors[j].nodePtr == this) {
        fundTo = neighbors[i].nodePtr->neighbors[j].fund;
        nodeIdx = j;
      }
    }
    if (nodeIdx == -1) {
      cout << "Find neighbor error" << endl;
      exit(1);
    }
    double fee = getFee(transfer, type, neighbors[i].fund, fundTo);
    if (accWeight + fee < neighbors[i].nodePtr->accWeight) {
      neighbors[i].nodePtr->accWeight = accWeight + fee;
      neighbors[i].nodePtr->fromNode = nodeIdx;
      neighbors[i].nodePtr->fromFee = fee;
      neighbors[i].nodePtr->setNeighborWeight(transfer, type);
    }
  }
}

bool Graph::traverse(int from, int to, double transfer, int type) {
  resetNodesAccWeight();
  _nodes[from].accWeight = 0.0;
  _nodes[from].setNeighborWeight(transfer, type);
  if (_nodes[to].accWeight >= double(INT_MAX / 2)) return false;
  return true; // return can go to the node or not
}

bool Graph::sendPayment(int from, int to, double transfer, int type) {
  if (from < 0 || from >= nodeNum || to < 0 || to >= nodeNum) {
    cout << "Send payment node number error" << endl;
    exit(1);
  }
  if(!traverse(from, to, transfer, type)) {
    // cout << "No path" << endl;
    return false;
  }
  
  Node* nPtr = &_nodes[to];
  while (1) {
    // cout << "id=" << nPtr->id << " acc=" << nPtr->accWeight << endl;
    if (nPtr->fromNode < 0) break;
    double fee = nPtr->fromFee;
    double preImbalance = nPtr->neighbors[nPtr->fromNode].fund;
    nPtr->neighbors[nPtr->fromNode].fund += (transfer + (fee / 2));
    double newImbalance = nPtr->neighbors[nPtr->fromNode].fund;
    Node* nextPtr = nPtr->neighbors[nPtr->fromNode].nodePtr;
    for (int j = 0; j <= nextPtr->neighbors.size(); j += 1) {
      if (nextPtr->neighbors[j].nodePtr == nPtr) {
        preImbalance -= nextPtr->neighbors[j].fund;
        preImbalance = (preImbalance >= 0) ? preImbalance : -preImbalance;
        nextPtr->neighbors[j].fund += ((fee / 2) - transfer);
        newImbalance -= nextPtr->neighbors[j].fund;
        newImbalance = (newImbalance >= 0) ? newImbalance : -newImbalance;
      }
    }
    nPtr = nextPtr;
    totalImbalance += (newImbalance - preImbalance);
    totalFund += fee;
    // cout << "from=" << nPtr->id << " fee=" << fee << endl;
  }
  return true;
}
