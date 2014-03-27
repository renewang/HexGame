/*
 * AbstractGameTree.h
 * This file defines an interface for Monte Carlo Game Tree and UTC Policy. UTC Policy is part of Game Tree property.
 */

#ifndef ABSTRACTGAMETREE_H_
#define ABSTRACTGAMETREE_H_

#include "Global.h"

#include <string>
#include <utility>
#include <vector>
#include <boost/graph/graph_as_tree.hpp>
#include <boost/graph/adjacency_list.hpp>

namespace boost {
//create an additional property
enum vertex_value_t {//for storing the calculation result of UTC calculation function
  vertex_value
};

enum vertex_position_t {//for storing the hex game board position
  vertex_position
};
BOOST_INSTALL_PROPERTY(vertex, value); //install vertex_value_t as internal property
BOOST_INSTALL_PROPERTY(vertex, position); //install vertex_position_t as internal property
}
/*
 * AbstractUTCPolicy class is used to provide an abstract interface which stores information for UTC Policy calculation
 * AbstractUTCPolicy(): default constructor which should not be called any time due to the abstract nature of this class
 */
class AbstractUTCPolicy {
 protected:
  AbstractUTCPolicy(){}; //default constructor
  virtual ~AbstractUTCPolicy(){}; //destructor
 public:
  enum valuekind {
    visitcount = 0, //enum type serves as index for stored value of visiting count extraction in feature holder
    wincount        //enum type servers as index for stored value of winning statistics extraction in feature holder
  };
  virtual std::string print() = 0; //print out the feactures
  virtual double estimate() = 0; //function to estimate the winning statistics
  virtual double calculate(AbstractUTCPolicy& parent) = 0; //function to calculate UTC value
  virtual bool update(valuekind indexofkind, int value, int increment = 0) = 0; //function to update feature values of either visitcount or wincount
  virtual bool updateAll(valuekind visitkind, int valueofvisit,
                         int increaseofvisit, valuekind winkind, int valueofwin,
                         int increaseofwin) = 0; //function to update values of visitcount and wincount together
  virtual int feature(valuekind indexofkind) =0; //function to access either visitcount or wincount
  virtual double getValue() const = 0; //getter for accessing estimated winning statistics (wincount)
  virtual double getBalance() const = 0; //getter for accessing calculated UTC balance
};
/*
 * AbstractUTCPolicyImpl class is used to provide default implementation for UTC Policy calculation
 * AbstractUTCPolicyImpl(): default constructor
 * TODO: 3/27/14 currently empty definition
 */
class AbstractUTCPolicyImpl:public AbstractUTCPolicy{
 public:
  AbstractUTCPolicyImpl(){};//default constructor
  virtual ~AbstractUTCPolicyImpl(){}; //destructor

  virtual std::string print() = 0; //see AbstractUTCPolicy.print()
  virtual double estimate() = 0; //see AbstractUTCPolicy.estimate()
  virtual double calculate(AbstractUTCPolicy& parent) = 0; //see AbstractUTCPolicy.calculate
  virtual bool update(valuekind indexofkind, int value, int increment = 0) = 0; //see AbstractUTCPolicy.update
  virtual bool updateAll(valuekind visitkind, int valueofvisit,
                         int increaseofvisit, valuekind winkind, int valueofwin,
                         int increaseofwin) =0; //see AbstractUTCPolicy.updateAll
  virtual int feature(valuekind indexofkind) =0; //see AbstractUTCPolicy.feature
  virtual double getValue() const = 0; //see AbstractUTCPolicy.getValue
  virtual double getBalance() const = 0; //see AbstractUTCPolicy.getBalance

  //for locking
  virtual bool waitforupdate() = 0; //function holds for a condition variable which will wait for update done
  virtual void notifyupdatedone() = 0; //function notify update is already done
  virtual int getCountforexpand() = 0; //function to get semaphore which indicating the number of threads already enter critical waiting for expansion region
  virtual void setCountforexpand(int value) = 0; //function to set semaphore indicating the number of threads already enter critical waiting for expansion region
  virtual void addCountforexpand(int increment) = 0; //function to add semaphore indicating the number of threads already enter critical waiting for expansion region
  virtual void subCountforexpand(int decrement) = 0; //function to subtract semaphore indicating the number of threads already enter critical waiting for expansion region
  virtual int getNumofFutureChildren() = 0; //getter to get number of future children
  virtual void setNumofFutureChildren(int numofchildren, int increment) = 0; //setter to set number of future children
  virtual bool getIsupdated() const = 0; //getter to get the state of boolean variable isupdate
  virtual void setIsupdated(bool isupdated) = 0; //setter to set the state of boolean variable isupdate
};
/*
 * AbstractGameTree class is used to provide abstract interface for GameTree and LockableGameTree class
 *
 */
