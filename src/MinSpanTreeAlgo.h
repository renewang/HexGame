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

  typedef ::Graph<Type, Val> Graph;  //the graph under calculation
  typedef ::PriorityQueue<int, Val> PriorityQueue;  //the priority queue used to track the minimal edge in the graph

 private:
  const Graph& graph;  //the graph under calculation
  Graph msttree;  //the minimal spanning tree for output
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
    void calculate() {
      int graphsize = algo.graph.getSizeOfVertices();
      Graph emptygraph(graphsize);
      //key as "(index of from node-1)"x"size of vertices" + "(index of to node-1)"
      //value as the edge value
      std::map<int, Val> mapalledges = algo.graph.getAllEdgesValues();
      PriorityQueue edgesqueue(mapalledges.size());
      for (typename std::map<int, Val>::iterator iter = mapalledges.begin();
          iter != mapalledges.end(); ++iter) {
        edgesqueue.insert((*iter).first, (*iter).second);
      }
      // count how many nodes have been added to the empty graph
      Val totalweight = 0;

      while (emptygraph.getSizeOfEdges() < (graphsize - 1)
          && edgesqueue.size() > 0) {
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
      }
      algo.totalminweight = totalweight;
      algo.msttree = emptygraph;
    }
    ;
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
    void calculate() {
      //this array is used to track the representatives in the dis-joined set, starting from singleton
      //initially assign to each vertex with its own vertexindex
      int graphsize = algo.graph.getSizeOfVertices();
      Graph emptygraph(graphsize);
      std::vector<int> repsentative(graphsize);
      for (unsigned i = 0; i < graphsize; i++)
        repsentative[i] = (i + 1);

      //this array is used to track the size of each dis-joined set
      std::vector<int> repsize(graphsize, 1);

      //insert edges into priority queue
      std::map<int, Val> mapalledges = algo.graph.getAllEdgesValues();
      PriorityQueue edgesqueue(mapalledges.size());
      for (typename std::map<int, Val>::iterator iter = mapalledges.begin();
          iter != mapalledges.end(); ++iter) {
        edgesqueue.insert((*iter).first, (*iter).second);
      }

      // count how many nodes have been added to the empty graph
      Val totalweight = 0;

      while (emptygraph.getSizeOfEdges() < (graphsize - 1)
          && edgesqueue.size() > 0) {
        int minedgekey = edgesqueue.minPrioirty();
        Val minedgeval = mapalledges[minedgekey];
        int indexoffrom = (minedgekey / graphsize) + 1;
        int indexofto = (minedgekey % graphsize) + 1;

        if (repsentative[indexoffrom - 1] != repsentative[indexofto - 1]) {  //not in the same set

          //check the size of group and update the one with smaller size
          int indexofsmallrep = repsentative[indexoffrom - 1];  //representative of smaller group
          int indexoflargerep = repsentative[indexofto - 1];  //representative of larger group

          if (repsize[repsentative[indexoffrom - 1] - 1]
              > repsize[repsentative[indexofto - 1] - 1]) {
            indexofsmallrep = repsentative[indexofto - 1];
            indexoflargerep = repsentative[indexoffrom - 1];
          }
          //reassign the representative of the members in small set to the representative of larger set
          for (unsigned i = 0; i < graphsize; i++)
            if (repsentative[i] == indexofsmallrep) {
              repsentative[i] = indexoflargerep;
              repsize[indexofsmallrep - 1]--;
              repsize[indexoflargerep - 1]++;
            }
          emptygraph.addEdge(indexoffrom, indexofto, minedgeval);
          totalweight += minedgeval;
        }
      }
      algo.totalminweight = totalweight;
      algo.msttree = emptygraph;
    }
    ;
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
    void calculate() {
      srand(clock());
      std::vector<int> vertices;  //get the all vertices in graph
      std::vector<int> close;  //visited or close set
      const Val kINF = std::numeric_limits<Val>::max();  //set the INF as the maximal value of the type
      const unsigned graphsize = algo.graph.getSizeOfVertices();
      PriorityQueue open(graphsize);  //unvisited or open set
      bool* visited = new bool[graphsize];  //marked if the nodes have been visited (put in close set already)
      Val* distance = new Val[graphsize];  //store the min distance between target node to any node in close set
      int* prevnode = new int[graphsize];  //store the node in close set which possesses the minimal edge

      Graph emptygraph(graphsize);

      //random assign a source node
      int source = rand() % graphsize + 1;

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

      /*
       * terminate condition: all vertices are visited or close set has the size = graph's node size.
       */
      Val totalweight = 0;

      while (close.size() < graphsize
          && emptygraph.getSizeOfEdges() < graphsize - 1) {

        //for each node in the close set, find the minimal out edge to the node in open set
        for (auto node : close) {
          std::vector<int> neighbors = algo.graph.getNeighbors(node);
          std::vector<Val> edges = algo.graph.getNeighborsEdgeValues(node);

          std::vector<int>::iterator iterneigh = neighbors.begin();
          typename std::vector<Val>::iterator iteredge = edges.begin();

          for (; iterneigh != neighbors.end(); ++iterneigh, ++iteredge)
            if (!visited[*iterneigh - 1]
                && *iteredge < distance[(*iterneigh) - 1]) {  //don't update those already in close set and replace with smaller edge
              distance[(*iterneigh) - 1] = *iteredge;
              prevnode[(*iterneigh) - 1] = node;
              assert(open.chgPrioirity(*iterneigh, *iteredge));
            }
        }
        int nexttonode = open.minPrioirty();
        int nextfromnode = prevnode[nexttonode - 1];
        Val nextminedge = algo.graph.getEdgeValue(nextfromnode, nexttonode);
        totalweight += nextminedge;
        emptygraph.addEdge(nextfromnode, nexttonode, nextminedge);
        close.push_back(nexttonode);
        visited[nexttonode - 1] = true;
      }
      algo.totalminweight = totalweight;
      algo.msttree = emptygraph;

      delete[] visited;
      delete[] distance;
      delete[] prevnode;
    }
    ;
  };
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
  virtual ~MinSpanTreeAlgo() {
  }
  ;

//TODO maybe change it to pass a function pointer or object
//Calculate the MST, invoked by client
//INPUT: NONE
//OUTPUT: NONE
//The result of calculated MST will be stored as data member in class.
//Client needs to use getters to get the result
  void calculate(AbstractAlgorithm& algo) {
    algo.calculate();
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
