/*
 * PriorityQueue.cpp
 *
 *  Created on: Dec 26, 2013
 *      Author: renewang
 */

///Return the Node with the minimal priority or the total weight
///@param NONE
///@return the node associating with the minimal priority or the total weight of a path
template<class N, class Val>
const N PriorityQueue<N, Val>::minPrioirty() {
  //removes the top element of the queue.
  const N minnode = top();
  pop_heap(nodetracker.begin(), nodetracker.end(), NodeComparator());
  nodetracker.pop_back();
  return minnode;
}
;
///Test if the priority queue contains the specified node
///@param node the inquiring node
///@return the boolean value to indicate if the node is contained in the queue or not <br/>
///TRUE: the node is contained in the priority queue <br/>
///FALSE: the node is not contained in the priority queue <br/>
template<class N, class Val>
bool PriorityQueue<N, Val>::contains(const N& node) {
  NodePriority newqelement;
  newqelement.node = node;
  bool iscontained = findElement(newqelement) != nodetracker.end();
  return iscontained;
}
;
///Insert element into queue
///@param node is the node needs to be inserted
///@param priority is the corresponding priority of the node
///@return NONE
template<class N, class Val>
void PriorityQueue<N, Val>::insert(const N& node, Val priority) {
  NodePriority qelement;
  qelement.node = node;
  qelement.priority = priority;
  nodetracker.push_back(qelement);
  std::push_heap(nodetracker.begin(), nodetracker.end(), NodeComparator());
}
///Insert element into queue with reference of r-value
///@param node is the node needs to be inserted
///@param priority is the corresponding priority of the node
//Output: NONE
#if __cplusplus > 199711L
template<class N, class Val>
void PriorityQueue<N, Val>::insert(const N&& node, Val priority) {
  NodePriority qelement;
  qelement.node = move(node);
  qelement.priority = priority;
  nodetracker.push_back(qelement);
  std::push_heap(nodetracker.begin(), nodetracker.end(), NodeComparator());
}
#endif
///Returns the top element of the queue.
///@param NONE
///@return Return the Node object with the minimal priority or weight.
//Calling this function won't remove top node from queue
template<class N, class Val>
const N PriorityQueue<N, Val>::top() {
  NodePriority topnode = nodetracker.front();
  return topnode.node;
}
///Return the number of queue_elements.
///@param NONE
///@return NONE
//The size of current priority queue
template<class N, class Val>
unsigned PriorityQueue<N, Val>::size() {
  return nodetracker.size();
}
///Change the priority or the total weight of the path leading to the specified node
///@param node is the current end node of a trial path
///@param priority is the priority of the end node needs to be changed
///@return A boolean variable to indicate if the change of priority is successful.<br/>
///TRUE: change priority of the specified node is successful <br/>
///FALSE: change priority of the specified node is unsuccessful <br/>
template<class N, class Val>
bool PriorityQueue<N, Val>::chgPrioirity(const N& node, Val priority) {
  bool issuccess = false;
  NodePriority newqelement;
  newqelement.node = node;
  newqelement.priority = priority;
  typename std::vector<NodePriority>::iterator chgiter = findElement(
      newqelement);
  if (chgiter != nodetracker.end()) {
    chgiter->priority = priority;
    std::make_heap(nodetracker.begin(), nodetracker.end(), NodeComparator());
    issuccess = true;
  }
  return issuccess;
}
///Return the const_iterator at the begin position
///@param NONE
///@return return const_iterator at the begin position
template<class N, class Val>
typename PriorityQueue<N, Val>::const_iterator PriorityQueue<N, Val>::begin() const {
  return const_iterator(nodetracker.begin(), 0);
}
///Return the const_iterator at the end position
///@param NONE
///@return return const_iterator at the end position
template<class N, class Val>
typename PriorityQueue<N, Val>::const_iterator PriorityQueue<N, Val>::end() const {
  return const_iterator(nodetracker.end(), nodetracker.size());
}
///Overloading operator* for random access iterator
///@param NONE
///@return return const_reference for the iterator
template<class N, class Val>
typename PriorityQueue<N, Val>::const_iterator::const_reference PriorityQueue<N,
    Val>::const_iterator::operator*() {
  return *(_ptr);
}
///Overloading operator* for random access iterator
///@param NONE
///@return return const_reference for the iterator
template<class N, class Val>
typename PriorityQueue<N, Val>::const_iterator::const_pointer PriorityQueue<N,
    Val>::const_iterator::operator->() {
  return _ptr;
}
///Prefix increment
///@param NONE
///@return return this
template<class N, class Val>
typename PriorityQueue<N, Val>::const_iterator::self_type& PriorityQueue<N, Val>::const_iterator::operator++() {
  ++_realiter;
  _ptr = &(*_realiter);
  _pos++;
  return *this;
}
///Postfix increment
///@param NONE
///@param int is a never used parameter for the purpose to distinguish from prefix decrement
///@return return copy of original object
template<class N, class Val>
typename PriorityQueue<N, Val>::const_iterator::self_type PriorityQueue<N, Val>::const_iterator::operator++(
    int) {
  self_type temp(*this);
  ++(*this);
  return temp;
}
///Prefix decrement
///@param NONE
///@return return this
template<class N, class Val>
typename PriorityQueue<N, Val>::const_iterator::self_type& PriorityQueue<N, Val>::const_iterator::operator--() {
  --_realiter;
  _ptr = &(*_realiter);
  _pos--;
  return *this;
}
///Postfix decrement
///@param int is a never used parameter for the purpose to distinguish from prefix decrement
///@return return copy of original object
template<class N, class Val>
typename PriorityQueue<N, Val>::const_iterator::self_type PriorityQueue<N, Val>::const_iterator::operator--(
    int) {
  self_type temp(*this);
  --(*this);
  return temp;
}
/// random access increment
///@param difference_type is the amount of increment added to the current iterator
///@return return copy of original object
template<class N, class Val>
typename PriorityQueue<N, Val>::const_iterator::self_type PriorityQueue<N, Val>::const_iterator::operator+(
    difference_type n) {
  (*this) += n;
  self_type temp(*this);
  return temp;
}
/// random access increment and assignment
///@param difference_type is the amount of increment added to the current iterator
///@return return this
template<class N, class Val>
typename PriorityQueue<N, Val>::const_iterator::self_type& PriorityQueue<N, Val>::const_iterator::operator+=(
    difference_type n) {
  _pos += n;
  _realiter += n;
  _ptr = &(*_realiter);
  return *this;
}
/// random access decrement
///@param difference_type is the amount of decrement subtracted from the current iterator
///@return return copy of original object
template<class N, class Val>
typename PriorityQueue<N, Val>::const_iterator::self_type PriorityQueue<N, Val>::const_iterator::operator-(
    difference_type n) {
  (*this) -= n;
  self_type temp(*this);
  return temp;
}
/// random access decrement
///@param difference_type is the amount of decrement subtracted from the current iterator
///@return return copy of original object
template<class N, class Val>
typename PriorityQueue<N, Val>::const_iterator::self_type& PriorityQueue<N, Val>::const_iterator::operator-=(
    difference_type n) {
  _pos -= n;
  _realiter -= n;
  _ptr = &(*_realiter);
  return *this;
}
/// Overloading == for const_iterator class
///@param the other  const_iterator
///@return return TRUE if equality satisfies; otherwise, return FALSE
template<class N, class Val>
bool PriorityQueue<N, Val>::const_iterator::operator==(
    const self_type &other) const {
  return (*_ptr == *(other._ptr));
}
/// Overloading != for const_iterator class
///@param the other  const_iterator
///@return return FALSE if equality satisfies; otherwise, return TRUE
template<class N, class Val>
bool PriorityQueue<N, Val>::const_iterator::operator!=(
    const self_type &other) const {
  return (*_ptr != *(other._ptr));
}
/// Return top priority (minimal priority) value
///@param NONE
///@return top priority (minimal priority) value
template<class N, class Val>
const Val PriorityQueue<N, Val>::topPriority(){
    NodePriority topnode = nodetracker.front();
    return topnode.priority;
}
/// Test if the queue contains given priority value
///@param priority if the priority will be tested
///@return TRUE when the queue contains given priority value; otherwise, FALSE
template<class N, class Val>
bool PriorityQueue<N, Val>::containsPriority(Val priority) {
  typename std::vector<NodePriority>::iterator iter = nodetracker.begin();
  for (; iter != nodetracker.end(); ++iter) {
    if ((*iter).priority == priority)
      break;
  }
  return iter!=nodetracker.end();
}
/// Test if the queue is empty
///@param NONE
///@return TRUE when the queue is empty; otherwise, FALSE
template<class N, class Val>
bool PriorityQueue<N, Val>::empty() {
  return 0==nodetracker.size();
}

