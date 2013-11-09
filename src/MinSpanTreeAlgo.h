/*
 * MinSpanTreeAlgo.h
 *
 *  Created on: Nov 7, 2013
 *      Author: renewang
 */

#ifndef MINSPANTREEALGO_H_
#define MINSPANTREEALGO_H_

#include <string>
#include "Graph.h"
#include "PriorityQueue.h"

class MinSpanTreeAlgo {

  typedef Graph<std::string, int> Graph;
  typedef PriorityQueue<int, int> PriorityQueue;

 private:
  const Graph& graph;  //the graph under calculation
  Graph msttree;  //the minimal spanning tree for output
  int totalminweight;  //the minimal weight for minimal spanning tree
  void kruskals();  //implement kruskal's MST

 public:
  MinSpanTreeAlgo();
  MinSpanTreeAlgo(const Graph& graph)
      : graph(graph),
        msttree(Graph(graph.getSizeOfVertices())),
        totalminweight(std::numeric_limits<int>::max()) {
  }
  ;
  virtual ~MinSpanTreeAlgo(){};

  //TODO maybe change it to pass a function pointer or object
  void calculate(){
    kruskals();
  };

  int getTotalminwieght() const {
    return totalminweight;
  }

  const Graph& getMsttree() const {
    return msttree;
  }
};
#endif /* MINSPANTREEALGO_H_ */
