/*
 * PriorityQueue.h
 * This file defines a customized priority queue to accelerate Dijkstra algorithm
 *
 */

#ifndef PRIORITYQUEUE_H_
#define PRIORITYQUEUE_H_

#include "Graph.h"
#include <queue>
#include <deque>
#include <algorithm>

/* PriorityQueue Class is used to hold the distance from the source node
 * and for the purpose to return the node with minimal distance
 * The template Type and Val should be consistent with the Graph
 *
 * Sample Usage:
 *
 * Graph<string, double> randomG(5, 0.5, 10);
 * PriorityQueue<string, double> testPQ;
 * for (unsigned i = 0; i < vertices.size(); i++)
 *   testPQ.insert(&(vertices[i]), 10 - i);
 * Node<string, double>* minNode = const_cast<Node<string, double>*>(testPQ.minPrioirty());
 */
template<class Type, class Val>
class PriorityQueue {
  typedef Node<Type, Val> Node;

  //NodePriority structure is used to hold the node and priority of the node.
  //In shortest path calculation should be the total weight of the path leading to the node.
 private:
  struct NodePriority {
    const Node* node;
    Val priority;  //priority of the node or the total weight of the path leading to the node

    friend bool operator ==(NodePriority& a, NodePriority& b) {
      return (a.node)->vertexindex == (b.node)->vertexindex;
    }
  };
  //NodeComparator is used to define how to compare the node object in make_heap function
  struct NodeComparator {
    bool operator()(NodePriority& a, NodePriority& b) {
      return a.priority > b.priority;  //calls your operator
    }
  };
  //Find the corresponding element in tracker
  typename std::vector<NodePriority>::iterator findElement(
      NodePriority& qelement) {
    typename std::vector<NodePriority>::iterator iter = nodetracker.begin();

    for (; iter != nodetracker.end(); ++iter) {
      if ((*iter) == qelement)
        break;
    }
    return iter;
  }
  ;
  std::vector<NodePriority> nodetracker;  //the underlying representation of priority queue, is used to hold the elements

 public:
  //Default constructor, takes no parameters
  PriorityQueue() {
  }
  ;
  //Default destructor, takes no parameters
  virtual ~PriorityQueue() {
  }
  ;
  //Change the priority or the total weight of the path leading to the specified node
  //Input:
  //node: the current end node of a trial path
  //priority: the priority of the end node needs to be changed
  //Output:
  //A boolean variable to indicate if the change of priority is successful.
  //TRUE: change priority of the specified node is successful
  //FALSE: change priority of the specified node is unsuccessful
  bool chgPrioirity(const Node* node, Val priority) {
    bool issuccess = false;
    if (contains(node)) {
      NodePriority newqelement;
      newqelement.node = node;
      newqelement.priority = priority;
      typename std::vector<NodePriority>::iterator chgiter = findElement(
          newqelement);
      if (chgiter != nodetracker.end()) {
        chgiter->priority = priority;
        make_heap(nodetracker.begin(), nodetracker.end(), NodeComparator());
        issuccess = true;
      }
    }
    return issuccess;
  }
  ;
  //Return the Node with the minimal priority or the total weight
  //Input: NONE
  //Output: the node associating with the minimal priority or the total weight of a path
  const Node* minPrioirty() {
    //removes the top element of the queue.
    const Node* minnode = top();
    pop_heap(nodetracker.begin(), nodetracker.end(), NodeComparator());
    nodetracker.pop_back();
    return minnode;
  }
  ;
  //Test if the priority queue contains the specified node
  //Input:
  //node: the inquiring node
  //Output:
  //the boolean value to indicate if the node is contained in the queue or not
  //TRUE: the node is contained in the priority queue
  //FALSE: the node is not contained in the priroity queue
  bool contains(const Node* node) {
    NodePriority newqelement;
    newqelement.node = node;
    bool iscontained = findElement(newqelement) != nodetracker.end();
    return iscontained;
  }
  ;
  //Insert element into queue
  //Input:
  //node: the node needs to be inserted
  //priority:  the corresponding priority of the node
  //Output: NONE
  void insert(const Node* node, Val priority) {

    NodePriority qelement;
    qelement.node = node;
    qelement.priority = priority;
    nodetracker.push_back(qelement);
  }
  ;
  //Returns the top element of the queue.
  //Input: NONE
  //Output:
  //Return the Node object with the minimal priority or weight.
  //Calling this function won't remove top node from queue
  const Node* top() {
    make_heap(nodetracker.begin(), nodetracker.end(), NodeComparator());
    NodePriority topnode = nodetracker.front();
    return topnode.node;
  }
  ;
  //Return the number of queue_elements.
  //Input: NONE
  //Output:
  //The size of current priority queue
  unsigned size() {
    return nodetracker.size();
  }
  ;
};

#endif /* PRIORITYQUEUE_H_ */
