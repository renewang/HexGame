/*
 * GameTree.cpp
 *
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
#if __cplusplus > 199711L
GameTree::GameTree()
    : GameTree('W', 0) {
}
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
GameTree::GameTree(char playerscolor, size_t indexofroot) {
  initGameTree(playerscolor, indexofroot);
}
GameTree::vertex_t GameTree::addNode(std::size_t positionofchild, char color) {
  vertex_t target = add_vertex(thetree);
  //TODO re-factor
  updateNodePosition(target, positionofchild);
  updateNodeColor(target, color);
  updateNodeValue(target);
  updateNodeName(target);
  return target;
}
bool GameTree::addEdge(vertex_t source, vertex_t target) {
  pair<edge_t, bool> result = add_edge(source, target, thetree);
  return result.second;
}
void GameTree::removeChidren(string nameofsource) {
#if __cplusplus > 199711L
  size_t indexofsource = stoul(nameofsource);
#else
  size_t indexofsource = atoi(nameofsource.c_str());
#endif
  vertex_t source = vertex(indexofsource, thetree);
  clear_out_edges(source, thetree);
}
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
#if __cplusplus > 199711L
      get(vertex_value, thetree, parent).get()->updateAll(
          AbstractUTCPolicy::valuekind::visitcount, 0, 1,
          AbstractUTCPolicy::valuekind::wincount, 0, curvalue);
      if (get(vertex_color, thetree, parent)
          == get(vertex_color, thetree, _root) && (parent != _root))
        assert(
            get(vertex_value, thetree, parent).get()->feature(
                AbstractUTCPolicy::valuekind::wincount) <= 0);  //make sure it's minimizing node
      else if (parent != _root)
        assert(
            get(vertex_value, thetree, parent).get()->feature(
                AbstractUTCPolicy::valuekind::wincount) >= 0);  //make sure it's maximizing node

      assert(
          abs(get(vertex_value, thetree, parent).get()->feature(
              AbstractUTCPolicy::valuekind::wincount))
              <= get(vertex_value, thetree, parent).get()->feature(
                  AbstractUTCPolicy::valuekind::visitcount));
#else
      get(vertex_value, thetree, parent).get()->updateAll(AbstractUTCPolicy::visitcount, 0, 1, AbstractUTCPolicy::wincount, 0, curvalue);
      if (get(vertex_color, thetree, parent) == get(vertex_color, thetree, _root) && (parent != _root))
      assert(
          get(vertex_value, thetree, parent).get()->feature(AbstractUTCPolicy::wincount) <= 0);  //make sure it's minimizing node
      else if (parent != _root)
      assert(
          get(vertex_value, thetree, parent).get()->feature(AbstractUTCPolicy::wincount) >= 0);//make sure it's maximizing node

      assert(
          abs(get(vertex_value, thetree, parent).get()->feature(AbstractUTCPolicy::wincount))
          <= get(vertex_value, thetree, parent).get()->feature(AbstractUTCPolicy::visitcount));
#endif
    }
    node = parent;
    curvalue = -1 * curvalue;
    --curlevel;
    if (curlevel == 0)
      break;
  }
}
//update name
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
void GameTree::updateNodeValue(vertex_t node) {
  put(vertex_value, thetree, node,
      hexgame::shared_ptr<AbstractUTCPolicy>(new UTCPolicy()));
}
//update color
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
void GameTree::updateNodePosition(vertex_t node, std::size_t position) {
  put(vertex_position, thetree, node, position);
}
size_t GameTree::getNodePosition(size_t indexofnode) {
  vertex_t node = vertex(indexofnode, thetree);
  return get(vertex_position, thetree, node);
}
int GameTree::updateNodefromSimulation(int indexofnode, int winner, int level) {
  vertex_t node = vertex(indexofnode, thetree);
#if __cplusplus > 199711L
  int value = 0;
  if (winner > 0) {
    value = get(vertex_value, thetree, node).get()->feature(
        AbstractUTCPolicy::valuekind::wincount);
    if (get(vertex_color, thetree, node) == get(vertex_color, thetree, _root))  //a minimizing node
      get(vertex_value, thetree, node).get()->updateAll(
          AbstractUTCPolicy::valuekind::visitcount, 0, 1,
          AbstractUTCPolicy::valuekind::wincount, 0, -1);
    else
      get(vertex_value, thetree, node).get()->updateAll(
          AbstractUTCPolicy::valuekind::visitcount, 0, 1,
          AbstractUTCPolicy::valuekind::wincount, 0, 1);
    value = get(vertex_value, thetree, node).get()->feature(
        AbstractUTCPolicy::valuekind::wincount) - value;
  } else
    get(vertex_value, thetree, node).get()->updateAll(
        AbstractUTCPolicy::valuekind::visitcount, 0, 1,
        AbstractUTCPolicy::valuekind::wincount, 0, 0);
#else
  int value = 0;
  if (winner > 0) {
    value = get(vertex_value, thetree, node).get()->feature(AbstractUTCPolicy::wincount);
    if (get(vertex_color, thetree, node) == get(vertex_color, thetree, _root))  //a minimizing node
    get(vertex_value, thetree, node).get()->updateAll(AbstractUTCPolicy::visitcount, 0, 1, AbstractUTCPolicy::wincount, 0, -1);
    else
    get(vertex_value, thetree, node).get()->updateAll(AbstractUTCPolicy::visitcount, 0, 1, AbstractUTCPolicy::wincount, 0, 1);
    value = get(vertex_value, thetree, node).get()->feature(AbstractUTCPolicy::wincount) - value;
  } else
  get(vertex_value, thetree, node).get()->updateAll(AbstractUTCPolicy::visitcount, 0, 1, AbstractUTCPolicy::wincount, 0, 0);
#endif
  backpropagate(node, value, level);
  return value;
}
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
int GameTree::selectMaxBalanceNode(int currentempty, bool isbreaktie) {
  vertex_t parent = _root;
  out_edge_iter viter, viterend;
  size_t numofchildren = out_degree(parent, thetree);

#ifndef NDEBUG
  if (maptochildren.find(parent) == maptochildren.end())
    maptochildren.insert(make_pair(parent, 0));
  assert(maptochildren.at(parent) == numofchildren);
#endif

  int level = 0;

#if __cplusplus > 199711L
  default_random_engine generator(
      static_cast<unsigned>(hexgame::chrono::system_clock::now()
          .time_since_epoch().count()));
  uniform_real_distribution<double> distribution(0.0, 1.0);
#else
  srand(static_cast<unsigned>(clock()));
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

#ifndef NDEBUG
    if (maptochildren.find(parent) == maptochildren.end())
      maptochildren.insert(make_pair(parent, 0));
    assert(maptochildren.at(parent) == numofchildren);
#endif
    ++level;
  }
#ifndef NDEBUG
  if (maptochildren.find(parent) == maptochildren.end())
    maptochildren.insert(make_pair(parent, 1));
  else
    ++maptochildren.at(parent);
  if ((currentempty - level) == 0) {
    --maptochildren.at(parent);
    assert(maptochildren.at(parent) == out_degree(parent, thetree));
  } else
    assert(maptochildren.at(parent) == out_degree(parent, thetree) + 1);
  assert(
      static_cast<int>(getNodeDepth(get(vertex_index, thetree, parent)))
          == level);
#endif
  return get(vertex_index, thetree, parent);
}
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
#ifdef NDEBUG
    srand((unsigned long)clock());
#endif
    size_t index = rand() % remainingmoves.size();
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
int GameTree::getNodeValueFeature(int indexofnode,
                                  AbstractUTCPolicy::valuekind indexofkind) {
  vertex_t node = vertex(indexofnode, thetree);
  return get(vertex_value, thetree, node).get()->feature(indexofkind);
}
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
  maptochildren.clear();
}
size_t GameTree::getSizeofNodes() {
  return num_vertices(thetree);
}
size_t GameTree::getSizeofEdges() {
  size_t numofedges = num_edges(thetree);
  assert(numofedges == num_vertices(thetree) - 1);
  return numofedges;
}
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
void GameTree::setNodePosition(size_t indexofnode, size_t position) {
  vertex_t node = vertex(indexofnode, thetree);
  updateNodePosition(node, position);
  updateNodeName(node);
}
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
size_t GameTree::getNumofTotalNodes() {
  return num_vertices(thetree);
}
size_t GameTree::getNumofChildren(size_t indexofnode) {
  vertex_t parent = vertex(indexofnode, thetree);
  return (out_degree(parent, thetree));
}
