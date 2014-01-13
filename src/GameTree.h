/*
 * GameTree.h
 *
 *  Created on: Jan 1, 2014
 *      Author: renewang
 */
#ifndef GAMETREE_H_
#define GAMETREE_H_

#include <iterator>
#include <boost/dynamic_bitset.hpp>
#include <boost/functional/hash.hpp>
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
BOOST_INSTALL_PROPERTY(vertex, key);
}
//TODO, temporary
class UTCPolicy {
 private:
  std::size_t visitcount;
  double value;
  const double coefficient;
 public:
  UTCPolicy()
      : visitcount(0),
        value(0.0),
        coefficient(1.0) {
  }
  ;
  UTCPolicy& operator=(const UTCPolicy& policy) {
    this->visitcount = policy.visitcount;
    this->value = policy.value;
    return *this;
  }
  double calculate();
};
class GameTree {
  //for property
  typedef boost::property<boost::vertex_value_t, UTCPolicy> vertex_value_prop;
  typedef boost::property<boost::vertex_color_t, boost::default_color_type,
      vertex_value_prop> vertex_color_prop;
  typedef boost::property<boost::vertex_name_t, std::string, vertex_color_prop> vertex_name_prop;
  typedef boost::property<boost::vertex_position_t, std::size_t,
      vertex_name_prop> vertex_position_prop;
  typedef boost::property<boost::vertex_key_t,
      boost::dynamic_bitset<unsigned char>, vertex_position_prop> vertex_final_prop;

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
  typedef boost::property_map<basegraph, boost::vertex_key_t>::type vertex_keymap_t;

  //for tree
  typedef boost::iterator_property_map<std::vector<vertex_t>::iterator,
      boost::property_map<basegraph, boost::vertex_key_t>::type> parent_map_t;
  typedef boost::graph_as_tree<basegraph, parent_map_t> gametree;

 private:
  basegraph thetree;
  vertex_namemap_t vertexname;
  vertex_colormap_t vertexcolor;
  vertex_valuemap_t vertexvalue;
  vertex_positionmap_t vertexposition;
  vertex_keymap_t vertexkey;
  vertex_t _root;

  //boost::hash<std::string> _namehash;
  //original idea to use _namehash is to have the following lookup:
  //string nameoftarget;
  //to_string(keyoftarget, nameoftarget);
  //size_t indexoftarget = _namehash(nameoftarget);
  //vertex_t target = vertex(indexoftarget, thetree);
  //Major problem: the hash value provided by string object is still too large
  //to fit in the maximal size of vector<size_t> which is used for index_map
  //in boost. Defer the implementation until I fully understand trie structure

  //add the edge between indeoxofsource and indexoftarget
  bool addChild(vertex_t source, boost::dynamic_bitset<unsigned char> keyoftarget,
               std::size_t positionofchild, char color);
  void removeChidren(std::string nameofsource);
  //update the name of a given node
  void updateNodeName(vertex_t node, boost::dynamic_bitset<unsigned char> key);
  void updateNodeName(vertex_t node, std::string nameofnode);
  //update the key of a given node
  void updateNodeKey(vertex_t node, boost::dynamic_bitset<unsigned char> key);
  //update the value of a given node
  void updateNodeValue(vertex_t node, UTCPolicy policy);
  //update the color of a given node
  void updateNodeColor(vertex_t node, char color);
  //update the board position of a given node
  void updateNodePosition(vertex_t node, std::size_t position);
  //re-root a subtree whose parents and siblings are removed if isprune is true
  void reRoot(std::string key, bool isprune);
  //prune the subtree of given vertex index
  void prune(std::string nameofindex);
  //Propagate the calculation from the leaf up to the root
  void backpropogate(boost::dynamic_bitset<unsigned char> indexofoffspring,
                     int level);

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
      *_bufiter++ = get(boost::vertex_name, t)[n];
    }
    template<typename Node, typename Tree>
    void postorder(Node, Tree&) {
      *_bufiter++ = ")";
    }
  };
 public:
  GameTree(std::size_t indexofroot);

  //print out the tree
  std::string printGameTree(std::string key);

  //tree expansion
  bool expandLeaf(std::string nameofparent, int move, char color = 'W');

  //update by position of game
  void prunebyPosition(std::size_t position);
  void reRootbyPosition(std::size_t position, bool isprune);

  //for simulation
  void addSimulatedMove(int move, char color);
  void updateNodefromSimulation(int winner);
  void backpropogatefromSimulation();
  std::pair<std::vector<int>, std::vector<int>> getMovesfromSimulation();
  double getBestMoveValuefromSimulation();
  int getBestMovefromSimulation();
  //to calculate the upper confidence bound according to a value function to evaluate the selection  of a move
  std::string calcUpperConfidenceBound();
};

#endif /* GAMETREE_H_ */
