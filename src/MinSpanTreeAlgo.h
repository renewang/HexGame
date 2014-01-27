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
#include "AbstractAlgorithm.h"

template<class Type, class Val>
class MinSpanTreeAlgo {

  //typedef ::Graph<Type, Val> Graph;  //the graph under calculation
  //typedef ::PriorityQueue<int, Val> PriorityQueue;  //the priority queue used to track the minimal edge in the graph

 private:
  const Graph<Type, Val>& graph;  //the graph under calculation
  Graph<Type, Val> msttree;  //the minimal spanning tree for output
  Val totalminweight;  //the minimal weight for minimal spanning tree

 public:
  class Kruskals : public AbstractAlgorithm {
   private:
    MinSpanTreeAlgo& algo;
   public:
    Kruskals(MinSpanTreeAlgo& algo)
        : algo(algo) {
    }
    ;
    //implement kruskal's MST
    //1. Make a empty graph which no nodes are connected
    //2. Extract the edges and sort from min to max
    //3. Connect the edge in empty graph if there's no loop created
    void calculate();
    inline virtual string name(){return string("Kruskals + DFS");};
  };
  class UnionFind : public AbstractAlgorithm {
   private:
    MinSpanTreeAlgo& algo;
   public:
    UnionFind(MinSpanTreeAlgo& algo)
        : algo(algo) {
    }
    ;
    //implement union-find algorithm
    //1. MakeSet: to create a group of single set which contain each vertex in the graph
    //2. Weighted Union: to join the smaller size of linked list to the larger one by pointing to the head/leader of the list
    //3. Find: to return the new head/leader of the joined list
    void calculate();
    inline virtual string name(){return string("Kruskals + UnionFind");};
  };
  class Prim : public AbstractAlgorithm {
   private:
    MinSpanTreeAlgo& algo;
   public:
    Prim(MinSpanTreeAlgo& algo)
        : algo(algo) {
    }
    ;
    //implement prim algorithm
    void calculate();
    inline virtual string name(){return string("Prim");};
  };
//default constructor, do nothing
  MinSpanTreeAlgo();
//constructor which is used to calculate the MST of the input Graph
  MinSpanTreeAlgo(const Graph<Type, Val>& graph)
      : graph(graph),
        msttree(Graph<Type, Val>(graph.getSizeOfVertices())),
        totalminweight(std::numeric_limits<int>::max()) {
  }
  ;
//destructor
  virtual ~MinSpanTreeAlgo() {
  }
  ;

//Calculate the MST, invoked by client
//INPUT: NONE
//OUTPUT: NONE
//The result of calculated MST will be stored as data member in class.
//Client needs to use getters to get the result
  void calculate(AbstractAlgorithm& algo);
//Getter to get the calculated MST result
//INPUT: NONE
//OUTPUT:
//The total minimal weight of the minimal spanning tree
  Val getTotalminwieght() const;
//Getter to get the calculated MST tree representation
//INPUT: NONE
//OUTPUT:
//The tree or graph representation of MST
  const Graph<Type, Val>& getMsttree() const;
};
#include "MinSpanTreeAlgo.cpp"
#endif /* MINSPANTREEALGO_H_ */
