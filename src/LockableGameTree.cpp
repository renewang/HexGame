/*
 * LockableGameTree.cpp
 *
 *  Created on: Feb 11, 2014
 *      Author: renewang
 */

#include <ctime>
#include <deque>
#include <stack>
#include <utility>
#include <iostream>

#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_algorithms.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include "Game.h"
#include "Global.h"
#include "Player.h"
#include "PriorityQueue.h"
#include "LockableGameTree.h"
#include "MultiMonteCarloTreeSearch.h"

using namespace std;
using namespace boost;

/// Let the thread wait for the update of feature values from back-propagation phase in MCTS
///@param NONE
///@return boolean indicator which will return true when complete waiting
bool LockableUTCPolicy::waitforupdate() {
  boost::unique_lock<LockableUTCPolicy> guard(*this);
  holdforupdate.wait(guard);
  return true;
}
/// Notify all waiting threads that update in back-propagation phase already finishes
///@param NONE
///@return NONE
void LockableUTCPolicy::notifyupdatedone() {
  holdforupdate.notify_all();
}
double LockableUTCPolicy::estimate() {
  return policy.estimate();
}
double LockableUTCPolicy::calculate(AbstractUTCPolicy& parent) {
  return policy.calculate(parent);
}
bool LockableUTCPolicy::update(valuekind indexofkind, int value,
                               int increment) {
  policy.update(indexofkind, value, increment);
  return true;
}
bool LockableUTCPolicy::updateAll(valuekind visitkind, int valueofvisit,
                                  int increaseofvisit, valuekind winkind,
                                  int valueofwin, int increaseofwin) {
  policy.updateAll(visitkind, valueofvisit, increaseofvisit, winkind, valueofwin, increaseofwin);
  isupdated = true;
  return true;
}
#if __cplusplus > 199711L
///Default constructor which will construct a game tree whose root has color label as 'W' (white) and index as 0. See also GameTree::GameTree()
///@param NONE
LockableGameTree::LockableGameTree()
    : LockableGameTree('W', 0) {
}
///Constructor will construct a game tree whose root with specified color and index as 0. See also GameTree::GameTree(char)
///@param playerscolor is the color for root node.
LockableGameTree::LockableGameTree(char playerscolor)
    : LockableGameTree(playerscolor, 0) {
}
#else
LockableGameTree::LockableGameTree()
: lockable_share_type() {
  initGameTree('W', 0);
}
LockableGameTree::LockableGameTree(char playerscolor)
: lockable_share_type() {
  initGameTree(playerscolor, 0);
}
#endif
///Constructor will construct a game tree whose root with specified color and index as indexofroot. See also GameTree::GameTree(char, size_t)
///@param playerscolor is the color for root node.
///@param indexofroot is the index for the root node
LockableGameTree::LockableGameTree(char playerscolor, std::size_t indexofroot)
    : lockable_share_type() {
  initGameTree(playerscolor, indexofroot);
}
/// Notify all updates are done for a game tree
///@param unique_lock<LockableGameTree>& is a guard which will lock GameTree against other access
///@param leaf is the leaf node which will be the starting node to notify update complete via back-propagation
///@return boolean variable is used to indicate all notification of update complete is done via back-propagation
bool LockableGameTree::notifyAllUpdateDone(boost::unique_lock<LockableGameTree>&,
                                           vertex_t leaf, int level) {
  int curlevel = level;
  vertex_t node = leaf, parent = graph_traits<basegraph>::null_vertex();
  get(vertex_value, thetree, node).get()->notifyupdatedone();
  while (node != _root) {
    in_edge_iter viter, viterend;
    for (tie(viter, viterend) = in_edges(node, thetree); viter != viterend;
        ++viter) {
      parent = source(*viter, thetree);
      get(vertex_value, thetree, parent).get()->notifyupdatedone();
    }
    node = parent;
    --curlevel;
    if (curlevel == 0)
      break;
  }
  return true;
}
/// Initialize lockable game tree with the root node of the specified color label and index
///@param playerscolor is the color label which will be assigned to new root node
///@param indexofroot is the index which will be assigned to the new root node
///@return NONE
void LockableGameTree::initGameTree(char playerscolor, std::size_t indexofroot) {
  //set the root's color label as opponent's color label
  //TODO duplicate code
  char rootscolor = 'W';
  if (playerscolor == 'R')
    rootscolor = 'B';
  else if (playerscolor == 'B')
    rootscolor = 'R';
  vertex_t root = addNode(indexofroot, rootscolor);
  _root = root;
  countonwait.store(0);
  countforexpand.store(0);
  isblockingforexpand.store(false);
}
/// Add Node to the game tree with external lock
///@param unique_lock<LockableGameTree>& is a reader/writer's lock which will grant exclusive right for game tree access
///@param positionofchild is the position which will be updated
///@param color is the labeled color which will be updated
///@return the newly created vertex or node
LockableGameTree::vertex_t LockableGameTree::addNode(
    boost::unique_lock<LockableGameTree>&, std::size_t positionofchild,
    char color) {
  //TODO duplicate code
  vertex_t target = add_vertex(thetree);
  updateNodeValue(target);
  unique_lock<LockableUTCPolicy> guard(*get(vertex_value, thetree, target));
  updateNodePosition(guard, target, positionofchild);
  updateNodeColor(guard, target, color);
  updateNodeName(guard, target);
  return target;
}
///Add Node to the game tree with internal lock
///@param positionofchild is the position which will be updated
///@param color is the labeled color which will be updated
LockableGameTree::vertex_t LockableGameTree::addNode(std::size_t positionofchild,
                                                     char color) {
  unique_lock<LockableGameTree> guard(*this);
  return addNode(guard, positionofchild, color);
}
/// Print game tree in parenthesized representation with external lock
///@param shared_lock<LockableGameTree>& is a reader's lock which will grant shared right for game tree access
///@param index is the index of node whose subtree will be printed
///@return string representation of game tree in parenthesized format
string LockableGameTree::printGameTree(boost::shared_lock<LockableGameTree>&,
                                       int index) {
  //TODO duplicate code
  stringstream treebuffer;
  ostream_iterator<string> treebufiter(treebuffer);
  LockableGameTree::TreePrinter printer(treebufiter);
  vertex_t node;
  node = vertex(index, thetree);
  gametree treeadptor(thetree, node);
  traverse_tree(node, treeadptor, printer);
  treebuffer << '\n';
  return treebuffer.str();
}
/// Print game tree in parenthesized representation with internal lock
///@param index is the index of node whose subtree will be printed
///@return string representation of game tree in parenthesized format
string LockableGameTree::printGameTree(int index) {
  shared_lock<LockableGameTree> guard(*this);
  return printGameTree(guard, index);
}
//external lock version
//update name
/// Update node name with external lock
///@param unique_lock<LockableUTCPolicy>& is a reader/writer's lock which will grant exclusive right for local node of game tree access
///@param node is the node whose name will be updated
///@return NONE
void LockableGameTree::updateNodeName(boost::unique_lock<LockableUTCPolicy>&,
                                      vertex_t node) {
  //TODO duplicate code
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
/// Update node color with internal lock
///@param node is the node whose color label will be updated
///@return NONE
void LockableGameTree::updateNodeName(vertex_t node) {
  unique_lock<LockableUTCPolicy> guard(*get(vertex_value, thetree, node));
  updateNodeName(node);
}
//update the value of a given node
/// Update node value with internal lock
///@param node is the node whose name will be updated
///@return NONE
void LockableGameTree::updateNodeValue(vertex_t node) {
  //TODO duplicate code
  put(vertex_value, thetree, node,
      hexgame::shared_ptr<LockableUTCPolicy>(new LockableUTCPolicy()));
}
//update the color of a given node
/// Update node value with external lock
///@param unique_lock<LockableUTCPolicy>& is a reader/writer's lock which will grant exclusive right for local node of game tree access
///@param node is the node whose color will be updated
///@param color is the new color label being updated
///@return NONE
void LockableGameTree::updateNodeColor(boost::unique_lock<LockableUTCPolicy>&,
                                       vertex_t node, char color) {
  //TODO duplicate code
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
//update the color of a given node
/// Update node value with external lock
///@param unique_lock<LockableUTCPolicy>& is a reader/writer's lock which will grant exclusive right for local node of game tree access
///@param node is the node whose color will be updated
///@param color is the new color label being updated
///@return NONE
void LockableGameTree::updateNodeColor(vertex_t node, char color) {
  unique_lock<LockableUTCPolicy> guard(*get(vertex_value, thetree, node));
  updateNodeColor(guard, node, color);
}
//update the board position of a given node
/// Update node position with external lock
///@param unique_lock<LockableUTCPolicy>& is a reader/writer's lock which will grant exclusive right for local node of game tree access
///@param node is the node whose position will be updated
///@param position is the new position will be updated
///@return NONE
void LockableGameTree::updateNodePosition(boost::unique_lock<LockableUTCPolicy>&,
                                          vertex_t node, std::size_t position) {
  //TODO duplicate code
  put(vertex_position, thetree, node, position);
}
/// Update node position with external lock
///@param node is the node whose position will be updated
///@param position is the new position will be updated
///@return NONE
void LockableGameTree::updateNodePosition(vertex_t node, std::size_t position) {
  unique_lock<LockableUTCPolicy> guard(*get(vertex_value, thetree, node));
  updateNodePosition(guard, node, position);
}
/// Expand a new node from given source node and update its position and color information with external guard
///@param unique_lock<LockableGameTree>& guard is a reader/writer's lock which will grant exclusive right for game tree access
///@param indexofsource is the index of source node from which a new node will be expanded
///@param move is the position on hex board for the new expanded node
///@param color is the color label for the new expanded node
///@return the index of the new expanded node
int LockableGameTree::expandNode(boost::unique_lock<LockableGameTree>& guard,
                                 int indexofsource, int move, char color) {
  //TODO redundant code
  vertex_t source = vertex(indexofsource, thetree);
  int indexofchild = -1;

  assert(get(vertex_value, thetree, source).get()->getCountforexpand() >= 0);
  assert(countforexpand.load() >= 0);

  while (countonwait.load() > 0)
    holdforselect.wait(guard);

  //adding the different colors from the parental node
  vertex_t target;
  if (get(vertex_color, thetree, source) == red_color)
    target = addNode(guard, static_cast<size_t>(move), 'B');
  else if (get(vertex_color, thetree, source) == black_color)
    target = addNode(guard, static_cast<size_t>(move), 'R');
  else
    target = addNode(guard, static_cast<size_t>(move), color);

  if (addEdge(source, target))
    indexofchild = get(vertex_index, thetree, target);

  if (get(vertex_value, thetree, source)->getNumofFutureChildren() > 0) {
    get(vertex_value, thetree, source)->setNumofFutureChildren(0, -1);
    if (get(vertex_value, thetree, source)->getNumofFutureChildren() == 0)
      isblockingforexpand = false;
    holdforexpand.notify_all();
  }
  assert(indexofchild > 0);
  //TODO
  setIsupdatedBackpropagation(guard, target);
  return indexofchild;
}
/// Expand a new node from given source node and update its position and color information with internal guard
///@param indexofsource is the index of source node from which a new node will be expanded
///@param move is the position on hex board for the new expanded node
///@param color is the color label for the new expanded node
///@return the index of the new expanded node
int LockableGameTree::expandNode(int indexofsource, int move, char color) {
  unique_lock<LockableGameTree> guard(*this);
  return expandNode(guard, indexofsource, move, color);
}
/// Add a new edge between source node and target node with internal lock
///@param source is the source node from which a new edge will be created.
///@param target is the target node to which a new edge will be created.
///@return A boolean variable indicates if adding edge is successful or not. True, the new edge is added successfully. False, not successfully.
bool LockableGameTree::addEdge(vertex_t source, vertex_t target) {
  //TODO duplicate code
  pair<edge_t, bool> result = add_edge(source, target, thetree);
  return result.second;
}
/// Select the node with the maximal UTC value
///@param unique_lock<LockableGameTree>& guard is a reader/writer's lock which will grant exclusive right for game tree access
///@param currentempty is the left empty position on the hex board in actual game state
///@param isbreaktie is the boolean variable which indicates if should break tie by random choice.
///@return a pair of integer and size_t. The first in pair is the index of selected node with maximal UTC value. The second is the level or depth at which the selected node locates.
pair<int,size_t> LockableGameTree::selectMaxBalanceNode(boost::unique_lock<LockableGameTree>& guard,
                                           int currentempty, bool isbreaktie) {
  vertex_t parent = _root;
  out_edge_iter viter, viterend;
  assert(getNumofChildren(parent) <= static_cast<unsigned>(currentempty));
  int level = 0;
#if __cplusplus > 199711L
  default_random_engine generator(
      static_cast<unsigned>(hexgame::chrono::system_clock::now()
          .time_since_epoch().count()));
  uniform_real_distribution<double> distribution(0.0, 1.0);
#else
  srand(static_cast<unsigned>(time(NULL)));
#endif
  while (isblockingforexpand)
    holdforexpand.wait(guard);

  //countonwait.fetch_add(1); //try to make two mutual exclusive critical regions in selection stage
  while ((getNumofChildren(parent)
      + get(vertex_value, thetree, parent)->getNumofFutureChildren()) != 0) {
    //test if the current examining node is fully expanded, if yes then return its child; otherwise, return the current node for expansion
    assert((currentempty - level) > 0);  //currentempty - level = 0 indicates the end of game
    if (static_cast<int>(getNumofChildren(parent)
        + get(vertex_value, thetree, parent).get()->getNumofFutureChildren())
        < (currentempty - level))
      break;

    if (get(vertex_value, thetree, parent).get()->getNumofFutureChildren() > 0
        && (static_cast<int>(getNumofChildren(parent)
            + get(vertex_value, thetree, parent)->getNumofFutureChildren())
            == (currentempty - level))) {
      get(vertex_value, thetree, parent).get()->addCountforexpand(1);
      countforexpand.fetch_add(1);
      isblockingforexpand.store(true);
      holdforselect.notify_all();
      while (get(vertex_value, thetree, parent).get()->getNumofFutureChildren()
          > 0)
        holdforexpand.wait(guard);
      get(vertex_value, thetree, parent).get()->subCountforexpand(1);
      countforexpand.fetch_sub(1);
    }

    countonwait.fetch_add(1);
    PriorityQueue<vertex_t, double> vertexchooser(getNumofChildren(parent));
    for (tie(viter, viterend) = out_edges(parent, thetree); viter != viterend;
        ++viter) {
      vertex_t node = target(*viter, thetree);
      assert(get(vertex_index, thetree, node) < num_vertices(thetree));
      //1) check point 1: if the visit count is zero, then isupdated = false;
#ifndef NDEBUG
      if (get(vertex_value, thetree, node).get()->feature(
          LockableUTCPolicy::visitcount) == 0)
        assert(!get(vertex_value, thetree, node).get()->getIsupdated());
#endif
      while (!get(vertex_value, thetree, node).get()->getIsupdated()) {
        holdforupdate.wait(guard);
      }
      assert(countonwait.load() >= 0);
      double value = -get(vertex_value, thetree, node).get()->calculate(
          *get(vertex_value, thetree, parent));
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
    assert(!vertexchooser.empty());
    parent = vertexchooser.minPrioirty();
    countonwait.fetch_sub(1);
    ++level;
  }

  if (countonwait.load() == 0) {  //immediately inform all the threads waiting for expansion to acquire the tree lock
    holdforselect.notify_all();
  }

  if ((currentempty - level) > 0)
    get(vertex_value, thetree, parent).get()->setNumofFutureChildren(0, 1);
  else
    //end of the game
    assert(get(vertex_value, thetree, parent).get()
           ->getNumofFutureChildren() == 0);
  return make_pair(get(vertex_index, thetree, parent), level);
}
/// Select the node with the maximal UTC value
///@param currentempty is the left empty position on the hex board in actual game state
///@param isbreaktie is the boolean variable which indicates if should break tie by random choice.
///@return a pair of integer and size_t. The first in pair is the index of selected node with maximal UTC value. The second is the level or depth at which the selected node locates.
pair<int, size_t> LockableGameTree::selectMaxBalanceNode(int currentempty, bool isbreaktie) {
  unique_lock<LockableGameTree> guard(*this);
  return selectMaxBalanceNode(guard, currentempty, isbreaktie);
}
/// Get the depth of a given node
///@param shared_lock<LockableGameTree>& is a reader's lock which will grant shared right for game tree retrieving access
///@param indexofnode is the index of node whose depth or level from root will be returned
///@return the depth or level from root
size_t LockableGameTree::getNodeDepth(boost::shared_lock<LockableGameTree>&,
                                      int indexofnode) {
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
/// Get the depth of a given node
///@param indexofnode is the index of node whose depth or level from root will be returned
///@return the depth or level from root
size_t LockableGameTree::getNodeDepth(int indexofnode) {
  shared_lock<LockableGameTree> guard;
  return getNodeDepth(guard, indexofnode);
}
/// Reconstruct simulated game history from game tree and return the past moves through collect moves from path starting from given node
///@param unique_lock<LockableGameTree>& is a reader/writer's lock which will grant exclusive right for game tree access
///@param indexofnode is the index of node which will serve as starting point from which the positions of all ancestral nodes will be collected to construct simulated game history
///@param babywatsons is the vector which stores the moves made by AI player in the simulated game history
///@param opponents is the vector which stores the moves made by virtual opponents in the simulated game history
///@param remainingmoves is an auxiliary data structure which will store the remaining moves after reconstructing simulated game history
///@return NONE
void LockableGameTree::getMovesfromTreeState(
    boost::unique_lock<LockableGameTree>& guard, int indexofnode,
    std::vector<int>& babywatsons, std::vector<int>& opponents,
    hexgame::unordered_set<int>& remainingmoves) {
  //TODO duplicate code
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
    vector<size_t> siblings = getSiblings(guard, chosenleaf);
#if __cplusplus > 199711L
    for (auto iter = siblings.begin(); iter != siblings.end();
        ++iter)
#else
        for (vector<size_t>::iterator iter = siblings.begin(); iter != siblings.end(); ++iter)
#endif
        {
      size_t pos = getNodePosition(*iter);
      if (remainingmoves.count(pos) != 0) {
        remainingmoves.erase(pos);
      }
    }
#if __cplusplus > 199711L
    default_random_engine generator(
        static_cast<unsigned>(hexgame::chrono::system_clock::now()
            .time_since_epoch().count()));
    uniform_int_distribution<int> distribution(0, remainingmoves.size() - 1);
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
  if (get(vertex_color, thetree, chosenleaf)
      != get(vertex_color, thetree, _root))
    babywatsons.push_back(get(vertex_position, thetree, chosenleaf));
  else
    opponents.push_back(get(vertex_position, thetree, chosenleaf));
}
/// Reconstruct simulated game history from game tree and return the past moves through collect moves from path starting from given node with internal lock
///@param indexofnode is the index of node which will serve as starting point from which the positions of all ancestral nodes will be collected to construct simulated game history
///@param babywatsons is the vector which stores the moves made by AI player in the simulated game history
///@param opponents is the vector which stores the moves made by virtual opponents in the simulated game history
///@param remainingmoves is an auxiliary data structure which will store the remaining moves after reconstructing simulated game history
///@return NONE
void LockableGameTree::getMovesfromTreeState(
    int indexofnode, std::vector<int>& babywatsons, std::vector<int>& opponents,
    hexgame::unordered_set<int>& remainingmoves) {
  unique_lock<LockableGameTree> guard(*this);
  return getMovesfromTreeState(guard, indexofnode, babywatsons, opponents,
                               remainingmoves);
}
/// Get the siblings of a given node with internal lock
///@param indexofnode is the index of node whose indices of siblings will be returned
///@return a vector of size_t which stores the indices of siblings of the given node
vector<size_t> LockableGameTree::getSiblings(std::size_t indexofnode) {
  boost::unique_lock<LockableGameTree> guard(*this);
  return getSiblings(guard, indexofnode);
}
/// Get the siblings of a given node with extenal lock
///@param unique_lock<LockableGameTree>& is a reader/writer's lock which will grant exclusive right for game tree access
///@param indexofnode is the index of node whose indices of siblings will be returned
///@return a vector of size_t which stores the indices of siblings of the given node
vector<size_t> LockableGameTree::getSiblings(boost::unique_lock<LockableGameTree>&,
                                             std::size_t indexofnode) {
  //TODO duplicate
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
/// Set the position of node with internal lock
///@param indexofnode is the index of node whose position will be updated
///@param position is the new position on hex board for the given node
///@return NONE
void LockableGameTree::setNodePosition(std::size_t indexofnode, std::size_t position) {
  vertex_t node = vertex(indexofnode, thetree);
  unique_lock<LockableUTCPolicy> guard(*get(vertex_value, thetree, node));
  setNodePosition(guard, node, position);
}
/// Set the position of node with internal lock
///@param unique_lock<LockableUTCPolicy>& guard is a reader/writer's lock which will grant exclusive right for game tree access
///@param indexofnode is the index of node whose position will be updated
///@param position is the new position on hex board for the given node
///@return NONE
void LockableGameTree::setNodePosition(boost::unique_lock<LockableUTCPolicy>& guard,
                                       vertex_t node, std::size_t position){
  updateNodePosition(guard, node, position);
  updateNodeName(guard, node);
}
/// Get node position of a given index of node with internal lock
///@param indexofnode is the index of the node in query
///@return return the position on hex board
size_t LockableGameTree::getNodePosition(std::size_t indexofnode) {
  vertex_t node = vertex(indexofnode, thetree);
  unique_lock<LockableUTCPolicy> guard(*get(vertex_value, thetree, node));
  return getNodePosition(guard, node);
}
/// Get node position of a given index of node with internal lock
///@param indexofnode is the index of the node in query
///@return return the position on hex board
std::size_t LockableGameTree::getNodePosition(boost::unique_lock<LockableUTCPolicy>&, vertex_t node){
  return get(vertex_position, thetree, node);
}
/// Update node from play-out simulation result with external lock
///@param unique_lock<LockableGameTree>& guard is a reader/writer's lock which will grant exclusive right for game tree access
///@param indexofnode is the index of node whose ancestral nodes will be updated during backpropagation phase. See backpropagate(vertex_t,int,int)
///@return NONE
void LockableGameTree::updateNodefromSimulation(
    boost::unique_lock<LockableGameTree>& guard, int indexofnode, int winner,
    int level) {
  //TODO duplicate code
  vertex_t node = vertex(indexofnode, thetree);
  int value = 0;
  if (winner > 0) {
    value = get(vertex_value, thetree, node).get()->feature(
        AbstractUTCPolicy::wincount);
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
        AbstractUTCPolicy_visitcount, 0, 1, AbstractUTCPolicy_wincount, 0, 0);
  backpropagate(guard, indexofnode, value, level);
}
/// Update node from play-out simulation result with internal lock
///@param indexofnode is the index of node whose ancestral nodes will be updated during backpropagation phase. See backpropagate(vertex_t,int,int)
///@return NONE
void LockableGameTree::updateNodefromSimulation(int indexofnode, int winner,
                                               int level) {
  unique_lock<LockableGameTree> guard(*this);
  updateNodefromSimulation(guard, indexofnode, winner, level);
}
/// Back propagate the play-out phase result till to the level specified with external lock
///@param unique_lock<LockableGameTree>& guard is a reader/writer's lock which will grant exclusive right for game tree access
///@param leaf is the leaf node or starting node from which a back propagation will be executed
///@param value is the value used for update via back propagation
///@param level is the depth from root till which a back propagation will be executed
///@return NONE
void LockableGameTree::backpropagate(boost::unique_lock<LockableGameTree>&,
                                     vertex_t leaf, int value, int level) {
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
          AbstractUTCPolicy_visitcount, 0, 1, AbstractUTCPolicy_wincount, 0,
          curvalue);
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
  //assert(notifyAllUpdateDone(guard, leaf, level));
  holdforupdate.notify_all();
}
/// Back propagate the play-out phase result till to the level specified with internal lock
///@param leaf is the leaf node or starting node from which a back propagation will be executed
///@param value is the value used for update via back propagation
///@param level is the depth from root till which a back propagation will be executed
///@return NONE
void LockableGameTree::backpropagate(vertex_t leaf, int value, int level) {
  unique_lock<LockableGameTree> guard(*this);
  return backpropagate(guard, leaf, value, level);
}
/// Get the number of children of a given node with internal lock
///@param indexofnode is the index of the node whose number of children will be returned
///@return the number of children nodes
size_t LockableGameTree::getNumofChildren(std::size_t indexofnode) {
  vertex_t parent = vertex(indexofnode, thetree);
  shared_lock<LockableUTCPolicy> guard(*get(vertex_value, thetree, parent));
  return getNumofChildren(guard, parent);
}
/// Get the number of children of a given node with external lock
///@param shared_lock<LockableGameTree>& is a reader's lock which will grant shared right for game tree retrieving access
///@param indexofnode is the index of the node whose number of children will be returned
///@return the number of children nodes
size_t LockableGameTree::getNumofChildren(boost::shared_lock<LockableUTCPolicy>&,
                                          vertex_t parent) {
  return out_degree(parent, thetree);
}
/// Get parent of a given node with external lock
///@param unique_lock<LockableUTCPolicy>& is a reader/writer's lock which will grant exclusive right for game tree access
///@param node is the node whose parent will be returned
///@return the parental node
AbstractGameTree::vertex_t LockableGameTree::getParent(boost::unique_lock<LockableUTCPolicy>&, vertex_t node) {
  in_edge_iter viter, viterend;
  assert(in_degree(node, thetree) == 1);
  vertex_t parent = graph_traits<basegraph>::null_vertex();
  for (tie(viter, viterend) = in_edges(node, thetree); viter != viterend;
      ++viter)
    parent = source(*viter, thetree);
  return parent;
}
/// Notify the waiting threads that all required updates completes in back-propagation phase
///@param leaf is leaf node from which notification will be executed via back-propagation
///@return boolean variable indicates if all notification is completed or not
bool LockableGameTree::notifyAllUpdateDone(vertex_t leaf, int level) {
  unique_lock<LockableGameTree> guard(*this);
  return notifyAllUpdateDone(guard, leaf, level);
}
/// Get parent of a given node with internal lock
///@param node is the node whose parent will be returned
///@return the parental node
std::size_t LockableGameTree::getParent(std::size_t indexofchild) {
  vertex_t node = vertex(indexofchild, thetree);
  unique_lock<LockableUTCPolicy> guard(*get(vertex_value, thetree, node));
  vertex_t parent = getParent(guard, node);
  return get(vertex_index, thetree, parent);
}
/// Get the flag of the given node which indicates if this node has been updated or not
///@param indexofnode is the node whose update status will be returned
///@return the boolean variable which indicates if this node has been updated or not
bool LockableGameTree::getIsupdated(int indexofnode) {
  vertex_t node = vertex(indexofnode, thetree);
  shared_lock<LockableUTCPolicy> guard(*get(vertex_value, thetree, node));
  return (get(vertex_value, thetree, node).get()->getIsupdated());
}
/// Take "and" result of all the LockableUTCPolicy#isupdated status along the back-propagation path with external lock
///@param shared_lock<LockableGameTree>& is a reader's lock which will grant shared right for game tree retrieving access
///@param indexofleaf is the index of leaf from which the LockableUTCPolicy#isupdated is acuiqred via back-propagation will be executed
///@return the boolean variable which indicates if this node along with back propagation path has been updated or not
bool LockableGameTree::getIsupdatedBackpropagation(
    boost::shared_lock<LockableGameTree>&, int indexofleaf) {
  vertex_t node = vertex(indexofleaf, thetree), parent =
      graph_traits<basegraph>::null_vertex();
  bool result = get(vertex_value, thetree, node).get()->getIsupdated();
  while (node != _root) {
    //check in-edges
    in_edge_iter viter, viterend;
    assert(in_degree(node, thetree) == 1);
    for (tie(viter, viterend) = in_edges(node, thetree); viter != viterend;
        ++viter) {
      parent = source(*viter, thetree);
      result &= get(vertex_value, thetree, parent).get()->getIsupdated();
    }
    node = parent;
  }
  return result;
}
/// Take "and" result of all the LockableUTCPolicy#isupdated status along the back-propagation path with internal lock
///@param indexofleaf is the index of leaf from which the LockableUTCPolicy#isupdated is acquired via back-propagation will be executed
///@return the boolean variable which indicates if this node along with back propagation path has been updated or not
bool LockableGameTree::getIsupdatedBackpropagation(int indexofleaf) {
  shared_lock<LockableGameTree> guard(*this);
  return getIsupdatedBackpropagation(guard, indexofleaf);
}
/// Get the leaf nodes from the game tree
///@param shared_lock<LockableGameTree>&
///@return the leaf nodes of the game tree
vector<size_t> LockableGameTree::getLeaves(
    boost::shared_lock<LockableGameTree>&) {
  vertex_t node = graph_traits<basegraph>::null_vertex();
  vector<size_t> leaves;
  stack<vertex_t> nodeholder;
  nodeholder.push(_root);
  while (!nodeholder.empty()) {
    node = nodeholder.top();
    nodeholder.pop();
    if (out_degree(node, thetree) == 0)
      leaves.push_back(get(vertex_index, thetree, node));
    out_edge_iter viter, viterend;
    for (tie(viter, viterend) = out_edges(node, thetree); viter != viterend;
        ++viter)
      nodeholder.push(target(*viter, thetree));
  }
  return (vector<size_t>(leaves));
}
/// Get the leaf nodes from the game tree
///@param NONE
///@return the leaf nodes of the game tree
vector<size_t> LockableGameTree::getLeaves() {
  boost::shared_lock<LockableGameTree> guard(*this);
  return (getLeaves(guard));
}
/// Set the LockableUTCPolicy#isupdated status along the back-propagation path with internal lock
///@param indexofleaf is the index of leaf from which the LockableUTCPolicy#isupdated is set via back-propagation will be executed
///@return the boolean variable which indicates if this node along with back propagation path has been updated or not
void LockableGameTree::setIsupdatedBackpropagation(
    boost::unique_lock<LockableGameTree>&, vertex_t indexofleaf) {
  //TODO duplicate code
  vertex_t node = vertex(indexofleaf, thetree), parent =
      graph_traits<basegraph>::null_vertex();
  get(vertex_value, thetree, node).get()->setIsupdated(false);
  while (node != _root) {
    //check in-edges
    in_edge_iter viter, viterend;
    assert(in_degree(node, thetree) == 1);
    for (tie(viter, viterend) = in_edges(node, thetree); viter != viterend;
        ++viter) {
      parent = source(*viter, thetree);
      get(vertex_value, thetree, parent).get()->setIsupdated(false);
    }
    node = parent;
  }
}
/// Get the best move from many play-out simulations via maximizing winning rate with internal node
///@param NONE
///@return a pair of integer and double. The first is the position on the hex board; while the second is the maximal UTC value calculated.
pair<int, double> LockableGameTree::getBestMovefromSimulation() {
  //TODO duplicate code
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
/// Clear all nodes and edges of a game tree with internal lock
///@param NONE
///@return NONE
void LockableGameTree::clearAll() {
  unique_lock<LockableGameTree> guard(*this);
  return clearAll(guard);
}
/// Clear all nodes and edges of a game tree with external lock
///@param unique_lock<LockableGameTree>& is a reader/writer's lock which will grant exclusive right for game tree access
///@return NONE
void LockableGameTree::clearAll(boost::unique_lock<LockableGameTree>&) {
  //TODO duplicate code
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
/// Get UTC value according to the specified feature
///@param indexofnode is the index of node whose UTC value of specified feature will be returned
///@param indexofkind is one of valuekind which will specify what value of features should be returned. Either wincount or visitcount
///@return the return UTC value
int LockableGameTree::getNodeValueFeature(
    int indexofnode, AbstractUTCPolicy::valuekind indexofkind) {
  //TODO duplicate code
  vertex_t node = vertex(indexofnode, thetree);
  unique_lock<LockableUTCPolicy> guard(*get(vertex_value, thetree, node));
  return get(vertex_value, thetree, node).get()->feature(indexofkind);
}
/// Get the total size of nodes
///@param shared_lock<LockableGameTree>& is a reader's lock which will grant shared right for game tree access
///@return the total size of nodes
size_t LockableGameTree::getSizeofNodes(boost::shared_lock<LockableGameTree>&) {
  return num_vertices(thetree);
}
/// Get the total size of nodes
///@param NONE
///@return the total size of nodes
size_t LockableGameTree::getSizeofNodes() {
  shared_lock<LockableGameTree> guard(*this);
  return num_vertices(thetree);
}
/// Get the total size of edges
///@param NONE
///@return the total size of edges
size_t LockableGameTree::getSizeofEdges() {
  shared_lock<LockableGameTree> guard(*this);
  return getSizeofEdges(guard);
}
/// Get the total size of edges
///@param shared_lock<LockableGameTree>& is a reader's lock which will grant shared right for game tree access
///@return the total size of edges
size_t LockableGameTree::getSizeofEdges(boost::shared_lock<LockableGameTree>&) {
  size_t numofedges = num_edges(thetree);
  assert(numofedges == num_vertices(thetree) - 1);
  return numofedges;
}

