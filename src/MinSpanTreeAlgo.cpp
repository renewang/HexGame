/*
 * MinSpanTreeAlgo.cpp
 *
 *  Created on: Nov 7, 2013
 *      Author: renewang
 */

#include "PriorityQueue.h"
#include "MinSpanTreeAlgo.h"

using namespace std;
//1. Make a empty graph which no nodes are connected
//2. Extract the edges and sort from min to max
//3. Connect the edge in empty graph if there's no loop created
void MinSpanTreeAlgo::kruskals() {
  int graphsize = graph.getSizeOfVertices();
  Graph emptygraph(graphsize);
  //key as "(index of from node-1)"x"size of vertices" + "(index of to node-1)"
  //value as the edge value
  map<int, int> mapalledges = graph.getAllEdgesValues();
  PriorityQueue edgesqueue(mapalledges.size());
  for (map<int, int>::iterator iter = mapalledges.begin();
      iter != mapalledges.end(); ++iter)
    edgesqueue.insert((*iter).first, (*iter).second);

  // count how many nodes have been added to the empty graph
  int connectededge = 0;
  //int pass = 1;
  int totalweight = 0;

  while (connectededge < (graphsize - 1) && edgesqueue.size() > 0) {
    int minedgekey = edgesqueue.minPrioirty();
    int minedgeval = mapalledges[minedgekey];
    int indexoffrom = (minedgekey / graphsize) + 1;
    int indexofto = (minedgekey % graphsize) + 1;

    emptygraph.addEdge(indexoffrom, indexofto, minedgeval);
    bool isloopexisting = emptygraph.isLoopExisting(indexoffrom);

    if (isloopexisting) {
      emptygraph.deleteEdge(indexoffrom, indexofto);
      continue;
    }
    totalweight += minedgeval;
    connectededge++;
  }
  this->totalminweight = totalweight;
  this->msttree = emptygraph;
}
