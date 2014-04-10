/*
 * ShortestPathAlgo.h
 * This file defines a set of function used for Dijkstra algorithm to calculate the shortest path in a give graph
 */

#ifndef SHORTESTPATHALGO_H_
#define SHORTESTPATHALGO_H_

#include <list>
#include <vector>
#include <limits>

#include "Graph.h"
#include "PriorityQueue.h"

/**
 *  ShortestPathAlgo Class is used to calculate the shortest path of a give graph by Dijkstra algorithm <br/>
 *
 * Sample Usage:<br/>
 *
 *      Graph<string, double> randomG(5, 0.5, 10);
 *      ShortestPathAlgo<string, double> algo(randomG);
 *      double shortestpathcost = algo.averagePathSize(1);
 */
template<class Type, class Val>
class ShortestPathAlgo {

 private:

  Graph<Type, Val>& graph;  ///<the graph under calculation
  std::list<int> shortestpath;  ///<store the resulting shortest path calculated by the algorithm
  Val shortestpathsize;  ///<store the resulting shortest path cost calcualted by the algorithm

  ///Dijkstra algorithm implementation
  ///@param indexofsource is the vertexindex of the source node
  ///@param indexofdest is the vertexindex of destination node
  ///@return NONE
  void dijkstraImpl(const int indexofsource, const int indexofdest);

 public:
  ///Default constructor
  ShortestPathAlgo()
      : graph(NULL),
        shortestpathsize(-1) {
  }
  ;
  ///Constructor based on a given graph
  ///@param graph is a graph whose pairwise shortest path will be calculated
  ShortestPathAlgo(Graph<Type, Val>& graph)
      : graph(graph),
        shortestpathsize(-1) {
  }
  ;
  ///destructor
  virtual ~ShortestPathAlgo() {
  }
  ;
  ///Return shortest path between u-w and returns the sequence of vertices representing shortest path
  ///@param indexofsource is vertexindex of the source node
  ///@param indexofdest is vertexindex of the destination node
  ///@return The linked list which store the shortest path
  std::list<int> path(int indexofsource, int indexofdest);
  ///Return the path cost associated with the shortest path
  ///@param indexofsource is vertexindex of the source node
  ///@param indexofdest is vertexindex of the destination node
  ///@return The cost of the shortest path between given source and destination
  //If the destination node is unreachable from source. The cost is 0.
  Val path_size(int indexofsource, int indexofdest);
  ///Clean up the member list and size for consecutive calls of path_size
  ///@param NONE
  ///@return NONE
  void clean();
  /// Calculate the average shortest path cost of all pairs of nodes
  ///@param NONE
  ///@return  The cost calculated in double
  ///Notice: the unreachable pair won't be included in calculation
  double averageAllPathSize();
  ///Calculate the average shortest path cost of a given source node to the other nodes
  ///@param indexofsource is vertexindex of the source node
  ///@return The cost calculated in double
  ///Notice: the unreachable pair won't be included in calculation
  double averagePathSize(int indexofsource);
};
#include "ShortestPathAlgo.cpp"
#endif /* SHORTESTPATHALGO_H_ */
