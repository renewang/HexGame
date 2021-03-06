/*
 * GameTree.cpp
 * This file defines the implementation of GameTree and UTCPolicy classes
 *  Created on: Jan 3, 2014
 *      Author: renewang
 */

#include <cmath>
#include <utility>
#include <sstream>

#if __cplusplus > 199711L
#include <chrono>
#endif

#include "GameTree.h"
#include "PriorityQueue.h"

using namespace std;
using namespace boost;

///Estimate the winning rate
///@param NONE
///@return the estimated winning rate
double UTCPolicy::estimate() {
  assert(featureholder.at(visitcount) > 0);
  value = static_cast<double>(featureholder.at(wincount))
      / static_cast<double>(featureholder.at(visitcount));
  return value;
}
///Calculate the UTC value
///@param parent is the parent node whose features will be used for UTC calculation
///@return the calculated balance according the UTC Policy
double UTCPolicy::calculate(AbstractUTCPolicy& parent) {
  double value = estimate();
  //calculate UCT value (Upper Confidence Bound applied to Tree)
  //equation is used from Chaslot G et al,
  double vcount = static_cast<double>(featureholder.at(visitcount));
  double vcountofparent = static_cast<double>(parent.feature(visitcount));
  balance = (value
      + (std::sqrt(coefficient * std::log(vcountofparent) / vcount)));
  return balance;
}
///Update the value of a given feature index or AbstractUTCPolicy::valuekind
///@param indexofkind is a AbstractUTCPolicy::valuekind enum type which can serve as index to access feature
///@param value is the given value intended to be updated. Value will not be updated when the value is zero but incremented by specified increment parameter
///@param increment is the amount of self increment. Self increment only takes effect when value is zero
///@return boolean variable which indicates the update is successful
bool UTCPolicy::update(valuekind indexofkind, int value, int increment) {
  if (value == 0)  //simply incrementing original value
    featureholder.at(indexofkind) = featureholder.at(indexofkind) + increment;
  else
    featureholder.at(indexofkind) = value;
  return true;
}
///Update all the values of given features
///@param visitcount is AbstractUTCPolicy::visitkind which will update visit count feature
///@param valueofvisit is the given value intended to be updated for visitcount
///@param increaseofvisit is the amount of self increment for visitcount. Self increment only takes effect when value is zero
///@param wincount is AbstractUTCPolicy::wincount which will update winning count feature
///@param valueofwin is the given value intended to be updated for wincount
///@param increaseofwin is the amount of self increment for wincount. Self increment only takes effect when value is zero
///@return boolean variable which indicates the update is successful
bool UTCPolicy::updateAll(valuekind visitcount, int valueofvisit, int increaseofvisit,
               valuekind wincount, int valueofwin, int increaseofwin) {
  update(visitcount, valueofvisit, increaseofvisit);
  update(wincount, valueofwin, increaseofwin);
  return true;
}
#if __cplusplus > 199711L
///Default constructor which will initiate a game tree with one root node. The color of root is white and index of root is zero.
GameTree::GameTree()
    : GameTree('W', 0) {
}
///User defined constructor which will initiate a game tree with one root node. The color of root is the opposite of playerscolor. <br/>
///For example, if playerscolor is 'R', then root's color is 'B'. Conversely, playercolor is 'B', then root's color is 'R'.<br/>
///The index of root is initialized as zero.
GameTree::GameTree(char playerscolor)
    : GameTree(playerscolor, 0) {
}
#else
GameTree::GameTree() {
  initGameTree('W', 0);
}
GameTree::GameTree(char playerscolor) {
  initGameTree(playerscolor, 0);
}
#endif
///User defined constructor which will initiate a game tree with one root node. The color of root is the opposite of playerscolor. <br/>
///For example, if playerscolor is 'R', then root's color is 'B'. Conversely, playercolor is 'B', then root's color is 'R'.<br/>
///The index of root is initialized as given indexofroot.
GameTree::GameTree(char playerscolor, std::size_t indexofroot) {
  initGameTree(playerscolor, indexofroot);
}
/// Add a new node to the game tree
///@param positionofchild is the hex board position for the new ndoe
///@param color is the color label for the new node
///@return return a newly created vertex
GameTree::vertex_t GameTree::addNode(std::size_t positionofchild, char color) {
  vertex_t target = add_vertex(thetree);
  //TODO re-factor
  updateNodePosition(target, positionofchild);
  updateNodeColor(target, color);
  updateNodeValue(target);
  updateNodeName(target);
  return target;
}
/// Add a new edge between source node and target node
///@param source is the source node from which a new edge will be created.
///@param target is the target node to which a new edge will be created.
///@return A boolean variable indicates if adding edge is successful or not. True, the new edge is added successfully. False, not successfully.
bool GameTree::addEdge(vertex_t source, vertex_t target) {
  pair<edge_t, bool> result = add_edge(source, target, thetree);
  return result.second;
}
/// Print game tree in parenthesized representation
///@param index is the index of node whose subtree will be printed
///@return string representation of game tree in parenthesized format
string GameTree::printGameTree(int index) {
  stringstream treebuffer;
  ostream_iterator<string> treebufiter(treebuffer);
  TreePrinter printer(treebufiter);

  vertex_t node;
  node = vertex(index, thetree);
  gametree treeadptor(thetree, node);
  traverse_tree(node, treeadptor, printer);
  treebuffer << '\n';
  return treebuffer.str();
}
//if level = -1, then will do back-propagate up to the root, starting from current level = 0
/// Back propagate the play-out phase result till to the level specified
///@param leaf is the leaf node or starting node from which a back propagation will be executed
///@param value is the value used for update via back propagation
///@param level is the depth from root till which a back propagation will be executed
///@return NONE
void GameTree::backpropagate(vertex_t leaf, int value, int level) {
  int curlevel = level;
  vertex_t node = leaf, parent = graph_traits<basegraph>::null_vertex();
  int curvalue = -1 * value;
  while (node != _root) {
    //check in-edges
    in_edge_iter viter, viterend;
    assert(in_degree(node, thetree) == 1);
    for (tie(viter, viterend) = in_edges(node, thetree); viter != viterend;
        ++viter) {
      parent = source(*viter, thetree);
      get(vertex_value, thetree, parent).get()->updateAll(
          AbstractUTCPolicy_visitcount, 0, 1,
          AbstractUTCPolicy_wincount, 0, curvalue);
      if (get(vertex_color, thetree, parent)
          == get(vertex_color, thetree, _root) && (parent != _root))
        assert(
            get(vertex_value, thetree, parent).get()->feature(
                AbstractUTCPolicy_wincount) <= 0);  //make sure it's minimizing node
      else if (parent != _root)
        assert(
            get(vertex_value, thetree, parent).get()->feature(
                AbstractUTCPolicy_wincount) >= 0);  //make sure it's maximizing node

      assert(
          abs(get(vertex_value, thetree, parent).get()->feature(
              AbstractUTCPolicy_wincount))
              <= get(vertex_value, thetree, parent).get()->feature(
                  AbstractUTCPolicy_visitcount));
    }
    node = parent;
    curvalue = -1 * curvalue;
    --curlevel;
    if (curlevel == 0)
      break;
  }
}
//update name
/// Update node name
///@param node is the node whose name will be updated
///@return NONE
void GameTree::updateNodeName(vertex_t node) {
  stringstream namebuffer;
  namebuffer << get(vertex_index, thetree, node);
  namebuffer << "@";
  namebuffer << get(vertex_position, thetree, node);
  namebuffer << ":";
  if (get(vertex_color, thetree, node) == black_color)
    namebuffer << 'B';
  else if (get(vertex_color, thetree, node) == red_color)
    namebuffer << 'R';
  else
    namebuffer << 'W';
  put(vertex_name, thetree, node, namebuffer.str());
}
//update value
/// Update node value
///@param node is the node whose UTCPolicy value will be updated
///@return NONDE
void GameTree::updateNodeValue(vertex_t node) {
  put(vertex_value, thetree, node,
      hexgame::shared_ptr<AbstractUTCPolicy>(new UTCPolicy()));
}
//update color
/// Update node color
///@param node is the node whose color will be updated
///@param color is the new color label will be updated
///@return NONE
void GameTree::updateNodeColor(vertex_t node, char color) {
  switch (color) {
    case 'R':
      put(vertex_color, thetree, node, red_color);
      break;
    case 'B':
      put(vertex_color, thetree, node, black_color);
      break;
    default:
      put(vertex_color, thetree, node, white_color);
  }
}
//update position
/// Update node position on hex board
///@param node is the node whose position on hex board will be updated
///@return position is the hex board position will be updated
void GameTree::updateNodePosition(vertex_t node, std::size_t position) {
  put(vertex_position, thetree, node, position);
}
/// Get node position of a given index of node
///@param indexofnode is the index of the node in query
///@return return the position on hex board
size_t GameTree::getNodePosition(size_t indexofnode) {
  vertex_t node = vertex(indexofnode, thetree);
  return get(vertex_position, thetree, node);
}
/// Update node from play-out simulation result
///@param indexofnode is the index of node whose ancestral nodes will be updated during backpropagation phase. See backpropagate(vertex_t,int,int)
///@return NONE
void GameTree::updateNodefromSimulation(int indexofnode, int winner, int level) {
  vertex_t node = vertex(indexofnode, thetree);
  int value = 0;
  if (winner > 0) {
    value = get(vertex_value, thetree, node).get()->feature(
        AbstractUTCPolicy_wincount);
    if (get(vertex_color, thetree, node) == get(vertex_color, thetree, _root))  //a minimizing node
      get(vertex_value, thetree, node).get()->updateAll(
          AbstractUTCPolicy_visitcount, 0, 1,
          AbstractUTCPolicy_wincount, 0, -1);
    else
      get(vertex_value, thetree, node).get()->updateAll(
          AbstractUTCPolicy_visitcount, 0, 1,
          AbstractUTCPolicy_wincount, 0, 1);
    value = get(vertex_value, thetree, node).get()->feature(
        AbstractUTCPolicy_wincount) - value;
  } else
    get(vertex_value, thetree, node).get()->updateAll(
        AbstractUTCPolicy_visitcount, 0, 1,
        AbstractUTCPolicy_wincount, 0, 0);
  backpropagate(node, value, level);
}
/// Expand a new node from given source node and update its position and color information
///@param indexofsource is the index of source node from which a new node will be expanded
///@param move is the position on hex board for the new expanded node
///@param color is the color label for the new expanded node
///@return the index of the new expanded node
int GameTree::expandNode(int indexofsource, int move, char color) {
  vertex_t source = vertex(indexofsource, thetree);
  int indexofchild = -1;

  //adding the different color from the parental node
  vertex_t target;
  if (get(vertex_color, thetree, source) == red_color)
    target = addNode(static_cast<size_t>(move), 'B');
  else if (get(vertex_color, thetree, source) == black_color)
    target = addNode(static_cast<size_t>(move), 'R');
  else
    target = addNode(static_cast<size_t>(move), color);

  if (addEdge(source, target))
    indexofchild = get(vertex_index, thetree, target);

  assert(indexofchild > 0);
  return indexofchild;
}
//called in MCST selection phase
/// Select the node with the maximal UTC value
///@param currentempty is the left empty position on the hex board in actual game state
///@param isbreaktie is the boolean variable which indicates if should break tie by random choice.
///@return a pair of integer and size_t. The first in pair is the index of selected node with maximal UTC value. The second is the level or depth at which the selected node locates.
pair<int,size_t> GameTree::selectMaxBalanceNode(int currentempty, bool isbreaktie) {
  vertex_t parent = _root;
  out_edge_iter viter, viterend;
  size_t numofchildren = out_degree(parent, thetree);

  int level = 0;

#if __cplusplus > 199711L
  default_random_engine generator(
      static_cast<unsigned>(hexgame::chrono::system_clock::now()
          .time_since_epoch().count()));
  uniform_real_distribution<double> distribution(0.0, 1.0);
#else
  srand(static_cast<unsigned>(time(NULL)));
#endif
  while (numofchildren != 0) {  //reach leaf
    //test if the current examining node is fully expanded, if yes then return its child; otherwise, return the current node for expansion
    assert((currentempty - level) > 0);  //currentempty - level = 0 indicates the end of game
    if (static_cast<int>(numofchildren) < currentempty - level)
      break;

    PriorityQueue<vertex_t, double> vertexchooser(numofchildren);
    for (tie(viter, viterend) = out_edges(parent, thetree); viter != viterend;
        ++viter) {
      vertex_t node = target(*viter, thetree);
      double value = -get(vertex_value, thetree, node).get()->calculate(
          (*get(vertex_value, thetree, parent)));
      if (isbreaktie && vertexchooser.containsPriority(value)) {  // break the tie

#if __cplusplus > 199711L
        double prob = distribution(generator);
#else
        double prob = (float) rand() / RAND_MAX;
#endif

        if (prob < 0.5)
          value -= prob / 100.0;
        else
          value += prob / 100.0;
      }
      vertexchooser.insert(node, value);
    }
    parent = vertexchooser.minPrioirty();
    numofchildren = out_degree(parent, thetree);

    ++level;
  }
  return make_pair(get(vertex_index, thetree, parent), level);
}
/// Get the best move from many play-out simulations via maximizing winning rate
///@param NONE
///@return a pair of integer and double. The first is the position on the hex board; while the second is the maximal UTC value calculated.
pair<int, double> GameTree::getBestMovefromSimulation() {
  //1. examine all children nodes below the root node
  size_t numofchildren = out_degree(_root, thetree);
  assert(numofchildren != 0);
  PriorityQueue<vertex_t, double> vertexchooser(numofchildren);
  out_edge_iter viter, viterend;
  for (tie(viter, viterend) = out_edges(_root, thetree); viter != viterend;
      ++viter) {
    vertex_t node = target(*viter, thetree);
    vertexchooser.insert(node,
                         -get(vertex_value, thetree, node).get()->estimate());
  }
  //2. choose the maximal value from all children nodes of root.
  int indexofbestmove = vertexchooser.minPrioirty();
  double maxvalue = get(vertex_value, thetree,
                        get(vertex_index, thetree, indexofbestmove)).get()
      ->getValue();
  assert(indexofbestmove > 0);
  assert(maxvalue >= 0.0);
  return make_pair(indexofbestmove, maxvalue);
}
/// Reconstruct simulated game history from game tree and return the past moves through collect moves from path starting from given node
///@param indexofnode is the index of node which will serve as starting point from which the positions of all ancestral nodes will be collected to construct simulated game history
///@param babywatsons is the vector which stores the moves made by AI player in the simulated game history
///@param opponents is the vector which stores the moves made by virtual opponents in the simulated game history
///@param remainingmoves is an auxiliary data structure which will store the remaining moves after reconstructing simulated game history
///@return NONE
void GameTree::getMovesfromTreeState(
    int indexofnode, vector<int>& babywatsons, vector<int>& opponents,
    hexgame::unordered_set<int>& remainingmoves) {

  vertex_t node = vertex(indexofnode, thetree), chosenleaf = node;
  in_edge_iter viter, viterend;

  while (node != _root) {
    if (node != chosenleaf) {
      if (get(vertex_color, thetree, node) != get(vertex_color, thetree, _root))
        babywatsons.push_back(get(vertex_position, thetree, node));
      else
        opponents.push_back(get(vertex_position, thetree, node));
      assert(get(vertex_position, thetree, node) != 0);
      assert(remainingmoves.count(get(vertex_position, thetree, node)) != 0);
      remainingmoves.erase(get(vertex_position, thetree, node));
    }
    assert(in_degree(node, thetree) == 1);
    for (tie(viter, viterend) = in_edges(node, thetree); viter != viterend;
        ++viter)
      node = source(*viter, thetree);
  }

  if (get(vertex_position, thetree, chosenleaf) == 0)  //new node
      {
    vector<size_t> siblings = getSiblings(chosenleaf);
#if __cplusplus > 199711L
    for (auto iter = siblings.begin(); iter != siblings.end();
        ++iter)
#else
    for (vector<size_t>::iterator iter = siblings.begin(); iter != siblings.end(); ++iter)
#endif
    {
      size_t pos = getNodePosition(*iter);
      assert(remainingmoves.count(pos) != 0);
      remainingmoves.erase(pos);
    }
#if __cplusplus > 199711L
    default_random_engine generator(
          static_cast<unsigned>(hexgame::chrono::system_clock::now()
              .time_since_epoch().count()));
    uniform_int_distribution<int> distribution(0, remainingmoves.size()-1);
    size_t index = distribution(generator);  // generates number in the range 1..6
    assert(index < remainingmoves.size());
#else
    srand(static_cast<unsigned>(time(NULL)));
    size_t index = rand() % remainingmoves.size();
#endif
    hexgame::unordered_set<int>::iterator iter = remainingmoves.begin();
    for (size_t i = 0; i < index; ++i)
      ++iter;
    int move = *iter;
    assert(remainingmoves.count(move));
    setNodePosition(chosenleaf, move);
  }
  if (get(vertex_color, thetree, chosenleaf) != get(vertex_color, thetree, _root))
    babywatsons.push_back(get(vertex_position, thetree, chosenleaf));
  else
    opponents.push_back(get(vertex_position, thetree, chosenleaf));
}
/// Get UTC value according to the specified feature
///@param indexofnode is the index of node whose UTC value of specified feature will be returned
///@param indexofkind is one of valuekind which will specify what value of features should be returned. Either wincount or visitcount
///@return the return UTC value
int GameTree::getNodeValueFeature(int indexofnode,
                                  AbstractUTCPolicy::valuekind indexofkind) {
  vertex_t node = vertex(indexofnode, thetree);
  return get(vertex_value, thetree, node).get()->feature(indexofkind);
}
/// Clear all nodes and edges of a game tree
///@param NONE
///@return NONE
void GameTree::clearAll() {
  char rootscolor = 'W';
  if (get(vertex_color, thetree, _root) == red_color)
    rootscolor = 'R';
  else if (get(vertex_color, thetree, _root) == black_color)
    rootscolor = 'B';
  int indexofroot = get(vertex_index, thetree, _root);
  thetree.clear();
  vertex_t root = addNode(indexofroot, rootscolor);
  _root = root;
}
/// Get the total size of nodes
///@param NONE
///@return the total size of nodes
size_t GameTree::getSizeofNodes() {
  return num_vertices(thetree);
}
/// Get the total size of edges
///@param NONE
///@return the total size of edges
size_t GameTree::getSizeofEdges() {
  size_t numofedges = num_edges(thetree);
  assert(numofedges == num_vertices(thetree) - 1);
  return numofedges;
}
/// Get the depth of a given node
///@param indexofnode is the index of node whose depth or level from root will be returned
///@return the depth or level from root
size_t GameTree::getNodeDepth(int indexofnode) {
  size_t depth = 0;
  vertex_t node = vertex(indexofnode, thetree);
  while (node != _root) {
    //check in-edges
    in_edge_iter viter, viterend;
    assert(in_degree(node, thetree) == 1);
    for (tie(viter, viterend) = in_edges(node, thetree); viter != viterend;
        ++viter) {
      node = source(*viter, thetree);
      ++depth;
    }
  }
  return depth;
}
/// Set the position of node
///@param indexofnode is the index of node whose position will be updated
///@param position is the new position on hex board for the given node
///@return NONE
void GameTree::setNodePosition(size_t indexofnode, size_t position) {
  vertex_t node = vertex(indexofnode, thetree);
  updateNodePosition(node, position);
  updateNodeName(node);
}
/// Get the siblings of a given node
///@param indexofnode is the index of node whose indices of siblings will be returned
///@return a vector of size_t which stores the indices of siblings of the given node
vector<size_t> GameTree::getSiblings(size_t indexofnode) {
  vertex_t node = vertex(indexofnode, thetree), chosenleaf = node;
  vertex_t parent = getParent(node);
  vector<size_t> siblings;
  siblings.reserve(out_degree(parent, thetree));
  out_edge_iter viter, viterend;
  for (tie(viter, viterend) = out_edges(parent, thetree); viter != viterend;
      ++viter) {
    vertex_t child = target(*viter, thetree);
    if (child != chosenleaf)
      siblings.push_back(get(vertex_index, thetree, child));
  }
  return vector<size_t>(siblings);
}
/// Get parent of a given node
///@param node is the node whose parent will be returned
///@return the parental node
GameTree::vertex_t GameTree::getParent(vertex_t node) {
  in_edge_iter viter, viterend;
  assert(in_degree(node, thetree) == 1);
  vertex_t parent = graph_traits<basegraph>::null_vertex();
  for (tie(viter, viterend) = in_edges(node, thetree); viter != viterend;
      ++viter) {
    parent = source(*viter, thetree);
  }
  return parent;
}
/// Initialize lockable game tree with the root node of the specified color label and index
///@param playerscolor is the color label which will be assigned to new root node
///@param indexofroot is the index which will be assigned to new root node
///@return NONE
void GameTree::initGameTree(char playerscolor, size_t indexofroot) {
  //set the root's color label as opponent's color label
  char rootscolor = 'W';
  if (playerscolor == 'R')
    rootscolor = 'B';
  else if (playerscolor == 'B')
    rootscolor = 'R';
  vertex_t root = addNode(indexofroot, rootscolor);
  _root = root;
}
/// Get the number of children of a given node
///@param indexofnode is the index of the node whose number of children will be returned
///@return the number of children nodes
size_t GameTree::getNumofChildren(size_t indexofnode) {
  vertex_t parent = vertex(indexofnode, thetree);
  return (out_degree(parent, thetree));
}
