/*
 * GameTree.h
 * This file defines the implementation for Monte Carlo Game Tree and UTC Policy. UTC Policy is part of Game Tree property.
 *  Created on: Jan 1, 2014
 *      Author: renewang
 */
#ifndef GAMETREE_H_
#define GAMETREE_H_

#include "AbstractGameTree.h"

#include <cmath>
#include <iterator>

//TODO template for featureholder
/**
 * UTCPolicy class is used to provide implementations for UTC Policy calculation
 * UTCPolicy(): is parameterless default constructor which initialize data members
 */
class UTCPolicy: public AbstractUTCPolicy{
 private:
  double value;///< This value is calculated by a value function (estimate) across simulated sample and needs to be maximized to get the best move
  double balance;///< This value is calculated by search policy, usually U (calculate) to balance exploration (random) and exploitation (greedy choice) and needs to be maximized to get the potential candidate
  const double coefficient;///< This constant value is used in UTC Policy to strike a balance between exploration and exploitation
  std::size_t numoffeatures;///< This value is used to specify how many features are used for UTC value calculation
  std::vector<int> featureholder;///< This vector is used to hold the values of features

 public:
  ///Parameterless default constructor which initialize data member with default values
  UTCPolicy()
      : value(0.0),
        balance(value),
        coefficient(2.0),
        numoffeatures(2),
        featureholder(std::vector<int>(numoffeatures)) {
    std::fill(featureholder.begin(), featureholder.end(), 0);
  }
  ;
  ///Copy assignment which should perform deep copy
  UTCPolicy& operator=(const UTCPolicy& policy) {
    this->value = policy.value;
    this->balance = policy.balance;
    this->numoffeatures = policy.numoffeatures;
    this->featureholder = std::vector<int>(policy.featureholder);
    return *this;
  }
  ///Print the values of UTC object, debug purpose
  std::string print() {
    std::stringstream buffer;
    buffer << " [" << value << "|" << balance << "|"
           << featureholder.at(wincount) << "|" << featureholder.at(visitcount)
           << "] ";
    return buffer.str();
  }
  ///Estimate the winning rate
  double estimate();
  ///Calculate the UTC value
  double calculate(AbstractUTCPolicy& parent);
  ///Update the value for a given feature
  bool update(valuekind indexofkind, int value, int increment = 0);
  ///Update the value for all features
  bool updateAll(valuekind visitcount, int valueofvisit, int increaseofvisit,
                 valuekind wincount, int valueofwin, int increaseofwin);
  ///Get the value of a particular feature
  int feature(valuekind indexofkind) {
    return featureholder.at(indexofkind);
  }
  ///Get the estimated winning rate
  double getValue() const {
    return value;
  }
  ///Get the calculated balance according to UTC policy
  double getBalance() const {
    return balance;
  }
};
/**
 * GameTree class defines the implementations of AbstractGameTree
 * GameTree(); parameterless default constructor which will initiate a game tree with one white root node
 * GameTree(char playerslabel): User defined constructor which will initiate a game tree with one root node whose color is opposite to playerslabel.
 * GameTree(char playerslabel, std::size_t indexofroot): User defined constructor which will initiate a game tree with one root node whose color is opposite to playerslabel and index is indexofroot.
 */
class GameTree: public AbstractGameTree{
 private:
  basegraph thetree; ///< boost adjacency list which is the underlying graph structure implementation and is alternative defined as basegraph in AbstractGameTree
  vertex_t _root; ///< root node of game tree

#ifndef NDEBUG
  friend class LockableGameTree;
#endif

 private:
  ///Add an unconnected node to the game tree with the given position on board or the color of new node
  vertex_t addNode(std::size_t positionofchild, char color);
  ///Get the parent node of given node
  vertex_t getParent(vertex_t node);
  ///Add the edge between source node and target node
  bool addEdge(vertex_t source, vertex_t target);
  //void updateNodeName(vertex_t node, std::size_t positionofchild, char color); //TODO modify this, enforce to update node color and position before update node name
  ///Update name of given node
  void updateNodeName(vertex_t node);
  ///Update the value of a given node
  void updateNodeValue(vertex_t node);
  ///Update the color of a given node
  void updateNodeColor(vertex_t node, char color);
  ///Update the board position of a given node
  void updateNodePosition(vertex_t node, std::size_t position);
  ///Utility function to initialize a game tree with one single node whose color is opponent's color and index is given index
  virtual void initGameTree(char playerscolor, size_t indexofroot);
  ///Propagate the calculation from the leaf up to the root
  void backpropagate(vertex_t leaf, int value, int level);
  ///Getter to return the root node
  vertex_t getRoot() const {
    return _root;
  }
  ///Getter to return the underlying boost adjacency implementation aka basegraph
  basegraph& getTree(){return thetree;}

