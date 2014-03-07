/*
 * LockableGameTree.cpp
 *
 *  Created on: Feb 11, 2014
 *      Author: renewang
 */

#include <ctime>
#include <deque>
#include <utility>
#include <iostream>

#include <boost/thread/mutex.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include "Game.h"
#include "Player.h"
#include "PriorityQueue.h"
#include "LockableGameTree.h"
#include "MultiMonteCarloTreeSearch.h"

using namespace std;
using namespace boost;

#ifndef NDEBUG
recursive_mutex mutex_;
#endif

bool LockableUTCPolicy::waitforupdate(
    boost::unique_lock<LockableUTCPolicy>& guard) {
  holdforupdate.wait(guard);
  return true;
}
double LockableUTCPolicy::estimate() {
  assert(featureholder.at(visitcount) > 0);
  value = static_cast<double>(featureholder.at(wincount))
      / static_cast<double>(featureholder.at(visitcount));
  return value;
}
double LockableUTCPolicy::calculate(LockableUTCPolicy& parent) {
  boost::unique_lock<LockableUTCPolicy> lock(*this);
  //choose the maximal value for non-uct implementation (greedy mc) if isgreedy = true
  double value = estimate();
  //calculate UCT value (Upper Confidence Bound applied to Tree)
  //equation is used from Chaslot G et al,
  double vcount = static_cast<double>(featureholder.at(visitcount));
  double vcountofparent = static_cast<double>(parent.feature(visitcount));
  assert(vcountofparent >= vcount);
  balance = (value
      + (std::sqrt(coefficient * std::log(vcountofparent) / vcount)));
  return balance;
}
bool LockableUTCPolicy::update(valuekind indexofkind, int value,
                               int increment) {
  if (value == 0)  //simply incrementing original value
    featureholder.at(indexofkind) = featureholder.at(indexofkind) + increment;
  else
    featureholder.at(indexofkind) = value;
  return true;
}
bool LockableUTCPolicy::updateAll(valuekind visitkind, int valueofvisit,
                                  int increaseofvisit, valuekind winkind,
                                  int valueofwin, int increaseofwin) {
  boost::unique_lock<LockableUTCPolicy> lock(*this);
  update(winkind, valueofwin, increaseofwin);
  update(visitkind, valueofvisit, increaseofvisit);
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
: lockable_base_type() {
  initGameTree('W', 0);
}
LockableGameTree::LockableGameTree(char playerscolor)
: lockable_base_type() {
  initGameTree(playerscolor, 0);
}
#endif
LockableGameTree::LockableGameTree(char playerscolor, size_t indexofroot)
    : lockable_base_type() {
  initGameTree(playerscolor, indexofroot);
}
bool LockableGameTree::notifyAllUpdateDone(unique_lock<LockableGameTree>&,
                                           vertex_t leaf, int level) {
#ifndef NDEBUG
  {
    strict_lock<recursive_mutex> lock(mutex_);
    clog << DEBUGHEADER() << boost::this_thread::get_id()
         << " acquires the lock " << this << endl;
  }
#endif
  int curlevel = level;
  vertex_t node = leaf, parent = graph_traits<basegraph>::null_vertex();
  get(vertex_value, thetree, node).get()->notifyupdatedone();
  while (node != _root) {
#ifndef NDEBUG
    {
      strict_lock<recursive_mutex> lock(mutex_);
      clog << DEBUGHEADER() << boost::this_thread::get_id()
           << " completes update " << this << " the node "
           << get(vertex_index, thetree, node) << " "
           << &get(vertex_value, thetree, parent) << endl;
    }
#endif
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
#ifndef NDEBUG
  {
    strict_lock<recursive_mutex> lock(mutex_);
    clog << DEBUGHEADER() << boost::this_thread::get_id()
         << " releases the lock " << this << " " << countonwait.load()
         << " threads on wait" << endl;
  }
#endif
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
    unique_lock<LockableGameTree>& guard, size_t positionofchild, char color) {
  vertex_t target = add_vertex(thetree);
  //TODO re-factor
  updateNodePosition(guard, target, positionofchild);
  updateNodeColor(guard, target, color);
  updateNodeValue(guard, target);
  updateNodeName(guard, target);
  return target;
}
LockableGameTree::vertex_t LockableGameTree::addNode(size_t positionofchild,
                                                     char color) {
  unique_lock<LockableGameTree> guard(*this);
  return addNode(guard, positionofchild, color);
}
//update color
void LockableGameTree::updateNodeColor(vertex_t node, char color) {
  unique_lock<LockableGameTree> guard(*this);
  updateNodeColor(guard, node, color);
}
//update position
void LockableGameTree::updateNodePosition(vertex_t node, size_t position) {
  unique_lock<LockableGameTree> guard(*this);
  updateNodePosition(guard, node, position);
}
//update value
void LockableGameTree::updateNodeValue(vertex_t node) {
  unique_lock<LockableGameTree> guard(*this);
  updateNodeValue(guard, node);
}
//update name
void LockableGameTree::updateNodeName(vertex_t node) {
  unique_lock<LockableGameTree> guard(*this);
  updateNodeName(guard, node);
}
string LockableGameTree::printGameTree(unique_lock<LockableGameTree>&,
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
  unique_lock<LockableGameTree> guard(*this);
  return printGameTree(guard, index);
}
//external lock version
void LockableGameTree::updateNodeName(unique_lock<LockableGameTree>&,
                                      vertex_t node) {
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
void LockableGameTree::updateNodeValue(unique_lock<LockableGameTree>&,
                                       vertex_t node) {
  put(vertex_value, thetree, node,
      hexgame::shared_ptr<LockableUTCPolicy>(new LockableUTCPolicy()));
}
//update the color of a given node
void LockableGameTree::updateNodeColor(unique_lock<LockableGameTree>&,
                                       vertex_t node, char color) {
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
void LockableGameTree::updateNodePosition(unique_lock<LockableGameTree>&,
                                          vertex_t node, std::size_t position) {
  put(vertex_position, thetree, node, position);
}
int LockableGameTree::expandNode(unique_lock<LockableGameTree>& guard,
                                 int indexofsource, int move, char color) {

  vertex_t source = vertex(indexofsource, thetree);
  int indexofchild = -1;
#ifndef NDEBUG
  {
    strict_lock<recursive_mutex> lock(mutex_);
    clog << DEBUGHEADER() << this_thread::get_id() << " acquires the lock "
         << this << " expands on node " << indexofsource << " while waiting "
         << countonwait.load() << " to finish" << " where countforexpand "
         << get(vertex_value, thetree, source).get()->getCountforexpand()
         << endl;
  }
#endif

  assert(get(vertex_value, thetree, source).get()->getCountforexpand() >= 0);
  assert(countforexpand.load() >= 0);

  while (countonwait.load() > 0)
    holdforselect.wait(guard);

#ifndef NDEBUG
  {
    strict_lock<recursive_mutex> lock(mutex_);
    clog << DEBUGHEADER() << this_thread::get_id() << " re-acquires the lock "
         << this << " expands on node " << indexofsource << " while waiting "
         << countonwait.load() << " to finish" << endl;
  }
#endif
  //adding the different colors from the parental node
  vertex_t target;
  if (get(vertex_color, thetree, source) == red_color)
    target = addNode(guard, static_cast<size_t>(move), 'B');
  else if (get(vertex_color, thetree, source) == black_color)
    target = addNode(guard, static_cast<size_t>(move), 'R');
  else
    target = addNode(guard, static_cast<size_t>(move), color);

  if (addEdge(guard, source, target))
    indexofchild = get(vertex_index, thetree, target);

  if (get(vertex_value, thetree, source)->getNumofFutureChildren() > 0) {
#ifndef NDEBUG
    {
      strict_lock<recursive_mutex> lock(mutex_);
      clog << DEBUGHEADER() << this_thread::get_id() << " notifies " << this
           << " to unblock threads waiting " << indexofsource
           << " for selection " << endl;
    }
#endif
    get(vertex_value, thetree, source)->setNumofFutureChildren(0, -1);
    if (get(vertex_value, thetree, source)->getNumofFutureChildren() == 0)
      isblockingforexpand = false;
    holdforexpand.notify_all();
  }
  assert(indexofchild > 0);
#ifndef NDEBUG
  {
    strict_lock<recursive_mutex> lock(mutex_);
    clog << DEBUGHEADER() << this_thread::get_id() << " releases the lock "
         << this << " expands a new node " << indexofchild << endl;
  }
#endif
  return indexofchild;
}
int LockableGameTree::expandNode(int indexofsource, int move, char color) {
  unique_lock<LockableGameTree> guard(*this);
  return expandNode(guard, indexofsource, move, color);
}
bool LockableGameTree::addEdge(unique_lock<LockableGameTree>&, vertex_t source,
                               vertex_t target) {
  pair<edge_t, bool> result = add_edge(source, target, thetree);
  return result.second;
}
bool LockableGameTree::addEdge(vertex_t source, vertex_t target) {
  unique_lock<LockableGameTree> guard(*this);
  return addEdge(guard, source, target);
}
int LockableGameTree::selectMaxBalanceNode(unique_lock<LockableGameTree>& guard,
                                           int currentempty, bool isbreaktie) {
#ifndef NDEBUG
  {
    strict_lock<recursive_mutex> lock(mutex_);
    clog << DEBUGHEADER() << this_thread::get_id() << " acquires the lock "
         << this << endl;
  }
#endif
  vertex_t parent = _root;
  out_edge_iter viter, viterend;
  assert(
      getNumofChildren(guard, get(vertex_index, thetree, parent))
          <= static_cast<unsigned>(currentempty));
  int level = 0;
#if __cplusplus > 199711L
  default_random_engine generator(
      static_cast<unsigned>(hexgame::chrono::system_clock::now()
          .time_since_epoch().count()));
  uniform_real_distribution<double> distribution(0.0, 1.0);
#else
  srand(static_cast<unsigned>(clock()));
#endif
  while (isblockingforexpand)
    holdforexpand.wait(guard);

  //countonwait.fetch_add(1); //try to make two mutual exclusive critical regions in selection stage
  while ((getNumofChildren(guard, get(vertex_index, thetree, parent))
      + get(vertex_value, thetree, parent)->getNumofFutureChildren()) != 0) {
    //test if the current examining node is fully expanded, if yes then return its child; otherwise, return the current node for expansion
    assert((currentempty - level) > 0);  //currentempty - level = 0 indicates the end of game
#ifndef NDEBUG
    {
      strict_lock<recursive_mutex> lock(mutex_);
      clog << DEBUGHEADER() << boost::this_thread::get_id() << " " << this
           << " node " << parent << " already has "
           << getNumofChildren(guard, get(vertex_index, thetree, parent))
           << " will have "
           << get(vertex_value, thetree, parent).get()->getNumofFutureChildren()
           << endl;
    }
#endif
    if (static_cast<int>(getNumofChildren(guard,
                                          get(vertex_index, thetree, parent))
        + get(vertex_value, thetree, parent).get()->getNumofFutureChildren())
        < (currentempty - level))
      break;

    if (get(vertex_value, thetree, parent).get()->getNumofFutureChildren() > 0
        && (static_cast<int>(getNumofChildren(
            guard, get(vertex_index, thetree, parent))
            + get(vertex_value, thetree, parent)->getNumofFutureChildren())
            == (currentempty - level))) {
      get(vertex_value, thetree, parent).get()->addCountforexpand(1);
      countforexpand.fetch_add(1);
      isblockingforexpand.store(true);
#ifndef NDEBUG
      {
        strict_lock<recursive_mutex> lock(mutex_);
        clog
            << DEBUGHEADER()
            << boost::this_thread::get_id()
            << " will verify "
            << this
            << " node "
            << parent
            << " "
            << countonwait.load()
            << " thread(s) enter(s) selection "
            << countforexpand.load()
            << " thread(s) enter(s) this region and this node contributes "
            << get(vertex_value, thetree, parent).get()->getCountforexpand()
            << " with "
            << get(vertex_value, thetree, parent).get()->getNumofFutureChildren()
            << " waiting to be expanded" << endl;
      }
#endif
      holdforselect.notify_all();
      while (get(vertex_value, thetree, parent).get()->getNumofFutureChildren()
          > 0)
        holdforexpand.wait(guard);
      get(vertex_value, thetree, parent).get()->subCountforexpand(1);
      countforexpand.fetch_sub(1);
#ifndef NDEBUG
      {
        strict_lock<recursive_mutex> lock(mutex_);
        clog
            << DEBUGHEADER()
            << boost::this_thread::get_id()
            << " will verify "
            << this
            << " node "
            << parent
            << " "
            << countonwait.load()
            << " thread(s) enter(s) selection "
            << countforexpand.load()
            << " thread(s) enter(s) this region and this node contributes "
            << get(vertex_value, thetree, parent).get()->getCountforexpand()
            << " with "
            << get(vertex_value, thetree, parent).get()->getNumofFutureChildren()
            << " waiting to be expanded" << endl;
      }
#endif
    }

    countonwait.fetch_add(1);
    PriorityQueue<vertex_t, double> vertexchooser(
        getNumofChildren(guard, get(vertex_index, thetree, parent)));
    for (tie(viter, viterend) = out_edges(parent, thetree); viter != viterend;
        ++viter) {
      vertex_t node = target(*viter, thetree);
#ifndef NDEBUG
      {
        strict_lock<recursive_mutex> lock(mutex_);
        clog
            << DEBUGHEADER()
            << boost::this_thread::get_id()
            << " record "
            << this
            << " node "
            << node
            << " "
            << countonwait.load()
            << " thread(s) enter(s) selection "
            << countforexpand.load()
            << " and this node contributes "
            << get(vertex_value, thetree, node).get()->getCountforexpand()
            << " thread(s) enter(s) the region waiting for expansion with "
            << get(vertex_value, thetree, parent).get()->getNumofFutureChildren()
            << " waiting to be expanded" << endl;
      }
#endif
      assert(get(vertex_index, thetree, node) < num_vertices(thetree));
      while (get(vertex_value, thetree, node).get()->feature(
          LockableUTCPolicy::visitcount) == 0)  //TODO change criterion
        holdforupdate.wait(guard);
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

  int countofchildren = get(vertex_value, thetree, parent).get()
      ->getNumofFutureChildren();
  if ((currentempty - level) > 0)
    get(vertex_value, thetree, parent).get()->setNumofFutureChildren(0, 1);
  else
    //end of the game
    assert(countofchildren == 0);

//expansion code
  int selectnode = get(vertex_index, thetree, parent),
      indexofchild = selectnode;

  if (static_cast<int>(getNodeDepth(guard, selectnode)) != currentempty)  //the selected node is the end of game which might be root (not empty cell for move) or any leaf which cannot be expanded no more
    indexofchild = expandNode(guard, selectnode, 0);

#ifndef NDEBUG
  {
    strict_lock<recursive_mutex> lock(mutex_);
    clog << DEBUGHEADER() << this_thread::get_id() << " releases the lock "
         << this << endl;
  }
#endif
  return indexofchild;
}
int LockableGameTree::selectMaxBalanceNode(int currentempty,
bool isbreaktie) {
  unique_lock<LockableGameTree> guard(*this);
  return selectMaxBalanceNode(guard, currentempty, isbreaktie);
}
size_t LockableGameTree::getNodeDepth(unique_lock<LockableGameTree>&,
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
  unique_lock<LockableGameTree> guard(*this);
  return getNodeDepth(guard, indexofnode);
}
void LockableGameTree::getMovesfromTreeState(unique_lock<LockableGameTree>&,
                                             int indexofnode,
                                             vector<int>& babywatsons,
                                             vector<int>& opponents) {
  vertex_t node = vertex(indexofnode, thetree);
  in_edge_iter viter, viterend;
  while (node != _root) {
    if (get(vertex_color, thetree, node) != get(vertex_color, thetree, _root))
      babywatsons.push_back(get(vertex_position, thetree, node));
    else
      opponents.push_back(get(vertex_position, thetree, node));
    assert(in_degree(node, thetree) == 1);
    for (tie(viter, viterend) = in_edges(node, thetree); viter != viterend;
        ++viter)
      node = source(*viter, thetree);
  }
}
void LockableGameTree::getMovesfromTreeState(int indexofnode,
                                             vector<int>& babywatsons,
                                             vector<int>& opponents) {
  unique_lock<LockableGameTree> guard(*this);
  return getMovesfromTreeState(guard, indexofnode, babywatsons, opponents);
}
vector<size_t> LockableGameTree::getSiblings(
    unique_lock<LockableGameTree>& guard, size_t indexofnode) {
  vertex_t node = vertex(indexofnode, thetree);
  vertex_t parent = getParent(guard, node);
  vector<size_t> siblings;
  siblings.reserve(getNumofChildren(guard, get(vertex_index, thetree, parent)));
  out_edge_iter viter, viterend;
  for (tie(viter, viterend) = out_edges(parent, thetree); viter != viterend;
      ++viter) {
    vertex_t child = target(*viter, thetree);
    siblings.push_back(get(vertex_index, thetree, child));
  }
  return vector<size_t>(siblings);
}
vector<size_t> LockableGameTree::getSiblings(size_t indexofnode) {
  unique_lock<LockableGameTree> guard(*this);
  return getSiblings(guard, indexofnode);
}
LockableGameTree::vertex_t LockableGameTree::getParent(
    unique_lock<LockableGameTree>&, vertex_t node) {
  in_edge_iter viter, viterend;
  assert(in_degree(node, thetree) == 1);
  vertex_t parent = graph_traits<basegraph>::null_vertex();
  for (tie(viter, viterend) = in_edges(node, thetree); viter != viterend;
      ++viter)
    parent = source(*viter, thetree);
  return parent;
}
LockableGameTree::vertex_t LockableGameTree::getParent(vertex_t node) {
  unique_lock<LockableGameTree> guard(*this);
  return getParent(guard, node);
}
void LockableGameTree::setNodePosition(unique_lock<LockableGameTree>& guard,
                                       size_t indexofnode, size_t position) {
  vertex_t node = vertex(indexofnode, thetree);
  updateNodePosition(guard, node, position);
  updateNodeName(guard, node);
}
void LockableGameTree::setNodePosition(size_t indexofnode, size_t position) {
  unique_lock<LockableGameTree> guard(*this);
  return setNodePosition(guard, indexofnode, position);
}
size_t LockableGameTree::getNodePosition(unique_lock<LockableGameTree>&,
                                         size_t indexofnode) {
  vertex_t node = vertex(indexofnode, thetree);
  return get(vertex_position, thetree, node);
}
size_t LockableGameTree::getNodePosition(std::size_t indexofnode) {
  unique_lock<LockableGameTree> guard(*this);
  return getNodePosition(guard, indexofnode);
}
int LockableGameTree::updateNodefromSimulation(
    unique_lock<LockableGameTree>& guard, int indexofnode, int winner) {
#ifndef NDEBUG
  {
    strict_lock<recursive_mutex> lock(mutex_);
    cerr << DEBUGHEADER() << boost::this_thread::get_id()
         << " acquires the lock " << this << endl;
  }
#endif
  vertex_t node = vertex(indexofnode, thetree);
#if __cplusplus > 199711L
  int value = 0;
  if (winner > 0) {
    value = get(vertex_value, thetree, node).get()->feature(
        LockableUTCPolicy::valuekind::wincount);
    if (get(vertex_color, thetree, node) == get(vertex_color, thetree, _root))  //a minimizing node
      get(vertex_value, thetree, node).get()->updateAll(
          LockableUTCPolicy::valuekind::visitcount, 0, 1,
          LockableUTCPolicy::valuekind::wincount, 0, -1);
    else
      get(vertex_value, thetree, node).get()->updateAll(
          LockableUTCPolicy::valuekind::visitcount, 0, 1,
          LockableUTCPolicy::valuekind::wincount, 0, 1);
    value = get(vertex_value, thetree, node).get()->feature(
        LockableUTCPolicy::valuekind::wincount) - value;
  } else
    get(vertex_value, thetree, node).get()->updateAll(
        LockableUTCPolicy::visitcount, 0, 1, LockableUTCPolicy::wincount, 0, 0);
#else
  int value = 0;
  if (winner > 0) {
    value = get(vertex_value, thetree, node).get()->feature(LockableUTCPolicy::wincount);
    if (get(vertex_color, thetree, node) == get(vertex_color, thetree, _root))  //a minimizing node
    get(vertex_value, thetree, node).get()->updateAll(LockableUTCPolicy::visitcount, 0, 1, LockableUTCPolicy::wincount, 0, -1);
    else
    get(vertex_value, thetree, node).get()->updateAll(LockableUTCPolicy::visitcount, 0, 1, LockableUTCPolicy::wincount, 0, 1);
    value = get(vertex_value, thetree, node).get()->feature(LockableUTCPolicy::wincount) - value;
  } else
  get(vertex_value, thetree, node).get()->updateAll(LockableUTCPolicy::visitcount, 0, 1, LockableUTCPolicy::wincount, 0, 0);
#endif
#ifndef NDEBUG
  {
    strict_lock<recursive_mutex> lock(mutex_);
    cerr << DEBUGHEADER() << boost::this_thread::get_id()
         << " releases the lock " << this << endl;
  }
#endif
  backpropagatefromSimulation(guard, indexofnode, value);
  return value;
}
int LockableGameTree::updateNodefromSimulation(int indexofnode, int winner) {
  unique_lock<LockableGameTree> guard(*this);
  return updateNodefromSimulation(guard, indexofnode, winner);
}
void LockableGameTree::backpropagatefromSimulation(
    unique_lock<LockableGameTree>& guard, int indexofnode, int value,
    int level) {
  vertex_t node = vertex(indexofnode, thetree);
  backpropagate(guard, node, value, level);  //to the root
}
void LockableGameTree::backpropagatefromSimulation(int indexofnode, int value,
                                                   int level) {
  unique_lock<LockableGameTree> guard(*this);
  return backpropagatefromSimulation(guard, indexofnode, value, level);
}
void LockableGameTree::backpropagate(unique_lock<LockableGameTree>&,
                                     vertex_t leaf, int value, int level) {
#ifndef NDEBUG
  {
    strict_lock<recursive_mutex> lock(mutex_);
    cerr << DEBUGHEADER() << boost::this_thread::get_id()
         << " acquires the lock " << this << endl;
  }
#endif
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
#if __cplusplus > 199711L
      get(vertex_value, thetree, parent).get()->updateAll(
          LockableUTCPolicy::visitcount, 0, 1, LockableUTCPolicy::wincount, 0,
          curvalue);
      if (get(vertex_color, thetree, parent)
          == get(vertex_color, thetree, _root) && (parent != _root))
        assert(
            get(vertex_value, thetree, parent).get()->feature(
                LockableUTCPolicy::valuekind::wincount) <= 0);  //make sure it's minimizing node
      else if (parent != _root)
        assert(
            get(vertex_value, thetree, parent).get()->feature(
                LockableUTCPolicy::valuekind::wincount) >= 0);  //make sure it's maximizing node

      assert(
          abs(get(vertex_value, thetree, parent).get()->feature(
              LockableUTCPolicy::valuekind::wincount))
              <= get(vertex_value, thetree, parent).get()->feature(
                  LockableUTCPolicy::valuekind::visitcount));
#else
      get(vertex_value, thetree, parent).get()->updateAll(LockableUTCPolicy::visitcount, 0, 1, LockableUTCPolicy::wincount, 0,curvalue);
      if (get(vertex_color, thetree, parent) == get(vertex_color, thetree, _root) && (parent != _root))
      assert(
          get(vertex_value, thetree, parent).get()->feature(LockableUTCPolicy::wincount) <= 0);  //make sure it's minimizing node
      else if (parent != _root)
      assert(
          get(vertex_value, thetree, parent).get()->feature(LockableUTCPolicy::wincount) >= 0);//make sure it's maximizing node

      assert(
          abs(get(vertex_value, thetree, parent).get()->feature(LockableUTCPolicy::wincount))
          <= get(vertex_value, thetree, parent).get()->feature(LockableUTCPolicy::visitcount));
#endif
    }
    node = parent;
    curvalue = -1 * curvalue;
    --curlevel;
    if (curlevel == 0)
      break;
  }
  //assert(notifyAllUpdateDone(guard, leaf, level));
  holdforupdate.notify_all();
#ifndef NDEBUG
  {
    strict_lock<recursive_mutex> lock(mutex_);
    cerr << DEBUGHEADER() << boost::this_thread::get_id()
         << " releases the lock " << this << endl;
  }
#endif
}
void LockableGameTree::backpropagate(vertex_t leaf, int value, int level) {
  unique_lock<LockableGameTree> guard(*this);
  return backpropagate(guard, leaf, value, level);
}
size_t LockableGameTree::getNumofChildren(unique_lock<LockableGameTree>&,
                                          size_t indexofnode) {
  vertex_t parent = vertex(indexofnode, thetree);
  return out_degree(parent, thetree);
}
size_t LockableGameTree::getNumofChildren(size_t indexofnode) {
  unique_lock<LockableGameTree> guard(*this);
  return getNumofChildren(guard, indexofnode);
}
int LockableGameTree::getParent(unique_lock<LockableGameTree>& guard,
                                int indexofnode) {
  vertex_t node = vertex(indexofnode, thetree);
  vertex_t parent = getParent(guard, node);
  return get(vertex_index, thetree, parent);
}
int LockableGameTree::getParent(int indexofnode) {
  unique_lock<LockableGameTree> guard(*this);
  return getParent(guard, indexofnode);
}
size_t LockableGameTree::getNumofTotalNodes(unique_lock<LockableGameTree>&) {
  return num_vertices(thetree);
}
size_t LockableGameTree::getNumofTotalNodes() {
  unique_lock<LockableGameTree> guard(*this);
  return getNumofTotalNodes(guard);
}
bool LockableGameTree::notifyAllUpdateDone(vertex_t leaf, int level) {
  unique_lock<LockableGameTree> guard(*this);
  return notifyAllUpdateDone(guard, leaf, level);
}
