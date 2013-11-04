/*
 * Graph_test.cpp
 *
 *  Created on: Oct 26, 2013
 *      Author: renewang
 */
#include <iostream>
#include "gtest/gtest.h"
#include "Graph.h"
#include "ShortestPathAlgo.h"
#include "PriorityQueue.h"

using namespace std;
template<typename T>
::testing::AssertionResult ArraysMatch(const vector<T>& expected,
		const vector<T>& actual)
{
	for (size_t i(0); i < expected.size(); ++i)
	{
		if (expected[i] != actual[i])
		{
			return ::testing::AssertionFailure() << "array[" << i << "] ("
					<< actual[i] << ") != expected[" << i << "] ("
					<< expected[i] << ")";
		}
	}

	return ::testing::AssertionSuccess();
}
class GraphTest: public ::testing::Test
{
protected:
	virtual void SetUp()
	{
		//initialize the test matrix
		testPointer = new int*[6];
		for (unsigned i = 0; i < 6; i++)
			testPointer[i] = new int[6];

		for (unsigned k = 0; k < 6; k++)
		{
			for (unsigned i = 0; i < (6 - k); i++)
			{
				testPointer[i][i + k] = k;
				if (k != 0)
					testPointer[i + k][i] = k;
			}
		}
		testAlter = new int*[6];
		for (unsigned i = 0; i < 6; i++)
			testAlter[i] = new int[6];

		for (unsigned k = 0; k < 6; k++)
		{
			for (unsigned i = 0; i < (6 - k); i++)
			{
				testAlter[i][i + k] = k * k;
				if (k != 0)
					testAlter[i + k][i] = k * k;
			}
		}

		testDouble = new double*[6];
		for (unsigned i = 0; i < 6; i++)
			testDouble[i] = new double[6];

		for (unsigned k = 0; k < 6; k++)
		{
			for (unsigned i = 0; i < (6 - k); i++)
			{
				testDouble[i][i + k] = k * k;
				if (k != 0)
					testDouble[i + k][i] = k * k;
			}
		}
	}
	virtual void TearDown()
	{
		if (testPointer != 0)
		{
			for (unsigned i = 0; i < 6; i++)
				delete testPointer[i];
			delete[] testPointer;
		}
		if (testAlter != 0)
		{
			for (unsigned i = 0; i < 6; i++)
				delete testAlter[i];
			delete[] testAlter;
		}
		if (testDouble != 0)
		{
			for (unsigned i = 0; i < 6; i++)
				delete testDouble[i];
			delete[] testDouble;
		}
	}
	int** testPointer;
	int** testAlter;
	double** testDouble;
};
TEST_F(GraphTest,PriorityQCheck)
{
	Graph<string, int> randomG(5, 0.5, 10);
	const vector<Node<string, int> > vertices = randomG.getAllVertices();
	PriorityQueue<string, int> testPQ;

	for (unsigned i = 0; i < vertices.size(); i++)
		testPQ.insert(&(vertices[i]), 10 - i);

	// test PriorityQueue contains
	EXPECT_TRUE(testPQ.contains(&(vertices[vertices.size() - 1])));

	Node<string, int> node;
	node.vertexindex = 100;
	EXPECT_FALSE(testPQ.contains(&node));

	// test PriorityQueue top and minPriority
	Node<string, int>* minNode = const_cast<Node<string, int>*>(testPQ.top());
	EXPECT_EQ(5, minNode->vertexindex);
	for (unsigned i = 0; i < vertices.size(); i++)
	{
		EXPECT_TRUE(testPQ.chgPrioirity(&(vertices[i]), 5 - i));
		minNode = const_cast<Node<string, int>*>(testPQ.top());
		EXPECT_EQ((int )i + 1, minNode->vertexindex);
	}
	minNode = const_cast<Node<string, int>*>(testPQ.minPrioirty());
	EXPECT_EQ(5, minNode->vertexindex);
	minNode = const_cast<Node<string, int>*>(testPQ.top());
	EXPECT_EQ(4, minNode->vertexindex);

}
TEST_F(GraphTest,BasicGraphCheck)
{
	//test ArraysMatch
	vector<int> dummyExpect, dummyFabricate;
	for(unsigned i = 0; i < 10; i++){
		dummyExpect.push_back(i);
		dummyFabricate.push_back(10-i);
	}
	EXPECT_FALSE(ArraysMatch(dummyExpect, dummyFabricate));
	EXPECT_NE(dummyExpect, dummyFabricate);

	Graph<int, int> testG(testPointer, 6);

	//test the getNeighbors for Graph
	vector<int> testNeigh = testG.getNeighbors(1);
	int testExpect[5] =
	{ 2, 3, 4, 5, 6 };
	vector<int> testVec;
	testVec.insert(testVec.begin(), testExpect, testExpect + 4);
	EXPECT_TRUE(ArraysMatch(testVec, testNeigh));
	//ASSERT_THAT(expect, ContainerEq(testNeigh)); //google mock

	//test the isAdjacent for Graph
	EXPECT_TRUE(testG.isAdjacent(1, 2));
	EXPECT_TRUE(testG.isAdjacent(2, 1));
	EXPECT_FALSE(testG.isAdjacent(1, 1));

	//case 1: test the Dijkstra algorithm, for a complete graph
	ShortestPathAlgo<int, int> algo(testG);
	list< Node<int, int> > shortestPath = algo.path(1, 6);
	int pathExpect[2] =
	{ 1, 6 };
	vector<int> shortestPathExpect;
	shortestPathExpect.insert(shortestPathExpect.begin(), pathExpect,
			pathExpect + 2);
	vector<int> shortestPathActual;
	for (list< Node<int, int> >::iterator iter = shortestPath.begin();
			iter != shortestPath.end(); ++iter)
	{
		shortestPathActual.push_back((*iter).vertexindex);
	}
	EXPECT_TRUE(ArraysMatch(shortestPathExpect, shortestPathActual));
	EXPECT_EQ(5, algo.path_size(1, 6));

	/*
	 * additional test to check if the trace back is working (the sum of shortest path list needs to equal to min distance)
	 * also test the getEdgeValue for Graph
	 */

	int pathSize = 0;
	list< Node<int, int> >::iterator iter = shortestPath.begin();
	int from = (*iter).vertexindex;
	++iter;
	for (; iter != shortestPath.end(); ++iter)
	{
		pathSize += testG.getEdgeValue(from, (*iter).vertexindex);
		from = (*iter).vertexindex;
	}
	EXPECT_EQ(5, pathSize);

	//test the average shortest path for a given Graph
	EXPECT_FLOAT_EQ(35.0 / 15.0, algo.averageAllPathSize());
	EXPECT_FLOAT_EQ(15.0 / 5.0, algo.averagePathSize(1));

	//test the deleteEdge and addEdge for a Graph
	testG.deleteEdge(1, 2);
	EXPECT_FALSE(testG.isAdjacent(1, 2));
	testG.addEdge(1, 2, 1);
	EXPECT_TRUE(testG.isAdjacent(1, 2));

	//case 2: modify the Graph and recalculate the shortest path, by setting one of path in the previous shortest path with a extremely large value
	testG.setEdgeValue(1, 6, 10);
	EXPECT_EQ(10, testG.getEdgeValue(1, 6));
	algo.clean();
	shortestPath = algo.path(1, 6);
	shortestPathActual.clear();
	shortestPathExpect.clear();
	int pathExpect2[3] =
	{ 1, 2, 6 };
	shortestPathExpect.insert(shortestPathExpect.begin(), pathExpect2,
			pathExpect2 + 3);
	for ( list<Node<int, int> >::iterator iter = shortestPath.begin();
			iter != shortestPath.end(); ++iter)
	{
		shortestPathActual.push_back((*iter).vertexindex);
	}
	EXPECT_EQ(shortestPathExpect, shortestPathActual);
	EXPECT_EQ(5, algo.path_size(1, 6));

	//test the average shortest path for a given Graph
	EXPECT_FLOAT_EQ(35.0 / 15.0, algo.averageAllPathSize());
	EXPECT_FLOAT_EQ(15.0 / 5.0, algo.averagePathSize(1));

	//case 3: modify the Graph and recalculate the shortest path, by delete one of the path in the previous shortest path
	testG.deleteEdge(1, 2);
	algo.clean();
	shortestPath = algo.path(1, 6);
	shortestPathActual.clear();
	shortestPathExpect.clear();
	int pathExpect3[3] =
	{ 1, 3, 6 };
	shortestPathExpect.insert(shortestPathExpect.begin(), pathExpect3,
			pathExpect3 + 3);
	for (list< Node<int, int> >::iterator iter = shortestPath.begin();
			iter != shortestPath.end(); ++iter)
	{
		shortestPathActual.push_back((*iter).vertexindex);
	}
	EXPECT_TRUE(ArraysMatch(shortestPathExpect, shortestPathActual));
	EXPECT_EQ(5, algo.path_size(1, 6));
	//test the average shortest path for a given Graph
	EXPECT_FLOAT_EQ(37.0 / 15.0, algo.averageAllPathSize());
	EXPECT_FLOAT_EQ(17.0 / 5.0, algo.averagePathSize(1));

	double expectAveFromNode[] =
	{ 19.0, 21.0, 23.0 };
	//case 4: delete the edges for 2 until it is isolated
	for (int i = 3; i < testG.getSizeOfVertices(); i++)
	{
		testG.deleteEdge(2, i);
		algo.clean();

		shortestPath = algo.path(1, 6);
		shortestPathActual.clear();
		shortestPathExpect.clear();
		shortestPathExpect.insert(shortestPathExpect.begin(), pathExpect3,
				pathExpect3 + 3);
		for (list< Node<int, int> >::iterator iter = shortestPath.begin();
				iter != shortestPath.end(); ++iter)
		{
			shortestPathActual.push_back((*iter).vertexindex);
		}
		EXPECT_TRUE( ArraysMatch(shortestPathExpect, shortestPathActual));
		EXPECT_EQ(5, algo.path_size(1, 6));
		//test the average shortest path for a given Graph
		EXPECT_FLOAT_EQ(expectAveFromNode[i - 3] / 5.0,
				algo.averagePathSize(1));
	}

	testG.deleteEdge(2, 6);
	algo.clean();
	shortestPath = algo.path(1, 6);
	shortestPathActual.clear();
	shortestPathExpect.clear();
	shortestPathExpect.insert(shortestPathExpect.begin(), pathExpect3,
			pathExpect3 + 3);
	for (list< Node<int, int> >::iterator iter = shortestPath.begin();
			iter != shortestPath.end(); ++iter)
	{
		shortestPathActual.push_back((*iter).vertexindex);
	}
	EXPECT_TRUE( ArraysMatch(shortestPathExpect, shortestPathActual));
	EXPECT_EQ(5, algo.path_size(1, 6));
	EXPECT_EQ(0, algo.path_size(1, 2));
	shortestPath = algo.path(1, 2);
	EXPECT_EQ(1, shortestPath.size());
	EXPECT_FLOAT_EQ(14.0 / 4.0, algo.averagePathSize(1));

	EXPECT_TRUE(testG.printRepGraph());
}
TEST_F(GraphTest, AlternativeGraphCheck)
{
	Graph<int, int> testG(testAlter, 6);
	ShortestPathAlgo<int, int> algo(testG);
	list< Node<int, int> > shortestPath = algo.path(1, 6);
	int pathExpect[6] =
	{ 1, 2, 3, 4, 5, 6 };
	vector<int> shortestPathExpect;
	shortestPathExpect.insert(shortestPathExpect.begin(), pathExpect,
			pathExpect + 6);
	vector<int> shortestPathActual;
	for (list< Node<int, int> >::iterator iter = shortestPath.begin();
			iter != shortestPath.end(); ++iter)
	{
		shortestPathActual.push_back((*iter).vertexindex);
	}
	EXPECT_TRUE(ArraysMatch(shortestPathExpect, shortestPathActual));
	EXPECT_EQ(5, algo.path_size(1, 6));

	Graph<string, double> testDG(testDouble, 6);
	ShortestPathAlgo<string, double> algoD(testDG);
	list< Node<string, double> > shortestPathD = algoD.path(1, 6);

	shortestPathActual.clear();
	for (list< Node<string, double> >::iterator iter = shortestPathD.begin();
			iter != shortestPathD.end(); ++iter)
	{
		shortestPathActual.push_back((*iter).vertexindex);
	}
	EXPECT_TRUE( ArraysMatch(shortestPathExpect, shortestPathActual));
	EXPECT_FLOAT_EQ(5.0, algoD.path_size(1, 6));
}

