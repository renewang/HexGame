/*
 * ShortestPathAlgo.cpp
 *
 *  Created on: Dec 26, 2013
 *      Author: renewang
 */

#include "ShortestPathAlgo.h"

using namespace std;

//Return shortest path between u-w and returns the sequence of vertices representing shortest path
//Input:
//indexofsource: vertexindex of the source node
//indexofdest: vertexindex of the destination node
//Output:
//The linked list which store the shortest path
template<class Type, class Val>
list<int> ShortestPathAlgo<Type, Val>::path(int indexofsource,
                                                 int indexofdest) {
  if (shortestpath.empty())
    dijkstraImpl(indexofsource, indexofdest);
  list<int> pathVec(shortestpath);
  shortestpath.clear();
  return list<int>(pathVec);
}
//Return the path cost associated with the shortest path
//Input:
//indexofsource: vertexindex of the source node
//indexofdest: vertexindex of the destination node
//Output:
//The cost of the shortest path between given source and destination
//If the destination node is unreachable from source. The cost is 0.
template<class Type, class Val>
Val ShortestPathAlgo<Type, Val>::path_size(int indexofsource, int indexofdest) {
  if (shortestpathsize == -1)
    dijkstraImpl(indexofsource, indexofdest);
  Val pathcost = shortestpathsize;
  shortestpathsize = -1;
  return pathcost;
}
//Clean up the member list and size for consecutive calls of path_size
template<class Type, class Val>
void ShortestPathAlgo<Type, Val>::clean() {
  shortestpath.clear();
  shortestpathsize = -1;
}
//Calculate the average shortest path cost of all pairs of nodes
//Input: NONE
//Output:
//The cost calculated in double
//Notice: the unreachable pair won't be included in calculation
template<class Type, class Val>
double ShortestPathAlgo<Type, Val>::averageAllPathSize() {
  double mean = 0;
  int count = 0;
  for (int i = 1; i < graph.getSizeOfVertices(); i++) {
    for (int j = i + 1; j <= graph.getSizeOfVertices(); j++) {
      Val minpathsize = path_size(i, j);
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
template<class Type, class Val>
double ShortestPathAlgo<Type, Val>::averagePathSize(int indexofsource) {
  Val mean = 0;
  int count = 0;
  for (int j = 1; j <= graph.getSizeOfVertices(); j++) {
    Val minpathsize = path_size(indexofsource, j);
    if (minpathsize > 0) {
      mean += minpathsize;
      count++;
    }
    clean();
  }
  return (static_cast<double>(mean) / static_cast<double>(count));
}
//Dijkstra algorithm implementation
//Input:
//indexofsource: the vertexindex of the source node
//indexofdest: the vertexindex of destination node
//Output: NONE
template<class Type, class Val>
void ShortestPathAlgo<Type, Val>::dijkstraImpl(const int indexofsource,
                                               const int indexofdest) {
  clean();
  vector<int> vertices;  //get the all vertices in graph
  vector<int> close;  //visited or close set
  int source = indexofsource;  //get the source node
  const Val kINF = numeric_limits < Val > ::max();  //set the INF as the maximal value of the type
  const unsigned graphsize = graph.getSizeOfVertices();
  PriorityQueue<int, Val> open(graphsize);  //unvisited or open set
  Val* distance = new Val[graphsize];  //store the min distance between source to any node
  bool* visited = new bool[graphsize];  //marked if the nodes have been visited (put in close set already)
  int* prevnode = new int[graphsize];  //store the previous node in path of the min distance between source to any node

//initialize all temporary arrays provided for calculation and PriorityQueue with all infinity priority
  for (unsigned i = 0; i < graphsize; i++)
    vertices.push_back(i + 1);

  for (unsigned i = 0; i < graphsize; i++) {
    if ((i + 1) == static_cast<unsigned>(source)) {
      visited[source - 1] = true;
      distance[source - 1] = 0;
    } else {
      visited[i] = false;
      distance[i] = kINF;
      open.insert(vertices[i], kINF);
      prevnode[i] = -1;
    }
  }
  close.push_back(source);  //initialize the close set with source node

  Val min = static_cast<Val>(0);
  int current = source;

  /*
   * terminate condition: all vertices are visited. close set has the size = graph's node size or
   * the target/destination node has been reached
   */
  while (close.size() < graphsize && current != indexofdest) {
    vector<int> neighbors = graph.getNeighbors(current);
    vector < Val > edges = graph.getNeighborsEdgeValues(current);
    vector<int>::iterator iterneigh = neighbors.begin();
    typename vector<Val>::iterator iteredge = edges.begin();

    for (; iterneigh != neighbors.end(); ++iterneigh, ++iteredge) {
      Val edgeval = min + *(iteredge);

      if (edgeval < distance[(*iterneigh) - 1] && !visited[(*iterneigh) - 1]) {
        distance[(*iterneigh) - 1] = edgeval;
        prevnode[(*iterneigh) - 1] = current;
        assert(open.chgPrioirity(*iterneigh, edgeval));
      }
    }

    current = open.minPrioirty();
    min = distance[current - 1];
    visited[current - 1] = true;
    close.push_back(current);
  }

  assert(current == indexofdest);
  if (min != kINF)
    shortestpathsize = min;
  else
    shortestpathsize = 0;  //unreachable

//trace back the shortest path
  while (current != source && prevnode[current - 1] > 0) {
    shortestpath.push_front(current);
    current = vertices[prevnode[current - 1] - 1];
  }
  shortestpath.push_front(source);

//free memory
  delete[] visited;
  delete[] distance;
  delete[] prevnode;
}

