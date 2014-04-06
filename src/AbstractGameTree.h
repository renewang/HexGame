/*
 * AbstractGameTree.h
 * This file defines an interface for Monte Carlo Game Tree and UTC Policy. UTC Policy is part of Game Tree property.
 *   Created on:
 *      Author: renewang
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
///create an additional property
enum vertex_value_t {  ///<for storing the calculation result of UTC calculation function
  vertex_value
};

enum vertex_position_t {  ///<for storing the hex game board position
  vertex_position
};
BOOST_INSTALL_PROPERTY(vertex, value);  ///<install vertex_value_t as internal property
BOOST_INSTALL_PROPERTY(vertex, position);///<install vertex_position_t as internal property
}
/**
 * AbstractUTCPolicy class is used to provide an abstract interface which stores information for UTC Policy calculation
 * AbstractUTCPolicy(): default constructor which should not be called any time due to the abstract nature of this class
 */
class AbstractUTCPolicy {
 protected:
  ///default constructor which should not be invoked to instantiate AbstractUTCPolicy object
  AbstractUTCPolicy() {
  }
  ;
  ///destructor
  virtual ~AbstractUTCPolicy() {
  }
  ;
 public:
  ///enum type serves as index for stored features in UTC Policy
  enum valuekind {
    visitcount = 0,  ///< enum type serves as index for stored value of visiting count extraction in feature holder
    wincount  ///< enum type servers as index for stored value of winning statistics extraction in feature holder
  };
  ///Print out the features
  virtual std::string print() = 0;
  ///To estimate the winning statistics
  virtual double estimate() = 0;
  ///To calculate UTC value
  virtual double calculate(AbstractUTCPolicy& parent) = 0;
  ///To update feature values of either visitcount or wincount
  virtual bool update(valuekind indexofkind, int value, int increment = 0) = 0;
  ///To update values of visitcount and wincount together
  virtual bool updateAll(valuekind visitcount, int valueofvisit,
                         int increaseofvisit, valuekind wincount,
                         int valueofwin, int increaseofwin) = 0;
  ///To access either visitcount or wincount
  virtual int feature(valuekind indexofkind) =0;
  ///Getter for accessing estimated winning statistics (wincount)
  virtual double getValue() const = 0;
  ///Getter for accessing calculated UTC balance
  virtual double getBalance() const = 0;
};
#if __cplusplus > 199711L
#define AbstractUTCPolicy_visitcount AbstractUTCPolicy::valuekind::visitcount
#define AbstractUTCPolicy_wincount AbstractUTCPolicy::valuekind::wincount
#else
#define AbstractUTCPolicy_visitcount AbstractUTCPolicy::visitcount
#define AbstractUTCPolicy_wincount AbstractUTCPolicy::wincount
#endif
/**
 * AbstractGameTree class is used to provide abstract interface for GameTree and LockableGameTree class
 * AbstractGameTree(): default constructor which should not be called any time to instantiate AbstractGameTree object due to the abstract nature of this class
 */
class AbstractGameTree {
 public:
  //for retrieving tree information
  ///Getter to get the depth from the root
  virtual std::size_t getNodeDepth(int indexofnode) = 0;
  ///Getter to get the size of total nodes in the current game tree
  virtual std::size_t getSizeofNodes() = 0;
  ///Getter to get the size of total edges in the current game tree which should be (number of total nodes - 1)
  virtual std::size_t getSizeofEdges() = 0;
  ///Getter to get the position of hex board storing in the given node
  virtual std::size_t getNodePosition(std::size_t indexofnode) = 0;
  ///Setter to set the position of hex board storing in the given node
  virtual void setNodePosition(std::size_t indexofnode,
                               std::size_t position) = 0;
  ///Getter to get the siblings of a given node
  virtual std::vector<std::size_t> getSiblings(std::size_t indexofnode) = 0;
  ///Getter to get the number of children of a given node
  virtual std::size_t getNumofChildren(std::size_t indexofnode) = 0;

