/*
 * GameTree.cpp
 *
 *  Created on: Jan 3, 2014
 *      Author: renewang
 */

#include <utility>
#include <sstream>
#include "GameTree.h"

using namespace std;
using namespace boost;

GameTree::GameTree(size_t indexofroot) {
  vertexname = get(vertex_name, thetree);
  vertexcolor = get(vertex_color, thetree);
  vertexvalue = get(vertex_value, thetree);
  vertexposition = get(vertex_position, thetree);
  vertexkey = get(vertex_key, thetree);

  dynamic_bitset<unsigned char> keyofroot(numeric_limits<unsigned char>::digits,
                                          0ul);
  vertex_t root = add_vertex(thetree);
  /*
  string nameofroot;
  to_string(keyofroot, nameofroot);*/
  string nameofroot = to_string(indexofroot) + ":W";
  updateNodeName(root, nameofroot);
  updateNodeColor(root, 'W');
  updateNodePosition(root, indexofroot);
  updateNodeKey(root, keyofroot);
  updateNodeValue(root, UTCPolicy());
  _root = root;
}
bool GameTree::addChild(vertex_t source,
                        dynamic_bitset<unsigned char> keyoftarget,
                        size_t positionofchild, char color) {
  string nameoftarget;
  //to_string(keyoftarget, nameoftarget);
  nameoftarget = to_string(positionofchild) + ":" + color;

  //size_t indexoftarget = _namehash(nameoftarget);
  //vertex_t target = vertex(indexoftarget, thetree);

  vertex_t target = add_vertex(thetree);
  pair<edge_t, bool> result = add_edge(source, target, thetree);
  if (result.second) {
    updateNodeColor(target, color);
    updateNodePosition(target, positionofchild);
    updateNodeValue(target, UTCPolicy());
    updateNodeKey(target, keyoftarget);
    updateNodeName(target, nameoftarget);
  }
  return result.second;
}
void GameTree::removeChidren(string nameofsource) {
  size_t indexofsource = stoul(nameofsource);
  vertex_t source = vertex(indexofsource, thetree);
  clear_out_edges(source, thetree);
}
//remove subtree of given vertex index
void GameTree::prune(string nameofindex) {
  size_t indexofnode = stoul(nameofindex);
  vertex_t parent = vertex(indexofnode, thetree);
  remove_vertex(parent, thetree);
}
//root doesn't have in-edges. Replace all in-edges with out-edges
void GameTree::reRoot(string nameofindex, bool isprune) {
  //iterate through in-edges
  size_t indexofnode = stoul(nameofindex);
  vertex_t node = vertex(indexofnode, thetree);
  in_edge_iter viter, viterend;
  for (boost::tie(viter, viterend) = in_edges(node, thetree); viter != viterend;
      ++viter) {
    if (!isprune)
      add_edge(target(*viter, thetree), source(*viter, thetree), thetree);
  }
  clear_in_edges(node, thetree);
}
string GameTree::printGameTree(string key) {
  stringstream treebuffer;
  ostream_iterator<string> treebufiter(treebuffer);
  TreePrinter printer(treebufiter);

  vertex_t node;
  if (key == vertexname[_root])
    node = _root;
  else{
    //node = vertex(_namehash(key), thetree);
    size_t index = stoul(key);
    node = vertex(index, thetree);
  }
  gametree treeadptor(thetree, node);
  traverse_tree(node, treeadptor, printer);
  treebuffer << '\n';
  return treebuffer.str();
}
//if level = -1, then will do back-propogate up to the root, starting from current level = 0
void GameTree::backpropogate(dynamic_bitset<unsigned char> key, int level) {
  int curlevel = level;
  vertex_t updatenode = vertex(key.to_ulong(), thetree);
  dynamic_bitset<unsigned char> currentkey = key;
  while (currentkey != vertexkey[_root]) {
    vertex_t node = vertex(currentkey.to_ulong(), thetree);
    updateNodeValue(node, vertexvalue[updatenode]);
    currentkey = currentkey >> sizeof(unsigned char);  //TODO need to test and play code with dynamic_bitset
    curlevel--;
  }
}
//update name
void GameTree::updateNodeName(vertex_t node,
                              dynamic_bitset<unsigned char> key) {
  to_string(key, vertexname[node]);
}
//update name
void GameTree::updateNodeName(vertex_t node, string nameofnode) {
  vertexname[node] = nameofnode;
}
//update value
void GameTree::updateNodeValue(vertex_t node, UTCPolicy policy) {
  vertexvalue[node] = policy;
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
//update key
void GameTree::updateNodeKey(vertex_t node,
                             boost::dynamic_bitset<unsigned char> key) {
  vertexkey[node] = key;
}
void GameTree::addSimulatedMove(int move, char color) {

}
void GameTree::updateNodefromSimulation(int winner) {

}
void GameTree::backpropogatefromSimulation() {

}
pair<vector<int>, vector<int>> GameTree::getMovesfromSimulation() {
  vector<int> redmove;
  vector<int> bluemove;
  return make_pair(vector<int>(redmove), vector<int>(bluemove));
}
double GameTree::getBestMoveValuefromSimulation() {
  double maxvalue;
  return maxvalue;
}
int GameTree::getBestMovefromSimulation() {
  int bestmove;
  return bestmove;
}
bool GameTree::expandLeaf(string nameofparent, int move, char color) {

  dynamic_bitset<unsigned char> keyoftarget(stoul(nameofparent));
  keyoftarget.append(static_cast<unsigned char>(move));  //extend prefix of simulated moves sequence

  vertex_t source;
  if (nameofparent == vertexname[_root])
    source = _root;
  else {
    //size_t indexofsource = _namehash(nameofparent);
    //source = vertex(indexofsource, thetree);
    //now assume pass the string representation of vertex_index_t
    size_t indexofsource = stoul(nameofparent);
    source = vertex(indexofsource, thetree);
  }

  //adding the different color from the parental node
  bool issuccss = false;
  if (vertexcolor[source] == red_color)
    issuccss = addChild(source, keyoftarget, static_cast<size_t>(move), 'B');
  else if (vertexcolor[source] == black_color)
    issuccss = addChild(source, keyoftarget, static_cast<size_t>(move), 'R');
  else
    //adding child under root node
    issuccss = addChild(source, keyoftarget, static_cast<size_t>(move), color);

  assert(issuccss == true);
  return issuccss;
}
string GameTree::calcUpperConfidenceBound() {

}

void GameTree::prunebyPosition(std::size_t position) {

}
void GameTree::reRootbyPosition(std::size_t position, bool isprune) {

}
