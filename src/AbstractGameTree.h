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

class AbstractUTCPolicy {
 public:
  AbstractUTCPolicy(){};
  virtual ~AbstractUTCPolicy(){};
  enum valuekind {
    visitcount = 0,
    wincount
  };
  virtual std::string print() = 0;
  virtual double estimate() = 0;
  virtual double calculate(AbstractUTCPolicy& parent) = 0;
  virtual bool update(valuekind indexofkind, int value, int increment = 0) = 0;
  virtual bool updateAll(valuekind visitkind, int valueofvisit,
                         int increaseofvisit, valuekind winkind, int valueofwin,
                         int increaseofwin) =0;
  virtual int feature(valuekind indexofkind) =0;
  virtual double getValue() const = 0;
  virtual double getBalance() const = 0;
};

class AbstractUTCPolicyImpl:public AbstractUTCPolicy{
 public:
  AbstractUTCPolicyImpl(){};
  virtual ~AbstractUTCPolicyImpl(){};
  enum valuekind {
    visitcount = 0,
    wincount
  };
  virtual std::string print() = 0;
  virtual double estimate() = 0;
  virtual double calculate(AbstractUTCPolicy& parent) = 0;
  virtual bool update(valuekind indexofkind, int value, int increment = 0) = 0;
  virtual bool updateAll(valuekind visitkind, int valueofvisit,
                         int increaseofvisit, valuekind winkind, int valueofwin,
                         int increaseofwin) =0;
  virtual int feature(valuekind indexofkind) =0;
  virtual double getValue() const = 0;
  virtual double getBalance() const = 0;
  //for locking
  virtual bool waitforupdate() = 0;
  virtual void notifyupdatedone() = 0;
  virtual int getCountforexpand() = 0;
  virtual void setCountforexpand(int value) = 0;
  virtual void addCountforexpand(int increment) = 0;
  virtual void subCountforexpand(int decrement) = 0;
  virtual int getNumofFutureChildren() = 0;
  virtual void setNumofFutureChildren(int numofchildren, int increment) = 0;
  virtual bool getIsupdated() const = 0;
  virtual void setIsupdated(bool isupdated) = 0;
};
class AbstractGameTree {
 public:
  //for retrieving tree information
  virtual std::size_t getNodeDepth(int indexofnode) = 0;
  virtual std::size_t getSizeofNodes() = 0;
  virtual std::size_t getSizeofEdges() = 0;
  virtual std::size_t getNodePosition(std::size_t indexofnode) = 0;
  virtual std::vector<std::size_t> getSiblings(std::size_t indexofnode) = 0;
  virtual void setNodePosition(std::size_t indexofnode, std::size_t position) = 0;

  //print out the tree
  virtual std::string printGameTree(int key) = 0;
  //for tree utility
  virtual void clearAll() = 0;
  //tree expansion
  virtual int expandNode(int indexofsource, int move, char color) = 0;
  //for simulation
  virtual int updateNodefromSimulation(int indexofnode, int winner, int level) = 0;
  virtual std::pair<int, double> getBestMovefromSimulation() = 0;
  virtual std::pair<int, int> selectMaxBalanceNode(int currentempty, bool isbreaktie) = 0;
  virtual void getMovesfromTreeState(
      int indexofnode, std::vector<int>& babywatsons, std::vector<int>& opponents,
      hexgame::unordered_set<int>& remainingmoves) = 0;
  //node property relevant
  virtual int getNodeValueFeature(int indexofnode,
                          AbstractUTCPolicy::valuekind indexofkind) = 0;
  virtual std::size_t getNumofTotalNodes() = 0;
  virtual std::size_t getNumofChildren(std::size_t indexofnode) = 0;
  virtual std::string name() = 0;

  AbstractGameTree() {};
  virtual ~AbstractGameTree() {};

 protected:
  //for property
  typedef boost::property<boost::vertex_value_t, hexgame::shared_ptr< AbstractUTCPolicy> > vertex_value_prop;
  typedef boost::property<boost::vertex_color_t, boost::default_color_type,
      vertex_value_prop> vertex_color_prop;
  typedef boost::property<boost::vertex_name_t, std::string, vertex_color_prop> vertex_name_prop;
  typedef boost::property<boost::vertex_position_t, std::size_t,
      vertex_name_prop> vertex_final_prop;

  //for graph and tree
  typedef boost::adjacency_list<boost::listS, boost::vecS,
      boost::bidirectionalS, vertex_final_prop> basegraph;  //adjacency_list<OutEdgeListS,VertexListS,DirectedS>

  //for graph traits
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