  //print out the tree
  ///Depth First Search (DFS) traversal to print tree with parenthesized representation
  virtual std::string printGameTree(int key) = 0;
  //for tree utility
  ///To clear up all nodes and edges
  virtual void clearAll() = 0;
  //tree expansion
  ///To expand one child from a given node
  virtual int expandNode(int indexofsource, int move, char color) = 0;

  //for game simulation
  ///To update UTC value according to the simulation (play-out) result
  virtual void updateNodefromSimulation(int indexofnode, int winner,
                                        int level) = 0;
  ///To get the best move by maximizing simulated winning statistics
  virtual std::pair<int, double> getBestMovefromSimulation() = 0;
  ///To select the node with maximal UTC balance value
  virtual std::pair<int, int> selectMaxBalanceNode(int currentempty,
                                                   bool isbreaktie) = 0;
  ///To get past moves (with the position information on the hex board)
  virtual void getMovesfromTreeState(
      int indexofnode, std::vector<int>& babywatsons,
      std::vector<int>& opponents,
      hexgame::unordered_set<int>& remainingmoves) = 0;
  //node property relevant
  ///To get feature of UTC Policy from a given node
  virtual int getNodeValueFeature(int indexofnode,
                                  AbstractUTCPolicy::valuekind indexofkind) = 0;
  ///Utility function to print out poly-morphic name
  virtual std::string name() = 0;

 protected:
  ///default constructor, should not be invoked to instantiate object under any circumstances
  AbstractGameTree() {
  }
  ;
  ///destructor
  virtual ~AbstractGameTree() {
  }
  ;

 protected:
  //setting up internal composite property for boost graph
  ///Value is use to store UTC values and features
  typedef boost::property<boost::vertex_value_t,
      hexgame::shared_ptr<AbstractUTCPolicy> > vertex_value_prop;
  ///Color is used to indicate the move made by which player (RED or BLUE)
  typedef boost::property<boost::vertex_color_t, boost::default_color_type,
      vertex_value_prop> vertex_color_prop;
  ///Name for node is used to print out information for debugging
  typedef boost::property<boost::vertex_name_t, std::string, vertex_color_prop> vertex_name_prop;
  ///Final composite property map which is the internal property map for boost graph
  typedef boost::property<boost::vertex_position_t, std::size_t,
      vertex_name_prop> vertex_final_prop;

  //for graph and tree
  ///Define boost adjacency_list as underlying implementation of game tree, basegraph
  typedef boost::adjacency_list<boost::listS, boost::vecS,
      boost::bidirectionalS, vertex_final_prop> basegraph;  //adjacency_list<OutEdgeListS,VertexListS,DirectedS>

  //for graph traits and redefining various basic type with shortening name
  ///Define vertex type of basegraph
  typedef boost::graph_traits<basegraph>::vertex_descriptor vertex_t;
  ///Define edge type of basegraph
  typedef boost::graph_traits<basegraph>::edge_descriptor edge_t;
  ///Define vertex iterator type of basegraph
  typedef boost::graph_traits<basegraph>::vertex_iterator vertex_iter;
  ///Define adjacency vertex iterator type of basegraph
  typedef boost::graph_traits<basegraph>::adjacency_iterator adj_vertex_iter;
  ///Define in-edge iterator type of basegraph
  typedef boost::graph_traits<basegraph>::in_edge_iterator in_edge_iter;
  ///Define out-edge iterator type of basegraph
  typedef boost::graph_traits<basegraph>::out_edge_iterator out_edge_iter;

  //for tree
  ///Define iterator to traverse parental nodes in tree wraper
  typedef boost::iterator_property_map<std::vector<vertex_t>::iterator,
      boost::property_map<basegraph, boost::vertex_index_t>::type> parent_map_t;
  ///Define a tree wrapper
  typedef boost::graph_as_tree<basegraph, parent_map_t> gametree;
};
#endif /* ABSTRACTGAMETREE_H_ */
