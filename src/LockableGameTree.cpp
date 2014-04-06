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

bool LockableUTCPolicy::waitforupdate() {
  boost::unique_lock<LockableUTCPolicy> guard(*this);
  holdforupdate.wait(guard);
  return true;
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
void LockableUTCPolicy::notifyupdatedone() {
  holdforupdate.notify_all();
}
#if __cplusplus > 199711L
LockableGameTree::LockableGameTree()
    : LockableGameTree('W', 0) {
}
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
LockableGameTree::LockableGameTree(char playerscolor, size_t indexofroot)
    : lockable_share_type() {
  initGameTree(playerscolor, indexofroot);
}
bool LockableGameTree::notifyAllUpdateDone(unique_lock<LockableGameTree>&,
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
void LockableGameTree::initGameTree(char playerscolor, size_t indexofroot) {
  //set the root's color label as opponent's color label
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
LockableGameTree::vertex_t LockableGameTree::addNode(
    unique_lock<LockableGameTree>&, size_t positionofchild,
    char color) {
  vertex_t target = add_vertex(thetree);
  //TODO re-factor, updateNodeName must be created at the last
  updateNodeValue(target);
  updateNodePosition(target, positionofchild);
  updateNodeColor(target, color);
  updateNodeName(target);
  return target;
}
LockableGameTree::vertex_t LockableGameTree::addNode(size_t positionofchild,
                                                     char color) {
  unique_lock<LockableGameTree> guard(*this);
  return addNode(guard, positionofchild, color);
}
string LockableGameTree::printGameTree(shared_lock<LockableGameTree>&,
                                       int index) {
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
string LockableGameTree::printGameTree(int index) {
  shared_lock<LockableGameTree> guard(*this);
  return printGameTree(guard, index);
}
//external lock version
void LockableGameTree::updateNodeName(unique_lock<LockableUTCPolicy>&,
                                      vertex_t node) {
 updateNodeName(node);
}
void LockableGameTree::updateNodeName(vertex_t node) {
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
//update the value of a given node
void LockableGameTree::updateNodeValue(vertex_t node) {
  put(vertex_value, thetree, node,
      hexgame::shared_ptr<LockableUTCPolicy>(new LockableUTCPolicy()));
}
//update the color of a given node
void LockableGameTree::updateNodeColor(unique_lock<LockableUTCPolicy>&,
                                       vertex_t node, char color) {
  updateNodeColor(node, color);
}
//update the color of a given node
void LockableGameTree::updateNodeColor(vertex_t node, char color) {
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
//update the board position of a given node
void LockableGameTree::updateNodePosition(unique_lock<LockableUTCPolicy>&,
                                          vertex_t node, std::size_t position) {
  updateNodePosition(node, position);
}
void LockableGameTree::updateNodePosition(vertex_t node, std::size_t position) {
  put(vertex_position, thetree, node, position);
}
int LockableGameTree::expandNode(unique_lock<LockableGameTree>& guard,
                                 int indexofsource, int move, char color) {
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
  setIsupdatedBackpropagation(guard, target);
  return indexofchild;
}
int LockableGameTree::expandNode(int indexofsource, int move, char color) {
  unique_lock<LockableGameTree> guard(*this);
  return expandNode(guard, indexofsource, move, color);
}
bool LockableGameTree::addEdge(vertex_t source, vertex_t target) {
  pair<edge_t, bool> result = add_edge(source, target, thetree);
  return result.second;
}
pair<int,int> LockableGameTree::selectMaxBalanceNode(unique_lock<LockableGameTree>& guard,
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
pair<int,int> LockableGameTree::selectMaxBalanceNode(int currentempty, bool isbreaktie) {
  unique_lock<LockableGameTree> guard(*this);
  return selectMaxBalanceNode(guard, currentempty, isbreaktie);
}
size_t LockableGameTree::getNodeDepth(shared_lock<LockableGameTree>&,
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
size_t LockableGameTree::getNodeDepth(int indexofnode) {
  shared_lock<LockableGameTree> guard;
  return getNodeDepth(guard, indexofnode);
}

void LockableGameTree::getMovesfromTreeState(
    unique_lock<LockableGameTree>& guard, int indexofnode,
    vector<int>& babywatsons, vector<int>& opponents,
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
void LockableGameTree::getMovesfromTreeState(
    int indexofnode, std::vector<int>& babywatsons, std::vector<int>& opponents,
    hexgame::unordered_set<int>& remainingmoves) {
  unique_lock<LockableGameTree> guard(*this);
  return getMovesfromTreeState(guard, indexofnode, babywatsons, opponents,
                               remainingmoves);
}
vector<size_t> LockableGameTree::getSiblings(size_t indexofnode) {
  unique_lock<LockableGameTree> guard(*this);
  return getSiblings(guard, indexofnode);
}
vector<size_t> LockableGameTree::getSiblings(unique_lock<LockableGameTree>&,
                                             size_t indexofnode) {
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
void LockableGameTree::setNodePosition(size_t indexofnode, size_t position) {
  vertex_t node = vertex(indexofnode, thetree);
  unique_lock<LockableUTCPolicy> guard(*get(vertex_value, thetree, node));
  updateNodePosition(guard, node, position);
  updateNodeName(guard, node);
}
size_t LockableGameTree::getNodePosition(std::size_t indexofnode) {
  vertex_t node = vertex(indexofnode, thetree);
  unique_lock<LockableUTCPolicy> guard(*get(vertex_value, thetree, node));
  return get(vertex_position, thetree, node);
}
void LockableGameTree::updateNodefromSimulation(
    unique_lock<LockableGameTree>& guard, int indexofnode, int winner,
    int level) {
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
void LockableGameTree::updateNodefromSimulation(int indexofnode, int winner,
                                               int level) {
  unique_lock<LockableGameTree> guard(*this);
  updateNodefromSimulation(guard, indexofnode, winner, level);
}
void LockableGameTree::backpropagate(unique_lock<LockableGameTree>&,
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
void LockableGameTree::backpropagate(vertex_t leaf, int value, int level) {
  unique_lock<LockableGameTree> guard(*this);
  return backpropagate(guard, leaf, value, level);
}
size_t LockableGameTree::getNumofChildren(size_t indexofnode) {
  vertex_t parent = vertex(indexofnode, thetree);
  shared_lock<LockableUTCPolicy> guard(*get(vertex_value, thetree, parent));
  return getNumofChildren(guard, parent);
}
size_t LockableGameTree::getNumofChildren(shared_lock<LockableUTCPolicy>&,
                                          vertex_t parent) {
  return out_degree(parent, thetree);
}
AbstractGameTree::vertex_t LockableGameTree::getParent(unique_lock<LockableUTCPolicy>&, vertex_t node) {
  in_edge_iter viter, viterend;
  assert(in_degree(node, thetree) == 1);
  vertex_t parent = graph_traits<basegraph>::null_vertex();
  for (tie(viter, viterend) = in_edges(node, thetree); viter != viterend;
      ++viter)
    parent = source(*viter, thetree);
  return parent;
}
bool LockableGameTree::notifyAllUpdateDone(vertex_t leaf, int level) {
  unique_lock<LockableGameTree> guard(*this);
  return notifyAllUpdateDone(guard, leaf, level);
}
std::size_t LockableGameTree::getParent(std::size_t indexofchild) {
  vertex_t node = vertex(indexofchild, thetree);
  unique_lock<LockableUTCPolicy> guard(*get(vertex_value, thetree, node));
  vertex_t parent = getParent(guard, node);
  return get(vertex_index, thetree, parent);

}
bool LockableGameTree::getIsupdated(int indexofnode) {
  vertex_t node = vertex(indexofnode, thetree);
  shared_lock<LockableUTCPolicy> guard(*get(vertex_value, thetree, node));
  return (get(vertex_value, thetree, node).get()->getIsupdated());
}
bool LockableGameTree::getIsupdatedBackpropagation(
    shared_lock<LockableGameTree>&, int indexofleaf) {
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
bool LockableGameTree::getIsupdatedBackpropagation(int indexofleaf) {
  shared_lock<LockableGameTree> guard(*this);
  return getIsupdatedBackpropagation(guard, indexofleaf);
}
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
vector<size_t> LockableGameTree::getLeaves() {
  shared_lock<LockableGameTree> guard(*this);
  return (getLeaves(guard));
}
void LockableGameTree::setIsupdatedBackpropagation(
    boost::unique_lock<LockableGameTree>&, vertex_t indexofleaf) {
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
pair<int, double> LockableGameTree::getBestMovefromSimulation() {
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
void LockableGameTree::clearAll() {
  unique_lock<LockableGameTree> guard(*this);
  return clearAll(guard);
}
void LockableGameTree::clearAll(unique_lock<LockableGameTree>&) {
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
int LockableGameTree::getNodeValueFeature(
    int indexofnode, AbstractUTCPolicy::valuekind indexofkind) {
  vertex_t node = vertex(indexofnode, thetree);
  unique_lock<LockableUTCPolicy> guard(*get(vertex_value, thetree, node));
  return get(vertex_value, thetree, node).get()->feature(indexofkind);
}
size_t LockableGameTree::getSizeofNodes(shared_lock<LockableGameTree>&) {
  return num_vertices(thetree);
}
size_t LockableGameTree::getSizeofNodes() {
  shared_lock<LockableGameTree> guard(*this);
  return num_vertices(thetree);
}
size_t LockableGameTree::getSizeofEdges() {
  shared_lock<LockableGameTree> guard(*this);
  return getSizeofEdges(guard);
}
size_t LockableGameTree::getSizeofEdges(shared_lock<LockableGameTree>&) {
  size_t numofedges = num_edges(thetree);
  assert(numofedges == num_vertices(thetree) - 1);
  return numofedges;
}

