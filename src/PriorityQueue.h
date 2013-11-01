/*
 * PriorityQueue.h
 *
 *  Created on: Nov 1, 2013
 *      Author: renewang
 */

#ifndef PRIORITYQUEUE_H_
#define PRIORITYQUEUE_H_

class PriorityQueue
{
public:
	PriorityQueue();
	virtual ~PriorityQueue();
	chgPrioirity(priority); //changes the priority (node value) of queue element.
	minPrioirty(); //removes the top element of the queue.
	bool contains(PQ, queue_element); //does the queue contain queue_element.
	void Insert(queue_element); //insert queue_element into queue
	top(PQ); //returns the top element of the queue.
	unsigned size(); //return the number of queue_elements.
};

#endif /* PRIORITYQUEUE_H_ */
