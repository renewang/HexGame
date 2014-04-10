/*
 * MinSpanTreeAlgo.h
 * This file defines the implementation of algorithm to find Minimal Spanning Tree (MST) in a graph.
 *
 *  Created on:
 *     Author: renewang
 */

#ifndef MINSPANTREEALGO_H_
#define MINSPANTREEALGO_H_

#include <string>
#include <limits>
#include "Graph.h"
#include "PriorityQueue.h"
#include "AbstractAlgorithm.h"
/**
 * The template argument should be consistent with Graph.<br/>
 * MinSpanTreeAlgo(): default constructor, takes no parameters and do nothing<br/>
 *
 * MinSpanTreeAlgo(const Graph& graph): constructor which is used to calculate the MST of the input Graph<br/>
 * Sample usage to find MST from a graph readed from a text file:<br/>
 *
 *      string filename = "tinyEWG.txt";
 *      PlainParser parser(filename);
 *      Graph<string, double> graph(parser);
 *      MinSpanTreeAlgo<string, double> mstalgo(graph);
 *      mstalgo.calculate();
 *      double minweight = mstalgo.getTotalminwieght();
 *      Graph<string, double> msttree = mstalgo.getMsttree();
 */
template<class Type, class Val>
class MinSpanTreeAlgo {
 private:
  const Graph<Type, Val>& graph;  ///< the graph under calculation
  Graph<Type, Val> msttree;  ///< the minimal spanning tree for output
  Val totalminweight;  ///< the minimal weight for minimal spanning tree

 public:
  /**
   * implement Kruskals algorithm <br/>
   * 1. Make a empty graph which no nodes are connected <br/>
   * 2. Extract the edges and sort from min to maximum <br/>
   * 3. Connect the edge in empty graph if there's no loop created <br/>
   *
   * See Also  MinSpanTreeAlgo <br/>
   *
   * Sample Usage:
   *
   *      PlainParser parser(filename);
   *      Graph <string, int> graph(parser);
   *      MinSpanTreeAlgo <string, int> mstalgo(graph);
   *      MinSpanTreeAlgo <string, int>::Kruskals kruskals(mstalgo);
   *      mstalgo.calculate(kruskals);
   */
  class Kruskals : public AbstractAlgorithm {
   private:
    MinSpanTreeAlgo& algo; ///<the MinSpanTreeAlgo object which will store the calculation result
   public:
    ///User defined constructor which takes a MinSpanTreeAlgo object's reference as input in order to store result in the object
    ///@param algo is a reference of MinSpanTreeAlgo object to store the calculation result
    Kruskals(MinSpanTreeAlgo& algo)
        : algo(algo) {
    }
    ;
    ///See AbstractAlgorithm::calculate()
    void calculate();
    ///See AbstractAlgorithm::name()
    inline virtual string name(){return string("Kruskals + DFS");};
  };
  /**
   * implement UnionFind algorithm<br/>
   * 1. MakeSet: to create a group of single set which contain each vertex in the graph<br/>
   * 2. Weighted Union: to join the smaller size of linked list to the larger one by pointing to the head/leader of the list<br/>
   * 3. Find: to return the new head/leader of the joined list<br/>
   *
   * See Also  MinSpanTreeAlgo<br/>
   *
   * Sample Usage:
   *
   *       PlainParser parser(filename);
   *       Graph <string, int> graph(parser);
   *       MinSpanTreeAlgo <string, int> mstalgo(graph);
   *       MinSpanTreeAlgo <string, int>::UnionFind unionfind(mstalgo);
   *       mstalgo.calculate(unionfind);
   */
  class UnionFind : public AbstractAlgorithm {
   private:
    MinSpanTreeAlgo& algo;
   public:
    ///User defined constructor which takes a MinSpanTreeAlgo object's reference as input in order to store result in the object
    ///@param algo is a reference of MinSpanTreeAlgo object to store the calculation result
    UnionFind(MinSpanTreeAlgo& algo)
        : algo(algo) {
    }
    ;
    ///See AbstractAlgorithm::calculate()
    void calculate();
    ///See AbstractAlgorithm::name()
    inline virtual string name(){return string("Kruskals + UnionFind");};
  };
  /**
   * implement prim algorithm<br/>
   * 1. Divide all the vertices into two set close and open. Close starts from an empty set and open from a full vertices set.<br/>
   * 2. Pick a random node into close set<br/>
   * 3. Choose the one vertex in open set which has the minimal distance to the close set.<br/>
   * 4. Stop till no more vertices can be add to close set<br/>
   * See Also  MinSpanTreeAlgo<br/>
   *
   * Sample Usage:<br/>
   *
   *        PlainParser parser(filename);
   *        Graph <string, double> graph(parser);
   *        MinSpanTreeAlgo <string, double> mstalgo(graph);
   *        MinSpanTreeAlgo <string, double>::Prim prim(mstalgo);
   *        mstalgo.calculate(prim);
   */
  class Prim : public AbstractAlgorithm {
   private:
    MinSpanTreeAlgo& algo;///<the MinSpanTreeAlgo object which is the interface
   public:
    ///User defined constructor which takes a MinSpanTreeAlgo object's reference as input in order to store result in the object
    ///@param algo is a reference of MinSpanTreeAlgo object to store the calculation result
    Prim(MinSpanTreeAlgo& algo)
        : algo(algo) {
    }
    ;
    ///See AbstractAlgorithm::calculate()
    void calculate();
    ///See AbstractAlgorithm::name()
    inline virtual string name(){return string("Prim");};
  };
  ///default constructor, do nothing
  MinSpanTreeAlgo();
 ///constructor which is used to calculate the MST of the input Graph
  MinSpanTreeAlgo(const Graph<Type, Val>& graph)
      : graph(graph),
        msttree(Graph<Type, Val>(graph.getSizeOfVertices())),
        totalminweight(std::numeric_limits<int>::max()) {
  }
  ;
///destructor
  virtual ~MinSpanTreeAlgo() {
  }
  ;
///Calculate the MST, invoked by client
///@param algo is an AbstractAlgorithm reference which is responsible for the actual MST algorithm calculation
///@return NONE
///The result of calculated MST will be stored as data member in class.<br/>
///Client needs to use getters getTotalminweight() to get the result <br/>
  void calculate(AbstractAlgorithm& algo);
///Getter to get the calculated MST result
///@param NONE
///@return The total minimal weight of the minimal spanning tree
  Val getTotalminwieght() const;
///Getter to get the calculated MST tree representation
///@param NONE
///@return The tree or graph representation of MST
  const Graph<Type, Val>& getMsttree() const;
};
#include "MinSpanTreeAlgo.cpp"
#endif /* MINSPANTREEALGO_H_ */
