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

/* ShortestPathAlgo Class is used to calculate the shortest path of a give graph by Dijkstra algorithm
 *
 * Sample Usage:
 *
 * Graph<string, double> randomG(5, 0.5, 10);
 * ShortestPathAlgo<string, double> algo(randomG);
 * double shortestpathcost = algo.averagePathSize(1);
 */
template<class Type, class Val>
class ShortestPathAlgo {
  typedef Graph<Type, Val> Graph;
  typedef Node<Type, Val> Node;
  typedef PriorityQueue<Type, Val> PriorityQueue;

 private:

  Graph& graph;  //the graph under calculation
  std::list<Node> shortestpath;  //store the resulting shortest path calculated by the algorithm
  Val shortestpathsize;  //store the resulting shortest path cost calcualted by the algorithm

  //Dijkstra algorithm implementation
  //Input:
  //indexofsource: the vertexindex of the source node
  //indexofdest: the vertexindex of destination node
  //Output: NONE
  void dijkstraImpl(int indexofsource, int indexofdest) {
    clean();
    const std::vector<Node> vertices = graph.getAllVertices();  //get the all vertices in graph
    std::vector<Node*> close;  //visited or close set
    PriorityQueue open;  //unvisited or open set
    const Node* source = &(vertices[indexofsource - 1]);  //get the source node
    const Val kINF = std::numeric_limits<Val>::max();  //set the INF as the maximal value of the type
    Val* distance = new Val[vertices.size()];  //store the min distance between source to any node
    bool* visited = new bool[vertices.size()];  //marked if the nodes have been visited (put in close set already)
    int* prevnode = new int[vertices.size()];  //store the previous node in path of the min distance between source to any node

    //initialize all temporary arrays provided for calculation and PriorityQueue with all infinity priority
    for (unsigned i = 0; i < vertices.size(); i++) {
      if ((i + 1) == static_cast<unsigned>(source->vertexindex)) {
        visited[source->vertexindex - 1] = true;
        distance[source->vertexindex - 1] = 0;
      } else {
        visited[i] = false;
        distance[i] = kINF;
        open.insert(&(vertices[i]), kINF);
        prevnode[i] = -1;
      }
    }
    close.push_back(const_cast<Node*>(source));  //initialize the close set with source node

    Val min = static_cast<Val>(0);
    Node* current = const_cast<Node*>(source);

    /*
     * terminate condition: all vertices are visited. close set has the size = graph's node size or
     * the target/destination node has been reached
     */
    while (close.size() < vertices.size() && current->vertexindex != indexofdest) {
      std::list<Node*> neighbors = current->neighbors;
      std::list<Val> edges = current->edges;
      typename std::list<Node*>::iterator iterneigh = neighbors.begin();
      typename std::list<Val>::iterator iteredge = edges.begin();

      for (; iterneigh != neighbors.end(); ++iterneigh, ++iteredge) {
        Val edgeval = min + *(iteredge);

        if (edgeval < distance[(*iterneigh)->vertexindex - 1]
            && !visited[(*iterneigh)->vertexindex - 1]) {
          distance[(*iterneigh)->vertexindex - 1] = edgeval;
          prevnode[(*iterneigh)->vertexindex - 1] = current->vertexindex;
          open.chgPrioirity((*iterneigh), edgeval);
        }
      }

      current = const_cast<Node*>(open.minPrioirty());
      min = distance[current->vertexindex - 1];
      visited[current->vertexindex - 1] = true;
      close.push_back(current);
    }

    assert(current->vertexindex == indexofdest);
    if (min != kINF)
      shortestpathsize = min;
    else
      shortestpathsize = 0;  //unreachable

    //trace back the shortest path
    while (current->vertexindex != source->vertexindex
        && prevnode[current->vertexindex - 1] > 0) {
      shortestpath.push_front(*current);
      current = const_cast<Node*>(&(vertices[prevnode[current->vertexindex - 1]
          - 1]));
    }
    shortestpath.push_front(*source);

    //free memory
    delete[] visited;
    delete[] distance;
    delete[] prevnode;
  }
  ;

 public:
  //Default constructor
  ShortestPathAlgo()
      : graph(NULL),
        shortestpathsize(-1) {
  }
  ;
  //Constructor based on a given graph
  ShortestPathAlgo(Graph& graph)
      : graph(graph),
        shortestpathsize(-1) {
  }
  ;
  //destructor
  virtual ~ShortestPathAlgo() {
  }
  ;
  //Return shortest path between u-w and returns the sequence of vertices representing shortest path
  //Input:
  //indexofsource: vertexindex of the source node
  //indexofdest: vertexindex of the destination node
  //Output:
  //The linked list which store the shortest path
  std::list<Node> path(int indexofsource, int indexofdest) {
    if (shortestpath.size() == 0)
      dijkstraImpl(indexofsource, indexofdest);
    std::list<Node> pathVec(shortestpath);
    shortestpath.clear();
    return std::list<Node>(pathVec);
  }
  ;
  //Return the path cost associated with the shortest path
  //Input:
  //indexofsource: vertexindex of the source node
  //indexofdest: vertexindex of the destination node
  //Output:
  //The cost of the shortest path between given source and destination
  //If the destination node is unreachable from source. The cost is 0.
  Val path_size(int indexofsource, int indexofdest) {
    if (shortestpathsize == -1)
      dijkstraImpl(indexofsource, indexofdest);
    Val pathcost = shortestpathsize;
    shortestpathsize = -1;
    return pathcost;
  }
  ;
  //Clean up the member list and size for consecutive calls of path_size
  //TODO temporary solution for consecutive calling path_size
  void clean() {
    shortestpath.clear();
    shortestpathsize = -1;
  }
  //Calculate the average shortest path cost of all pairs of nodes
  //Input: NONE
  //Output:
  //The cost calculated in double
  //Notice: the unreachable pair won't be included in calculation
  double averageAllPathSize() {
    double mean = 0;
    int count = 0;
    for (int i = 1; i < graph.getSizeOfVertices(); i++) {
      for (int j = i + 1; j <= graph.getSizeOfVertices(); j++) {
        float minpathsize = path_size(i, j);
        if (minpathsize > 0) {
          mean += minpathsize;
          count++;
        }
        clean();
      }
    }
    return (mean / static_cast<double>(count));
  }
  //Calculate the average shortest path cost of a given source node to the other nodes
  //Input:
  //indexofsource: vertexindex of the source node
  //Output:
  //The cost calculated in double
  //Notice: the unreachable pair won't be included in calculation
  double averagePathSize(int indexofsource) {
    float mean = 0;
    int count = 0;
    for (int j = 1; j <= graph.getSizeOfVertices(); j++) {
      float minpathsize = path_size(indexofsource, j);
      if (minpathsize > 0) {
        mean += minpathsize;
        count++;
      }
      clean();
    }
    return (mean / static_cast<double>(count));
  }
};

#endif /* SHORTESTPATHALGO_H_ */
