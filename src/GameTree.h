/*
 * GameTree.h
 *
 *  Created on: Jan 1, 2014
 *      Author: renewang
 */
#ifndef GAMETREE_H_
#define GAMETREE_H_

#include <cmath>
#include <iterator>
#include <boost/graph/graph_as_tree.hpp>
#include <boost/graph/adjacency_list.hpp>

namespace boost {
//create an additional property
enum vertex_value_t {
  vertex_value
};
//for storing the calculation result of value function
enum vertex_position_t {
  vertex_position
};
enum vertex_key_t {
  vertex_key
};
BOOST_INSTALL_PROPERTY(vertex, value);
BOOST_INSTALL_PROPERTY(vertex, position);
BOOST_INSTALL_PROPERTY(vertex, key);}

//TODO
//1. template for featureholder
//2. move out UTCPolicy and think about the possible design to adapt different policies
class UTCPolicy {
 private:
  double value;  //this value is calculated by a value function (estimate) across simulated sample and needs to be maximized to get the best move
  double balance;  //this value is calculated by search policy, usually U (calculate) to balance exploration (random) and exploitation (greedy choice) and needs to be maximized to get the potential candidate
  const double coefficient;
  std::size_t numoffeatures;
  std::vector<int> featureholder;
 public:
  enum valuekind {
    visitcount = 0,
    wincount
  };
  UTCPolicy()
      : value(0.0),
        balance(value),
        coefficient(2.0),
        numoffeatures(2),
        featureholder(std::vector<int>(numoffeatures)) {
    std::fill(featureholder.begin(), featureholder.end(), 0);
  }
  ;
  UTCPolicy& operator=(const UTCPolicy& policy) {
    this->value = policy.value;
    this->balance = policy.balance;
    this->numoffeatures = policy.numoffeatures;
    this->featureholder = std::vector<int>(policy.featureholder);
    return *this;
  }
  std::string print() {
    std::stringstream buffer;
    buffer << " [" << value << "|" << balance << "|"
           << featureholder.at(wincount) << "|" << featureholder.at(visitcount)
           << "] ";
    return buffer.str();
  }
  double estimate() {
    assert(featureholder.at(visitcount) > 0);
    value = static_cast<double>(featureholder.at(wincount))
        / static_cast<double>(featureholder.at(visitcount));
    return value;
  }
  ;
  double calculate(UTCPolicy& parent, bool isgreedy = false) {
    //choose the maximal value for non-uct implementation (greedy mc) if isgreedy = true
    double value = estimate();
    if (!isgreedy) {
      //calculate UCT value (Upper Confidence Bound applied to Tree)
      //equation is used from Chaslot G et al,
      double vcount = static_cast<double>(featureholder.at(visitcount));
      double vcountofparent = static_cast<double>(parent.feature(visitcount));
      balance = (value
          + (std::sqrt(coefficient * std::log(vcountofparent) / vcount)));
    } else
      balance = value;
    return balance;
  }
  bool update(int indexofkind, int value, int increment = 0) {
    if (value == 0)  //simply incrementing original value
      featureholder.at(indexofkind) = featureholder.at(indexofkind) + increment;
    else
      featureholder.at(indexofkind) = value;
    return true;
  }
  ;
  int feature(int indexofkind) {
    return featureholder.at(indexofkind);
  }
  double getValue() const {
    return value;
  }

  double getBalance() const {
    return balance;
  }
};
class GameTree {
  //for property
  typedef boost::property<boost::vertex_value_t, UTCPolicy> vertex_value_prop;
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

  //for property map
  typedef boost::property_map<basegraph, boost::vertex_name_t>::type vertex_namemap_t;
  typedef boost::property_map<basegraph, boost::vertex_color_t>::type vertex_colormap_t;
  typedef boost::property_map<basegraph, boost::vertex_value_t>::type vertex_valuemap_t;
  typedef boost::property_map<basegraph, boost::vertex_position_t>::type vertex_positionmap_t;
  typedef boost::property_map<basegraph, boost::vertex_index_t>::type vertex_indexmap_t;

  //for tree
  typedef boost::iterator_property_map<std::vector<vertex_t>::iterator,
      boost::property_map<basegraph, boost::vertex_index_t>::type> parent_map_t;
  typedef boost::graph_as_tree<basegraph, parent_map_t> gametree;

 private:
  basegraph thetree;
  vertex_namemap_t vertexname;
  vertex_colormap_t vertexcolor;
  vertex_valuemap_t vertexvalue;
  vertex_positionmap_t vertexposition;
  vertex_indexmap_t vertexindex;
  vertex_t _root;

  vertex_t addNode(std::size_t positionofchild, char color);
  vertex_t getParent(vertex_t node);
  bool addEdge(vertex_t source, vertex_t target);
  void removeChidren(std::string nameofsource);
  void updateNodeName(vertex_t node);
  //update the value of a given node
  void updateNodeValue(vertex_t node, UTCPolicy& policy);
  void updateNodeValue(vertex_t node);
  //update the color of a given node
  void updateNodeColor(vertex_t node, char color);
  //update the board position of a given node
  void updateNodePosition(vertex_t node, std::size_t position);
  //re-root a subtree whose parents and siblings are removed if isprune is true
  void reRoot(vertex_t node, bool isprune);
  //prune the subtree of given vertex index
  void prune(vertex_t node);
  //Propagate the calculation from the leaf up to the root
  void backpropagate(vertex_t leaf, int value, int level);
  void traversetoPruneBy(std::size_t position, int level, vertex_t node);

  //for print
  class TreePrinter {
   private:
    std::ostream_iterator<std::string> _bufiter;
   public:
    TreePrinter(std::ostream_iterator<std::string>& bufiter)
        : _bufiter(bufiter) {
    }
    ;
    template<typename Node, typename Tree>
    void preorder(Node, Tree&) {
      *_bufiter++ = "(";
    }
    template<typename Node, typename Tree>
    void inorder(Node n, Tree& t) {
      *_bufiter++ = get(boost::vertex_name, t)[n]
          + get(boost::vertex_value, t)[n].print();
    }
    template<typename Node, typename Tree>
    void postorder(Node, Tree&) {
      *_bufiter++ = ")";
    }
  };

 public:
  GameTree();
  GameTree(char playerslabel);
  GameTree(char playerslabel, std::size_t indexofroot);

  //TODO generalize
  //for retreiving tree information
  std::size_t getNodeDepth(int indexofnode);
  std::size_t getSizeofNodes();
  std::size_t getSizeofEdges();
  std::size_t getNodePosition(std::size_t indexofnode);
  std::vector<std::size_t> getSiblings(std::size_t indexofnode);
  void setNodePosition(std::size_t indexofnode, std::size_t position);

  //print out the tree
  std::string printGameTree(int key);

  //for tree utility
  void clearAll();

  //tree expansion
  int expandNode(int indexofsource, int move, char color = 'W');

  //update by position of game
  void reRootfromSimulation(int indexofnode, bool isprune = true);

  //for simulation
  int updateNodefromSimulation(int indexofnode, int winner);
  void backpropagatefromSimulation(int indexofnode, int value, int level = -1);
  std::pair<int, double> getBestMovefromSimulation();
  int selectMaxBalanceNode(int currentempty, bool isbreaktie = true);
  void getMovesfromTreeState(int indexofchild, std::vector<int>& babywatsons,
                             std::vector<int>& opponents);
  void prunebyPosition(std::size_t position, int level);
  int reRootbyPosition(size_t position);

  //node property relevant
  int getNodeValueFeature(int indexofnode, int indexofkind);
};

#endif /* GAMETREE_H_ */
