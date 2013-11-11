/*
 * MinSpanTreeAlgo.h
 * This file defines the implementation of algorithm to find Minimal Spanning Tree (MST) in a graph.
 * The template argument should be consistent with Graph.
 * MinSpanTreeAlgo(): default constructor, takes no parameters and do nothing
 *
 * MinSpanTreeAlgo(const Graph& graph): constructor which is used to calculate the MST of the input Graph
 * Sample usage to find MST from a graph readed from a text file:
 * string filename = "tinyEWG.txt";
 * PlainParser parser(filename);
 * Graph<string, double> graph(parser);
 * MinSpanTreeAlgo<string, double> mstalgo(graph);
 * mstalgo.calculate();
 * double minweight = mstalgo.getTotalminwieght();
 * Graph<string, double> msttree = mstalgo.getMsttree();
 */

#ifndef MINSPANTREEALGO_H_
#define MINSPANTREEALGO_H_

#include <string>
#include "Graph.h"
#include "PriorityQueue.h"

template<class Type, class Val>
class MinSpanTreeAlgo {

  typedef Graph<Type, Val> Graph; //the graph under calculation
  typedef PriorityQueue<int, Val> PriorityQueue; //the priority queue used to track the minimal edge in the graph

 private:
  const Graph& graph;  //the graph under calculation
  Graph msttree;  //the minimal spanning tree for output
  Val totalminweight;  //the minimal weight for minimal spanning tree

  //implement kruskal's MST
  //1. Make a empty graph which no nodes are connected
  //2. Extract the edges and sort from min to max
  //3. Connect the edge in empty graph if there's no loop created
  void kruskals() {
    int graphsize = graph.getSizeOfVertices();
    Graph emptygraph(graphsize);
    //key as "(index of from node-1)"x"size of vertices" + "(index of to node-1)"
    //value as the edge value
    std::map<int, Val> mapalledges = graph.getAllEdgesValues();
    PriorityQueue edgesqueue(mapalledges.size());
    for (typename std::map<int, Val>::iterator iter = mapalledges.begin();
        iter != mapalledges.end(); ++iter){
      edgesqueue.insert((*iter).first, (*iter).second);
    }

    // count how many nodes have been added to the empty graph
    int connectededge = 0;
    Val totalweight = 0;

    while (connectededge < (graphsize -1) && edgesqueue.size() > 0) {
      int minedgekey = edgesqueue.minPrioirty();
      Val minedgeval = mapalledges[minedgekey];
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
  };

public:
//default constructor, do nothing
MinSpanTreeAlgo();
//constructor which is used to calculate the MST of the input Graph
MinSpanTreeAlgo(const Graph& graph)
: graph(graph),
msttree(Graph(graph.getSizeOfVertices())),
totalminweight(std::numeric_limits<int>::max()) {
}
;
//destructor
virtual ~MinSpanTreeAlgo() {};

//TODO maybe change it to pass a function pointer or object
//Calculate the MST, invoked by client
//INPUT: NONE
//OUTPUT: NONE
//The result of calculated MST will be stored as data member in class.
//Client needs to use getters to get the result
void calculate() {
  kruskals();
}
;
//Getter to get the calculated MST result
//INPUT: NONE
//OUTPUT:
//The total minimal weight of the minimal spanning tree
Val getTotalminwieght() const {
  return totalminweight;
}
//Getter to get the calculated MST tree representation
//INPUT: NONE
//OUTPUT:
//The tree or graph representation of MST
const Graph& getMsttree() const {
  return msttree;
}
};
#endif /* MINSPANTREEALGO_H_ */