class AbstractGameTree {
 public:
  //for retrieving tree information
  virtual std::size_t getNodeDepth(int indexofnode) = 0; //getter to get the depth from the root
  //virtual std::size_t getSizeofNodes() = 0; //getter to get the size of total nodes in the current game tree
  virtual std::size_t getSizeofEdges() = 0; //getter to get the size of total edges in the current game tree which should be (number of total nodes - 1)
  virtual std::size_t getNodePosition(std::size_t indexofnode) = 0; //getter to get the position of hex board storing in the given node
  virtual void setNodePosition(std::size_t indexofnode, std::size_t position) = 0; //setter to set the position of hex board storing in the given node
  virtual std::vector<std::size_t> getSiblings(std::size_t indexofnode) = 0; //getter to get the siblings of a given node
  virtual std::size_t getNumofChildren(std::size_t indexofnode) = 0; //getter to get the number of children of a given node

  //print out the tree
  virtual std::string printGameTree(int key) = 0; //DFS traversal to print tree with parenthesized representation
  //for tree utility
  virtual void clearAll() = 0; //to clear up all nodes and edges
  //tree expansion
  virtual int expandNode(int indexofsource, int move, char color) = 0; //to expand one child from a given node

  //for game simulation
  virtual int updateNodefromSimulation(int indexofnode, int winner, int level) = 0; //to update UTC value according to the simulation (play-out) result
  virtual std::pair<int, double> getBestMovefromSimulation() = 0;//to get the best move by maximizing simulated winning statistics
  virtual std::pair<int, int> selectMaxBalanceNode(int currentempty, bool isbreaktie) = 0;//to select the node with maximal UTC balance value
  virtual void getMovesfromTreeState(
      int indexofnode, std::vector<int>& babywatsons, std::vector<int>& opponents,
      hexgame::unordered_set<int>& remainingmoves) = 0; //to get past moves (with the position information on the hex board)
  //node property relevant
  virtual int getNodeValueFeature(int indexofnode,
                          AbstractUTCPolicy::valuekind indexofkind) = 0;//to get feature of UTC Policy from a given node
  virtual std::string name() = 0; //utility function to print out poly-morphic name

 protected:
  AbstractGameTree() {}; //default constructor, should not be invoked to instantiate object under any circumstances
  virtual ~AbstractGameTree() {}; //destructor

 protected:
  //setting up internal composite property for boost graph
  typedef boost::property<boost::vertex_value_t, hexgame::shared_ptr< AbstractUTCPolicy > > vertex_value_prop; //value is use to store UTC values and features
  typedef boost::property<boost::vertex_color_t, boost::default_color_type,
      vertex_value_prop> vertex_color_prop; //color is used to indicate the move made by which player (RED or BLUE)
  typedef boost::property<boost::vertex_name_t, std::string, vertex_color_prop> vertex_name_prop; //name for node is used to print out information for debugging
  typedef boost::property<boost::vertex_position_t, std::size_t,
      vertex_name_prop> vertex_final_prop; //final property map

  //for graph and tree
  typedef boost::adjacency_list<boost::listS, boost::vecS,
      boost::bidirectionalS, vertex_final_prop> basegraph;  //adjacency_list<OutEdgeListS,VertexListS,DirectedS>

  //for graph traits and redefining various basic type with shortening name
  typedef boost::graph_traits<basegraph>::vertex_descriptor vertex_t;
  typedef boost::graph_traits<basegraph>::edge_descriptor edge_t;
  typedef boost::graph_traits<basegraph>::vertex_iterator vertex_iter;
  typedef boost::graph_traits<basegraph>::adjacency_iterator adj_vertex_iter;
  typedef boost::graph_traits<basegraph>::in_edge_iterator in_edge_iter;
  typedef boost::graph_traits<basegraph>::out_edge_iterator out_edge_iter;

  //for tree
  typedef boost::iterator_property_map<std::vector<vertex_t>::iterator,
      boost::property_map<basegraph, boost::vertex_index_t>::type> parent_map_t;
  typedef boost::graph_as_tree<basegraph, parent_map_t> gametree;
};
#endif /* ABSTRACTGAMETREE_H_ */