  /**
   * TreePrinter is the enclosed class which is used to define what actions to perform in Depth First Search (DFS) traversal<br/>
   * TreePrinter(std::ostream_iterator<std::string>& bufiter): constructor which takes iterator of ostream for print out<br/>
   * Sample Usage:<br/>
   *  stringstream treebuffer; <br/>
   *  ostream_iterator<string> treebufiter(treebuffer);<br/>
   *  TreePrinter printer(treebufiter);<br/>
   */
  class TreePrinter {
   private:
    std::ostream_iterator<std::string> _bufiter;///< string buffer to return parenthesized tree representation as string
   public:
    ///constructor which takes string iterator of ostream object for print out
    ///@param bufiter is string iterator of ostream
    TreePrinter(std::ostream_iterator<std::string>& bufiter)
        : _bufiter(bufiter) {
    }
    ;
    ///specify an action for preorder traversal
    ///@param Node is the node after DFS visiting
    ///@param Tree is the tree on which DFS traverse
    ///@return NONE
    template<typename Node, typename Tree>
    void preorder(Node, Tree&) {
      *_bufiter++ = "(";
    }
    ///specify an action for inorder traversal
    ///@param Node n is the node which DFS visits
    ///@param Tree t is the tree on which DFS traverse
    ///@return NONE
    template<typename Node, typename Tree>
    void inorder(Node n, Tree& t) {
      *_bufiter++ = get(boost::vertex_name, t)[n]
          + get(boost::vertex_value, t)[n].get()->print();
    }
    ///specify an action for inorder traversal
    ///@param Node n is the node after DFS visiting
    ///@param Tree t is the tree on which DFS traverse
    ///@return NONE
    template<typename Node, typename Tree>
    void postorder(Node, Tree&) {
      *_bufiter++ = ")";
    }
  };

 public:
  ///Parameterless default constructor which will initialize a game tree with a single white root node
  GameTree();
  ///User defined game tree which will initialize a game tree with a single root node with opponent's color
  GameTree(char playerslabel);
  ///User defined game tree which will initialize a game tree with a single root node with opponent's color and given indexofroot
  GameTree(char playerslabel, std::size_t indexofroot);
  ///destructor
  virtual ~GameTree() {
  }
  ;

  //for retrieving tree information
  ///Get the depth from root for a given node
  std::size_t getNodeDepth(int indexofnode);
  ///Get the size of total nodes
  std::size_t getSizeofNodes();
  ///Get the size of total edges
  std::size_t getSizeofEdges();
  ///Get the position of board given the index of node
  std::size_t getNodePosition(std::size_t indexofnode);
  ///Get the siblings of a node given the index of node
  std::vector<std::size_t> getSiblings(std::size_t indexofnode);
  ///Set the position of board given the index of node
  void setNodePosition(std::size_t indexofnode, std::size_t position);

  ///Print out the tree
  std::string printGameTree(int key);
  //for tree utility
  ///Clear out the nodes and edge
  void clearAll();
  //tree expansion
  ///Expand one node from given node and specify the information of position on board and color
  int expandNode(int indexofsource, int move, char color = 'W');
  //for simulation
  ///Update play-out simulation result for the given index of node called in Monte Carlo Tree Search back-propagation phase
  void updateNodefromSimulation(int indexofnode, int winner, int level = -1);
  ///Select the best move with maximal winning rate from the children nodes of the root after a round of play-out simulation
  std::pair<int, double> getBestMovefromSimulation();
  ///select the node with the maximal UTC value called in Monte Carlo Tree Search selection phase
  std::pair<int, int> selectMaxBalanceNode(int currentempty, bool isbreaktie = true);
  ///Get the moves from the given the index of node to root in order to construct the game history from game tree
  void getMovesfromTreeState(
      int indexofnode, std::vector<int>& babywatsons, std::vector<int>& opponents,
      hexgame::unordered_set<int>& remainingmoves);
  //node property relevant
  ///Get the UTC winning count or visit count
  int getNodeValueFeature(int indexofnode, AbstractUTCPolicy::valuekind indexofkind);
  ///Get the number of children for a given index of node
  std::size_t getNumofChildren(std::size_t indexofnode);
  ///Return the meaningful class name as "GameTree"
  std::string name() {
    return std::string("GameTree");
  }
  ;
};

#endif /* GAMETREE_H_ */
