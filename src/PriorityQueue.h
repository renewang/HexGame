/*
 * PriorityQueue.h
 *
 */

#ifndef PRIORITYQUEUE_H_
#define PRIORITYQUEUE_H_

#include "Graph.h"
#include <queue>
#include <deque>
#include <algorithm>

using namespace std;

template<class Type, class Val>
class PriorityQueue
{
	typedef Node<Type, Val> Node;

private:
	struct NodePriority
	{
		const Node* node;
		Val priority;

		friend bool operator ==(NodePriority& a, NodePriority& b)
		{
			return (a.node)->vertexIdex == (b.node)->vertexIdex;
		}
	};
	struct MyComparator
	{
		bool operator()(NodePriority& a, NodePriority& b)
		{
			return a.priority > b.priority; //calls your operator
		}
	};
	/*
	 * find the corresponding element in tracker
	 */
	typename vector<NodePriority>::iterator findElement(NodePriority& qElement)
	{
		typename vector<NodePriority>::iterator iter = nodeTracker.begin();

		for (; iter != nodeTracker.end(); ++iter)
		{
			if ((*iter) == qElement)
				break;
		}
		return iter;
	}
	;
	vector<NodePriority> nodeTracker;

public:
	PriorityQueue()
	{
	}
	;
	virtual ~PriorityQueue()
	{
		nodeTracker.clear();
	}
	;
	bool chgPrioirity(const Node* node, Val priority)
	{
		//changes the priority (node value) of queue element.
		bool isSuccess = false;
		if (contains(node))
		{
			NodePriority newqElement;
			newqElement.node = node;
			newqElement.priority = priority;
			typename vector<NodePriority>::iterator chgIter = findElement(
					newqElement);
			if (chgIter != nodeTracker.end())
			{
				chgIter->priority = priority;
				make_heap(nodeTracker.begin(), nodeTracker.end(),
						MyComparator());
				isSuccess = true;
			}
		}
		return isSuccess;
	}
	;
	const Node* minPrioirty()
	{
		//removes the top element of the queue.
		const Node* minNode = top();
		pop_heap(nodeTracker.begin(), nodeTracker.end(), MyComparator());
		nodeTracker.pop_back();
		return minNode;
	}
	;
	bool contains(const Node* node)
	{
		//does the queue contain queue_element.
		NodePriority newqElement;
		newqElement.node = node;
		bool isContained = findElement(newqElement) != nodeTracker.end();
		return isContained;
	}
	;
	void insert(const Node* node, Val priority)
	{
		//insert queue_element into queue
		NodePriority qElement;
		qElement.node = node;
		qElement.priority = priority;
		nodeTracker.push_back(qElement);
	}
	;
	const Node* top()
	{
		//returns the top element of the queue.
		make_heap(nodeTracker.begin(), nodeTracker.end(), MyComparator());
		NodePriority topNode = nodeTracker.front();
		return topNode.node;
	}
	;
	unsigned size()
	{
		//return the number of queue_elements.
		return nodeTracker.size();
	}
	;
};

#endif /* PRIORITYQUEUE_H_ */
