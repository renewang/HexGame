/*
 * PriorityQueue.h
 * This file defines a customized priority queue to accelerate Dijkstra algorithm
 *
 */

#ifndef PRIORITYQUEUE_H_
#define PRIORITYQUEUE_H_

#include <queue>
#include <deque>
#include <algorithm>

#include "Graph.h"

/* PriorityQueue Class is used to hold the distance from the source node
 * and for the purpose to return the node with minimal distance
 * The template Val should be consistent with the Graph while N is the object needs to be put in the heap.
 * The template N is not needed to be consistent with the Graph label.
 *
 * Sample Usage:
 *
 * Graph<string, double> randomG(5, 0.5, 10);
 * PriorityQueue<int, double> testPQ;
 * for (unsigned i = 0; i < vertices.size(); i++)
 *   testPQ.insert(i, 10 - i);
 * int indexofminnode = testPQ.minPrioirty();
 */
template<class N, class Val>
class PriorityQueue {

  //NodePriority structure is used to hold the node and priority of the node.
  //In shortest path calculation should be the total weight of the path leading to the node.
 private:
  struct NodePriority {
    N node;
    Val priority;  //priority of the node or the total weight of the path leading to the node

    friend bool operator ==(NodePriority& a, NodePriority& b) {
      return (a.node) == (b.node);
    }
    friend bool operator !=(NodePriority& a, NodePriority& b) {
      return (a.node) != (b.node);
    }
    friend bool operator ==(const NodePriority& a, const NodePriority& b) {
      return (a.node) == (b.node);
    }
    friend bool operator !=(const NodePriority& a, const NodePriority& b) {
      return (a.node) != (b.node);
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
  PriorityQueue() {
  }
  //Constructor, to reserve size for future constructing
  PriorityQueue(unsigned reservedsize) {
    nodetracker.reserve(reservedsize);
    std::make_heap(nodetracker.begin(), nodetracker.end(), NodeComparator());
  }
  ;
  //Default destructor, takes no parameters
  virtual ~PriorityQueue() {
  }
  ;
  //Implement a const_iterator for PrioroityQueue container, non-modifiable
  class const_iterator {
   public:
    typedef const_iterator self_type;  //define self_type
    //define iterator_traits, in order to pass into std algorithms
    typedef size_t difference_type;
    typedef NodePriority value_type;
    typedef const NodePriority* const_pointer;
    typedef const NodePriority& const_reference;
    typedef std::random_access_iterator_tag iterator_category;
    typedef typename std::vector<NodePriority>::const_iterator const_realiter;
    const_iterator(const_realiter realiter, size_t pos)
        : _pos(pos),
          _realiter(realiter) {
      _ptr = &(*realiter);
    }
    //overloading operators
    const_reference operator*();
    const_pointer operator->();
    self_type &operator++();  //prefix increment
    self_type operator++(int);  //postfix increment
    self_type &operator--();  //prefix decrement
    self_type operator--(int);  //postfix decrement
    self_type operator+(difference_type);
    self_type &operator+=(difference_type);
    self_type operator-(difference_type);
    self_type &operator-=(difference_type);
    bool operator==(const self_type&) const;
    bool operator!=(const self_type&) const;
   private:
    const_pointer _ptr;
    size_t _pos;
    const_realiter _realiter;
  };
  //Change the priority or the total weight of the path leading to the specified node
  //Input:
  //node: the current end node of a trial path
  //priority: the priority of the end node needs to be changed
  //Output:
  //A boolean variable to indicate if the change of priority is successful.
  //TRUE: change priority of the specified node is successful
  //FALSE: change priority of the specified node is unsuccessful
  bool chgPrioirity(const N& node, Val priority);
  //Return the Node with the minimal priority or the total weight
  //Input: NONE
  //Output: the node associating with the minimal priority or the total weight of a path
  const N minPrioirty();
  //Test if the priority queue contains the specified node
  //Input:
  //node: the inquiring node
  //Output:
  //the boolean value to indicate if the node is contained in the queue or not
  //TRUE: the node is contained in the priority queue
  //FALSE: the node is not contained in the priority queue
  bool contains(const N& node);
  //Insert element into queue
  //Input:
  //node: the node needs to be inserted
  //priority:  the corresponding priority of the node
  //Output: NONE
  void insert(const N& node, Val priority);
  //Insert element into queue with reference of r-value
  //Input:
  //node: the node needs to be inserted
  //priority:  the corresponding priority of the node
  //Output: NONE
  void insert(const N&& node, Val priority);
  //Returns the top element of the queue.
  //Input: NONE
  //Output:
  //Return the Node object with the minimal priority or weight.
  //Calling this function won't remove top node from queue
  const N top();
  //Return the number of queue_elements.
  //Input: NONE
  //Output:
  //The size of current priority queue
  unsigned size();
  const_iterator begin() const;
  const_iterator end() const;
}
;

#include "PriorityQueue.cpp"

#endif /* PRIORITYQUEUE_H_ */
