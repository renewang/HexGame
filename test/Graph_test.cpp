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
	}
	virtual void TearDown()
	{
		if (testPointer != 0)
		{
			for (unsigned i = 0; i < 6; i++)
				delete testPointer[i];
			delete[] testPointer;
		}
	}
	int** testPointer;
};
TEST_F(GraphTest,RandomGraphCheck)
{
	Graph<string, int>* randomG = new Graph<string, int>(50, 0.5, 10);
	ASSERT_EQ(50, randomG->getSizeOfVertices());

	const vector<Node<string, int>*>* nodes = randomG->getAllVertices();
	int sumOfNeighbors = 0;
	for (unsigned i = 0; i < nodes->size(); i++)
		sumOfNeighbors += (nodes->at(i))->numOfNeighbors;

	EXPECT_EQ(sumOfNeighbors/2, randomG->getSizeOfEdges());

	if (randomG != 0)
		delete randomG;
}
TEST_F(GraphTest,BasicGraphCheck)
{
	Graph<int, int>* testG = new Graph<int, int>(testPointer, 6);

	vector<int> testNeigh = testG->getNeighbors(1);
	int testExpect[5] =
	{ 2, 3, 4, 5, 6 };
	vector<int> testVec;
	testVec.insert(testVec.begin(), testExpect, testExpect + 4);
	EXPECT_TRUE(ArraysMatch(testVec, testNeigh));
	//ASSERT_THAT(expect, ContainerEq(testNeigh)); //google mock*/

	EXPECT_TRUE(testG->isAdjacent(1, 2));
	EXPECT_TRUE(testG->isAdjacent(2, 1));
	EXPECT_FALSE(testG->isAdjacent(1, 1));

	ShortestPathAlgo<int, int>* algo = new ShortestPathAlgo<int, int>(testG);
	list<Node<int, int>*>* shortestPath = algo->path(1, 6);

	EXPECT_EQ(5, algo->path_size(1, 6));

	int pathSize = 0;
	list<Node<int, int>*>::iterator iter = shortestPath->begin();
	int from = (*iter)->vertexIdex;
	++iter;
	for (; iter != shortestPath->end(); ++iter)
	{
		pathSize += testG->getEdgeValue(from, (*iter)->vertexIdex);
		from = (*iter)->vertexIdex;
	}
	EXPECT_EQ(5, pathSize);
	EXPECT_FLOAT_EQ(35.0 / 15.0, algo->averagePathSize());

	testG->deleteEdge(1, 2);
	EXPECT_FALSE(testG->isAdjacent(1, 2));
	testG->addEdge(1, 2, 1);
	EXPECT_TRUE(testG->isAdjacent(1, 2));
	testG->setEdgeValue(1, 2, 10);
	EXPECT_EQ(10, testG->getEdgeValue(1, 2));

	delete shortestPath;
	delete testG;
}
void PrintOutMatrix(int** matrix, unsigned row, unsigned col)
{
	cout << setw(9) << " ";
	for (unsigned i = 0; i < col; i++)
		cout << i + 1 << setw(5) << " ";
	cout << "\n";
	for (unsigned i = 0; i < row; i++)
	{
		cout << setw(5) << i + 1;
		for (unsigned j = 0; j < col; j++)
		{
			cout << setw(5) << matrix[i][j] << " ";
		}
		cout << "\n";
	}
}
int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