TEST_F(GraphTest,RandomGraphCheck)
{
	Graph<string, int> randomG(50, 0.7, 10);
	ASSERT_EQ(50, randomG.getSizeOfVertices());

	const vector<Node<string, int> > nodes = randomG.getAllVertices();
	int sumOfNeighbors = 0;
	for (unsigned i = 0; i < nodes.size(); i++)
		sumOfNeighbors += nodes[i].numofneighbors;

	EXPECT_EQ(sumOfNeighbors / 2, randomG.getSizeOfEdges());
	ShortestPathAlgo<string, int> algo(randomG);
	EXPECT_TRUE(algo.averageAllPathSize() > 0);

	for (unsigned i = 0; i < 10; i++)
	{
		Graph<string, int> randomG2(50, 0.4, 10);
		ShortestPathAlgo<string, int> algo2(randomG2);
		EXPECT_TRUE(algo2.averagePathSize(1) > 0);
		/*
		 * additional test to check if the trace back is working (the sum of shortest path list needs to equal to min distance)
		 * also test the getEdgeValue for Graph
		 */
		list< Node<string, int> > shortestPath = algo2.path(1, 50);
		int pathSize = 0;
		list< Node<string, int> >::iterator iter = shortestPath.begin();
		int from = (*iter).vertexindex;
		++iter;
		for (; iter != shortestPath.end(); ++iter)
		{
			pathSize += randomG2.getEdgeValue(from, (*iter).vertexindex);
			from = (*iter).vertexindex;
		}
		EXPECT_EQ(pathSize, algo2.path_size(1, 50));
	}
}
int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
