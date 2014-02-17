/*
 * LockableGameTree.cpp
 *
 *  Created on: Feb 11, 2014
 *      Author: renewang
 */

#include <iostream>
#include <boost/thread/strict_lock.hpp>

#include "Global.h"
#include "LockableGameTree.h"

using namespace std;
using namespace boost;
#if __cplusplus > 199711L
LockableGameTree::LockableGameTree()
    : LockableGameTree('W', 0) {
}
LockableGameTree::LockableGameTree(char playerscolor)
    : LockableGameTree(playerscolor, 0) {
}
#else
LockableGameTree::LockableGameTree()
    : GameTree('W', 0), thetree(GameTree::getTree()){
  initGameTree();
}
LockableGameTree::LockableGameTree(char playerscolor)
    : GameTree(playerscolor, 0), thetree(GameTree::getTree()) {
  initGameTree();
}
#endif
LockableGameTree::LockableGameTree(char playerscolor, size_t indexofroot)
    : GameTree(playerscolor, indexofroot),
      thetree(GameTree::getTree()) {
  initGameTree();
}
int LockableGameTree::selectMaxBalanceNode(int currentempty, bool isbreaktie){
  return GameTree::selectMaxBalanceNode(currentempty, isbreaktie);
}
int LockableGameTree::expandNode(int indexofsource, int move, char color){
  return GameTree::expandNode(indexofsource, move, color);
}
void LockableGameTree::setNodePosition(std::size_t indexofnode, std::size_t position){
  return GameTree::setNodePosition(indexofnode, position);
}
int LockableGameTree::updateNodefromSimulation(int indexofnode, int winner){
  return GameTree::updateNodefromSimulation(indexofnode, winner);
}
void LockableGameTree::backpropagatefromSimulation(int indexofnode, int value, int level){
  return GameTree::backpropagatefromSimulation(indexofnode, value, level);
}
void LockableGameTree::initGameTree(){
  vertexname = get(vertex_name, thetree);
  vertexcolor = get(vertex_color, thetree);
  vertexvalue = get(vertex_value, thetree);
  vertexposition = get(vertex_position, thetree);
  vertexindex = get(vertex_index, thetree);
  _root = GameTree::getRoot();
}
