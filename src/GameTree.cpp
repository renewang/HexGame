/*
 * GameTree.cpp
 *
 *  Created on: Jan 3, 2014
 *      Author: renewang
 */

#include <cmath>
#include <chrono>
#include <utility>
#include <sstream>

#include "Global.h"
#include "GameTree.h"
#include "PriorityQueue.h"

using namespace std;
using namespace boost;
GameTree::GameTree()
    : GameTree('W', 0) {
}
GameTree::GameTree(char playerscolor)
    : GameTree(playerscolor, 0) {
}
GameTree::GameTree(char playerscolor, size_t indexofroot) {
  vertexname = get(vertex_name, thetree);
  vertexcolor = get(vertex_color, thetree);
  vertexvalue = get(vertex_value, thetree);
  vertexposition = get(vertex_position, thetree);
  vertexindex = get(vertex_index, thetree);

  //set the root's color label as opponent's color label
  char rootscolor = 'W';
  if (playerscolor == 'R')
    rootscolor = 'B';
  else if (playerscolor == 'B')
    rootscolor = 'R';
  vertex_t root = addNode(indexofroot, rootscolor);

  _root = root;
}
GameTree::vertex_t GameTree::addNode(std::size_t positionofchild, char color) {
  vertex_t target = add_vertex(thetree);
  updateNodeColor(target, color);
  updateNodePosition(target, positionofchild);
  updateNodeValue(target);
  updateNodeName(target);
  return target;
}
bool GameTree::addEdge(vertex_t source, vertex_t target) {
  pair<edge_t, bool> result = add_edge(source, target, thetree);
  return result.second;
}
void GameTree::removeChidren(string nameofsource) {
  size_t indexofsource = stoul(nameofsource);
  vertex_t source = vertex(indexofsource, thetree);
  clear_out_edges(source, thetree);
}
//remove subtree of given vertex index
void GameTree::prune(vertex_t node) {
  remove_vertex(node, thetree);
}
//root doesn't have in-edges. Replace all in-edges with out-edges
void GameTree::reRoot(vertex_t node, bool isprune) {
  //iterate through in-edges
  in_edge_iter viter, viterend;
  if (!isprune) {
    for (boost::tie(viter, viterend) = in_edges(node, thetree);
        viter != viterend; ++viter)
      add_edge(target(*viter, thetree), source(*viter, thetree), thetree);
  }
  clear_in_edges(node, thetree);
  _root = node;
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
      vertexvalue[parent].update(UTCPolicy::valuekind::wincount, 0, curvalue);  //increment winning count
      vertexvalue[parent].update(UTCPolicy::valuekind::visitcount, 0, 1);  //increment visit count

      if (vertexcolor[parent] == vertexcolor[_root] && (parent != _root))
        assert(
            vertexvalue[parent].feature(UTCPolicy::valuekind::wincount) <= 0);  //make sure it's minimizing node
      else if (parent != _root)
        assert(
            vertexvalue[parent].feature(UTCPolicy::valuekind::wincount) >= 0);  //make sure it's maximizing node

      assert(
          abs(vertexvalue[parent].feature(UTCPolicy::valuekind::wincount))
              <= vertexvalue[parent].feature(UTCPolicy::valuekind::visitcount));
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
  namebuffer << vertexindex[node];
  namebuffer << "@";
  namebuffer << vertexposition[node];
  namebuffer << ":";
  if (vertexcolor[node] == black_color)
    namebuffer << 'B';
  else if (vertexcolor[node] == red_color)
    namebuffer << 'R';
  else
    namebuffer << 'W';
  vertexname[node] = namebuffer.str();
}
//update value
void GameTree::updateNodeValue(vertex_t node, UTCPolicy& policy) {
  vertexvalue[node] = policy;
}
//update value
void GameTree::updateNodeValue(vertex_t node) {
  vertexvalue[node] = UTCPolicy();
}
//update color
void GameTree::updateNodeColor(vertex_t node, char color) {
  switch (color) {
    case 'R':
      vertexcolor[node] = red_color;
      break;
    case 'B':
      vertexcolor[node] = black_color;  //temporary
      break;
    default:
      vertexcolor[node] = white_color;
  }
}
//update position
void GameTree::updateNodePosition(vertex_t node, std::size_t position) {
  vertexposition[node] = position;
}
size_t GameTree::getNodePosition(size_t indexofnode) {
  vertex_t node = vertex(indexofnode, thetree);
  return vertexposition[node];
}
int GameTree::updateNodefromSimulation(int indexofnode, int winner) {
  vertex_t node = vertex(indexofnode, thetree);
  vertexvalue[node].update(UTCPolicy::valuekind::visitcount, 0, 1);
  int value = 0;
  if (winner > 0) {
    value = vertexvalue[node].feature(UTCPolicy::valuekind::wincount);
    if (vertexcolor[node] == vertexcolor[_root])  //a minimizing node
      vertexvalue[node].update(UTCPolicy::valuekind::wincount, 0, -1);
    else
      vertexvalue[node].update(UTCPolicy::valuekind::wincount, 0, 1);
    value = vertexvalue[node].feature(UTCPolicy::valuekind::wincount) - value;
  }
  return value;
}
void GameTree::backpropagatefromSimulation(int indexofnode, int value,
                                           int level) {
  vertex_t node = vertex(indexofnode, thetree);
  backpropagate(node, value, level);  //to the root
}
int GameTree::expandNode(int indexofsource, int move, char color) {
  vertex_t source = vertex(indexofsource, thetree);
  int indexofchild = -1;

  //adding the different color from the parental node
  vertex_t target;
  if (vertexcolor[source] == red_color)
    target = addNode(static_cast<size_t>(move), 'B');
  else if (vertexcolor[source] == black_color)
    target = addNode(static_cast<size_t>(move), 'R');
  else
    target = addNode(static_cast<size_t>(move), color);

  if (addEdge(source, target))
    indexofchild = vertexindex[target];

  assert(indexofchild > 0);
  return indexofchild;
}
//called in MCST selection phase
int GameTree::selectMaxBalanceNode(int currentempty, bool isbreaktie) {
  vertex_t parent = _root;
  out_edge_iter viter, viterend;
  size_t numofchildren = out_degree(parent, thetree);
  int level = 0;
  default_random_engine generator(static_cast<unsigned>(chrono::system_clock::now().time_since_epoch().count()));
  uniform_real_distribution<double> distribution(0.0, 1.0);
  while (numofchildren != 0) {  //reach leaf
    //test if the current examining node is fully expanded, if yes then return its child; otherwise, return the current node for expansion
    assert((currentempty - level) > 0);  //currentempty - level = 0 indicates the end of game
    if (static_cast<int>(numofchildren) < currentempty - level)
      break;
    PriorityQueue<vertex_t, double> vertexchooser(numofchildren);
    for (tie(viter, viterend) = out_edges(parent, thetree); viter != viterend;
        ++viter) {
      vertex_t node = target(*viter, thetree);
      double value = -vertexvalue[node].calculate(vertexvalue[parent]);
      if (isbreaktie && vertexchooser.containsPriority(value)) {  // break the tie
        double prob = distribution(generator);
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
  int selectnode = vertexindex[parent];
  return selectnode;
}
pair<int, double> GameTree::getBestMovefromSimulation() {
  //1. examine all children nodes below the root node
  size_t numofchildren = out_degree(_root, thetree);
  PriorityQueue<vertex_t, double> vertexchooser(numofchildren);
  out_edge_iter viter, viterend;
  for (tie(viter, viterend) = out_edges(_root, thetree); viter != viterend;
      ++viter) {
    vertex_t node = target(*viter, thetree);
    vertexchooser.insert(node, -vertexvalue[node].estimate());
  }
#ifndef NDEBUG
  DEBUGHEADER();
  cerr << "log the simulation statistics for final choice" << endl;
  for (tie(viter, viterend) = out_edges(_root, thetree); viter != viterend;
      ++viter)
    cerr << vertexposition[target(*viter, thetree)] << ":"
         << vertexvalue[target(*viter, thetree)].getValue() << " ";
  cerr << endl;
#endif
  //2. choose the maximal value from all children nodes of root.
  int indexofbestmove = vertexchooser.minPrioirty();
  double maxvalue =
      vertexvalue[static_cast<vertex_t>(indexofbestmove)].getValue();
  assert(indexofbestmove > 0);
  assert(maxvalue >= 0.0);
  return make_pair(indexofbestmove, maxvalue);
}
void GameTree::reRootfromSimulation(int indexofnode, bool isprune) {
  vertex_t node = vertex(indexofnode, thetree);
  reRoot(node, isprune);
}
void GameTree::getMovesfromTreeState(int indexofnode, vector<int>& babywatsons,
                                     vector<int>& opponents) {
  vertex_t node = vertex(indexofnode, thetree);
  in_edge_iter viter, viterend;
  while (node != _root) {
    if (vertexcolor[node] != vertexcolor[_root])
      babywatsons.push_back(vertexposition[node]);
    else
      opponents.push_back(vertexposition[node]);
    assert(in_degree(node, thetree) == 1);
    for (tie(viter, viterend) = in_edges(node, thetree); viter != viterend;
        ++viter)
      node = source(*viter, thetree);
  }
}
int GameTree::reRootbyPosition(size_t position) {
  out_edge_iter viter, viterend;
  vertex_t node = graph_traits<basegraph>::null_vertex();
  for (tie(viter, viterend) = out_edges(_root, thetree); viter != viterend;
      ++viter) {
    if (vertexposition[target(*viter, thetree)] == position) {
      node = target(*viter, thetree);
      break;
    }
  }
  reRoot(node, true);
  return vertexindex[node];
}
void GameTree::prunebyPosition(size_t position, int level) {
  int curlevel = level;
  traversetoPruneBy(position, --curlevel, _root);
}
//TODO temporary, by hand right now. Figure out a way to fit in boost graph framework
void GameTree::traversetoPruneBy(size_t position, int curlevel, vertex_t node) {
  size_t numofchildren = out_degree(node, thetree);
  if (numofchildren == 0 || curlevel == 0) {  //reach leaf
    if (vertexposition[node] != position)
      prune(node);
    return;
  } else if (curlevel == 0) {
    out_edge_iter viter, viterend;
    vector<vertex_t> nodestoprune;
    for (tie(viter, viterend) = out_edges(node, thetree); viter != viterend;
        ++viter) {
      vertex_t child = target(*viter, thetree);
      if (vertexposition[child] == position)
        nodestoprune.push_back(child);
    }
    for (unsigned i = 0; i < nodestoprune.size(); ++i)
      prune(nodestoprune[i]);

    curlevel--;
    for (tie(viter, viterend) = out_edges(node, thetree); viter != viterend;
        ++viter)
      traversetoPruneBy(position, curlevel, target(*viter, thetree));
  }
}
int GameTree::getNodeValueFeature(int indexofnode, int indexofkind) {
  vertex_t node = vertex(indexofnode, thetree);
  return vertexvalue[node].feature(indexofkind);
}
void GameTree::clearAll() {
  char rootscolor = 'W';
  if (vertexcolor[_root] == red_color)
    rootscolor = 'R';
  else if (vertexcolor[_root] == black_color)
    rootscolor = 'B';
  int indexofroot = vertexindex[_root];
  thetree.clear();
  vertex_t root = addNode(indexofroot, rootscolor);
  _root = root;
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
  vertex_t node = vertex(indexofnode, thetree);
  vertex_t parent = getParent(node);
  vector<size_t> siblings;
  siblings.reserve(out_degree(parent, thetree));
  out_edge_iter viter, viterend;
  for (tie(viter, viterend) = out_edges(parent, thetree); viter != viterend;
      ++viter) {
    vertex_t child = target(*viter, thetree);
    siblings.push_back(vertexindex[child]);
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
