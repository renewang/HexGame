//============================================================================
// Name        : Graph_test.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Google Test for Dijkstra and basic graph unit tests
//============================================================================
#include <map>
#include <sstream>
#include <iostream>
#include "gtest/gtest.h"
#include "Graph.h"
#include "ShortestPathAlgo.h"
#include "PriorityQueue.h"

using namespace std;
template<typename T>
::testing::AssertionResult ArraysMatch(const vector<T>& expected,
                                       const vector<T>& actual) {
  for (size_t i(0); i < expected.size(); ++i) {
    if (expected[i] != actual[i]) {
      return ::testing::AssertionFailure() << "array[" << i << "] ("
          << actual[i] << ") != expected[" << i << "] (" << expected[i] << ")";
    }
  }

  return ::testing::AssertionSuccess();
}
class GraphTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    //initialize the test matrix
    testPointer = new int*[6];
    for (unsigned i = 0; i < 6; i++)
      testPointer[i] = new int[6];

    for (unsigned k = 0; k < 6; k++) {
      for (unsigned i = 0; i < (6 - k); i++) {
        testPointer[i][i + k] = k;
        if (k != 0)
          testPointer[i + k][i] = k;
      }
    }
    testAlter = new int*[6];
    for (unsigned i = 0; i < 6; i++)
      testAlter[i] = new int[6];

    for (unsigned k = 0; k < 6; k++) {
      for (unsigned i = 0; i < (6 - k); i++) {
        testAlter[i][i + k] = k * k;
        if (k != 0)
          testAlter[i + k][i] = k * k;
      }
    }

    testDouble = new double*[6];
    for (unsigned i = 0; i < 6; i++)
      testDouble[i] = new double[6];

    for (unsigned k = 0; k < 6; k++) {
      for (unsigned i = 0; i < (6 - k); i++) {
        testDouble[i][i + k] = k * k;
        if (k != 0)
          testDouble[i + k][i] = k * k;
      }
    }
  }
  virtual void TearDown() {
    if (testPointer != 0) {
      for (unsigned i = 0; i < 6; i++)
        delete testPointer[i];
      delete[] testPointer;
    }
    if (testAlter != 0) {
      for (unsigned i = 0; i < 6; i++)
        delete testAlter[i];
      delete[] testAlter;
    }
    if (testDouble != 0) {
      for (unsigned i = 0; i < 6; i++)
        delete testDouble[i];
      delete[] testDouble;
    }
  }
  int** testPointer;
  int** testAlter;
  double** testDouble;
};
TEST_F(GraphTest,PriorityQCheck) {
  Graph<string, int> randomG(5, 0.5, 10);
  vector<int> vertices(randomG.getSizeOfVertices());

  for (unsigned i = 1; i <= vertices.size(); i++)
    vertices[i - 1] = i;

  PriorityQueue<int, int> testPQ(vertices.size());

  for (unsigned i = 0; i < vertices.size(); i++)
    testPQ.insert(vertices[i], 10 - i);

  // test PriorityQueue contains
  EXPECT_TRUE(testPQ.contains(vertices[vertices.size() - 1]));
  EXPECT_FALSE(testPQ.contains(100));

  // test PriorityQueue top and minPriority
  int minNode = testPQ.top();
  EXPECT_EQ(5, minNode);
  for (unsigned i = 0; i < vertices.size(); i++) {
    EXPECT_TRUE(testPQ.chgPrioirity(vertices[i], 5 - i));
    minNode = testPQ.top();
    EXPECT_EQ((int )i + 1, minNode);
  }
  minNode = testPQ.minPrioirty();
  EXPECT_EQ(5, minNode);
  minNode = testPQ.top();
  EXPECT_EQ(4, minNode);

}
TEST_F(GraphTest,BasicGraphCheck) {
  //test ArraysMatch
  vector<int> dummyExpect, dummyFabricate;
  for (unsigned i = 0; i < 10; i++) {
    dummyExpect.push_back(i);
    dummyFabricate.push_back(10 - i);
  }
  EXPECT_FALSE(ArraysMatch(dummyExpect, dummyFabricate));
  EXPECT_NE(dummyExpect, dummyFabricate);

  Graph<int, int> testG(testPointer, 6);

  //test the getNeighbors for Graph
  vector<int> testNeigh = testG.getNeighbors(1);
  int testExpect[5] = { 2, 3, 4, 5, 6 };
  vector<int> testVec;
  testVec.insert(testVec.begin(), testExpect, testExpect + 4);
  EXPECT_TRUE(ArraysMatch(testVec, testNeigh));

  //test the isAdjacent for Graph
  EXPECT_TRUE(testG.isAdjacent(1, 2));
  EXPECT_TRUE(testG.isAdjacent(2, 1));
  EXPECT_FALSE(testG.isAdjacent(1, 1));

  //case 1: test the Dijkstra algorithm, for a complete graph
  ShortestPathAlgo<int, int> algo(testG);
  list<int> shortestPath = algo.path(1, 6);
  int pathExpect[2] = { 1, 6 };
  vector<int> shortestPathExpect;
  shortestPathExpect.insert(shortestPathExpect.begin(), pathExpect,
                            pathExpect + 2);
  vector<int> shortestPathActual(shortestPath.begin(), shortestPath.end());
  EXPECT_TRUE(ArraysMatch(shortestPathExpect, shortestPathActual));
  EXPECT_EQ(5, algo.path_size(1, 6));

  /*
   * additional test to check if the trace back is working (the sum of shortest path list needs to equal to min distance)
   * also test the getEdgeValue for Graph
   */

  int pathSize = 0;
  list<int>::iterator iter = shortestPath.begin();
  int from = *iter;
  ++iter;
  for (; iter != shortestPath.end(); ++iter) {
    pathSize += testG.getEdgeValue(from, *iter);
    from = *iter;
  }
  EXPECT_EQ(5, pathSize);

  //test the average shortest path for a given Graph
  EXPECT_FLOAT_EQ(float(35.0 / 15.0),
                  static_cast<float>(algo.averageAllPathSize()));
  EXPECT_FLOAT_EQ((float )(15.0 / 5.0),
                  static_cast<float>(algo.averagePathSize(1)));

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
  int pathExpect2[3] = { 1, 2, 6 };
  shortestPathExpect.insert(shortestPathExpect.begin(), pathExpect2,
                            pathExpect2 + 3);
  for (list<int>::iterator iter = shortestPath.begin();
      iter != shortestPath.end(); ++iter) {
    shortestPathActual.push_back(*iter);
  }
  EXPECT_EQ(shortestPathExpect, shortestPathActual);
  EXPECT_EQ(5, algo.path_size(1, 6));

  //test the average shortest path for a given Graph
  EXPECT_FLOAT_EQ((float )(35.0 / 15.0),
                  static_cast<float>(algo.averageAllPathSize()));
  EXPECT_FLOAT_EQ((float )(15.0 / 5.0),
                  static_cast<float>(algo.averagePathSize(1)));

  //case 3: modify the Graph and recalculate the shortest path, by delete one of the path in the previous shortest path
  testG.deleteEdge(1, 2);
  algo.clean();
  shortestPath = algo.path(1, 6);
  shortestPathActual.clear();
  shortestPathExpect.clear();
  int pathExpect3[3] = { 1, 3, 6 };
  shortestPathExpect.insert(shortestPathExpect.begin(), pathExpect3,
                            pathExpect3 + 3);
  for (list<int>::iterator iter = shortestPath.begin();
      iter != shortestPath.end(); ++iter) {
    shortestPathActual.push_back(*iter);
  }
  EXPECT_TRUE(ArraysMatch(shortestPathExpect, shortestPathActual));
  EXPECT_EQ(5, algo.path_size(1, 6));
  //test the average shortest path for a given Graph
  EXPECT_FLOAT_EQ((float )(37.0 / 15.0),
                  static_cast<float>(algo.averageAllPathSize()));
  EXPECT_FLOAT_EQ((float )(17.0 / 5.0),
                  static_cast<float>(algo.averagePathSize(1)));

  float expectAveFromNode[] = { 19.0, 21.0, 23.0 };
  //case 4: delete the edges for 2 until it is isolated
  for (int i = 3; i < testG.getSizeOfVertices(); i++) {
    testG.deleteEdge(2, i);
    algo.clean();

    shortestPath = algo.path(1, 6);
    shortestPathActual.clear();
    shortestPathExpect.clear();
    shortestPathExpect.insert(shortestPathExpect.begin(), pathExpect3,
                              pathExpect3 + 3);
    for (list<int>::iterator iter = shortestPath.begin();
        iter != shortestPath.end(); ++iter) {
      shortestPathActual.push_back(*iter);
    }
    EXPECT_TRUE(ArraysMatch(shortestPathExpect, shortestPathActual));
    EXPECT_EQ(5, algo.path_size(1, 6));
    //test the average shortest path for a given Graph
    EXPECT_FLOAT_EQ(expectAveFromNode[i - 3] / (float )5.0,
                    static_cast<float>(algo.averagePathSize(1)));
  }

  testG.deleteEdge(2, 6);
  algo.clean();
  shortestPath = algo.path(1, 6);
  shortestPathActual.clear();
  shortestPathExpect.clear();
  shortestPathExpect.insert(shortestPathExpect.begin(), pathExpect3,
                            pathExpect3 + 3);
  for (list<int>::iterator iter = shortestPath.begin();
      iter != shortestPath.end(); ++iter) {
    shortestPathActual.push_back(*iter);
  }
  EXPECT_TRUE(ArraysMatch(shortestPathExpect, shortestPathActual));
  EXPECT_EQ(5, algo.path_size(1, 6));
  EXPECT_EQ(0, algo.path_size(1, 2));
  shortestPath = algo.path(1, 2);
  EXPECT_EQ(1, static_cast<int>(shortestPath.size()));
  EXPECT_FLOAT_EQ(14.0 / 4.0, static_cast<float>(algo.averagePathSize(1)));
}
TEST_F(GraphTest, AlternativeGraphCheck) {
  Graph<int, int> testG(testAlter, 6);
  ShortestPathAlgo<int, int> algo(testG);
  list<int> shortestPath = algo.path(1, 6);
  int pathExpect[6] = { 1, 2, 3, 4, 5, 6 };
  vector<int> shortestPathExpect;
  shortestPathExpect.insert(shortestPathExpect.begin(), pathExpect,
                            pathExpect + 6);
  vector<int> shortestPathActual;
  for (list<int>::iterator iter = shortestPath.begin();
      iter != shortestPath.end(); ++iter) {
    shortestPathActual.push_back(*iter);
  }
  EXPECT_TRUE(ArraysMatch(shortestPathExpect, shortestPathActual));
  EXPECT_EQ(5, algo.path_size(1, 6));

  Graph<string, double> testDG(testDouble, 6);
  ShortestPathAlgo<string, double> algoD(testDG);
  list<int> shortestPathD = algoD.path(1, 6);

  shortestPathActual.clear();
  for (list<int>::iterator iter = shortestPathD.begin();
      iter != shortestPathD.end(); ++iter) {
    shortestPathActual.push_back(*iter);
  }
  EXPECT_TRUE(ArraysMatch(shortestPathExpect, shortestPathActual));
  EXPECT_FLOAT_EQ(5.0, static_cast<float>(algoD.path_size(1, 6)));
}

TEST_F(GraphTest,RandomGraphCheck) {
  Graph<string, int> randomG(50, (float) 0.7, 10);
  ASSERT_EQ(50, randomG.getSizeOfVertices());

  vector<int> nodes(randomG.getSizeOfVertices());
  for (unsigned i = 0; i < nodes.size(); i++)
    nodes[i] = (i + 1);

  int sumOfNeighbors = 0;
  for (unsigned i = 0; i < nodes.size(); i++)
    sumOfNeighbors += randomG.getNeighborsSize(nodes[i]);

  EXPECT_EQ(sumOfNeighbors / 2, randomG.getSizeOfEdges());
  ShortestPathAlgo<string, int> algo(randomG);
  EXPECT_TRUE(algo.averageAllPathSize() > 0);

  for (unsigned i = 0; i < 10; i++) {
    Graph<string, int> randomG2(50, (float) 0.4, 10);
    ShortestPathAlgo<string, int> algo2(randomG2);
    EXPECT_TRUE(algo2.averagePathSize(1) > 0);
    /*
     * additional test to check if the trace back is working (the sum of shortest path list needs to equal to min distance)
     * also test the getEdgeValue for Graph
     */
    list<int> shortestPath = algo2.path(1, 50);
    int pathSize = 0;
    list<int>::iterator iter = shortestPath.begin();
    int from = *iter;
    ++iter;
    for (; iter != shortestPath.end(); ++iter) {
      pathSize += randomG2.getEdgeValue(from, *iter);
      from = *iter;
    }
    EXPECT_EQ(pathSize, algo2.path_size(1, 50));
  }
}
TEST_F(GraphTest, MSTGraphCheck) {
  //test the empty graph
  unsigned sizeoftestgraph = 6;
  Graph<int, int> emptyG(sizeoftestgraph);
  for (unsigned i = 1; i <= sizeoftestgraph; i++) {
    for (unsigned j = 1; j <= sizeoftestgraph; j++) {
      if (i != j)
        EXPECT_FALSE(emptyG.isAdjacent(i, j));
    }
  }
  //test the PriorityQueue with getAllEdgesValues()
  Graph<int, int> testG(testAlter, 6);
  //key as "(index of from node-1)"x"size of vertices" + "(index of to node-1)"
  //value as the edge value
  map<int, int> mapAllEdges = testG.getAllEdgesValues();
  ASSERT_EQ(testG.getSizeOfEdges(), static_cast<int>(mapAllEdges.size()));
  PriorityQueue<int, int> edgesQueue(mapAllEdges.size());
  for (map<int, int>::iterator iter = mapAllEdges.begin();
      iter != mapAllEdges.end(); ++iter) {
    edgesQueue.insert((*iter).first, (*iter).second);
  }
  for (unsigned k = 1; k < sizeoftestgraph; k++) {
    for (unsigned i = (sizeoftestgraph - k); i > 0; i--) {
      //from the smallest value = k*k
      int minNode = edgesQueue.minPrioirty();
      int row = minNode / sizeoftestgraph;
      int col = minNode % sizeoftestgraph;
      ASSERT_EQ(static_cast<int>(k), (col - row));
      EXPECT_EQ(static_cast<int>(k * k), testG.getEdgeValue(row + 1, col + 1));
    }
  }

  //test the directed graph
  emptyG.setIsundirected(false);
  emptyG.addEdge(1, 2, 1);
  emptyG.addEdge(1, 3, 1);

  ASSERT_TRUE(emptyG.isAdjacent(1, 2));
  ASSERT_FALSE(emptyG.isAdjacent(2, 1));
  vector<int> testNeigh = emptyG.getNeighbors(1);
  int testExpect[2] = { 2, 3 };
  vector<int> testVec;
  testVec.insert(testVec.begin(), testExpect, testExpect + 2);
  EXPECT_EQ(testVec, testNeigh);

  emptyG.addEdge(2, 4, 1);
  emptyG.addEdge(2, 5, 1);

  testNeigh.clear();
  testNeigh = emptyG.getNeighbors(2);
  int testExpect2[2] = { 4, 5 };
  testVec.clear();
  testVec.insert(testVec.begin(), testExpect2, testExpect2 + 2);

  emptyG.addEdge(3, 6, 1);
  testNeigh.clear();
  testNeigh = emptyG.getNeighbors(3);
  int testExpect3[1] = { 6 };
  testVec.clear();
  testVec.insert(testVec.begin(), testExpect3, testExpect3 + 1);

  EXPECT_FALSE(emptyG.isLoopExisting(1));

  emptyG.addEdge(3, 5, 4);
  EXPECT_TRUE(emptyG.isLoopExisting(1));

  //test with undirected graph
  for (unsigned i = 0; i < sizeoftestgraph; i++)
    EXPECT_TRUE(testG.isLoopExisting(i + 1))
        << " test loop true for undirected graph " << (i + 1);

  testG.deleteEdge(1, 4);
  testG.deleteEdge(1, 5);
  testG.deleteEdge(1, 6);
  testG.deleteEdge(2, 3);
  testG.deleteEdge(2, 6);
  testG.deleteEdge(3, 4);
  testG.deleteEdge(3, 5);
  testG.deleteEdge(4, 5);
  testG.deleteEdge(4, 6);
  testG.deleteEdge(5, 6);

  for (unsigned i = 0; i < sizeoftestgraph; i++)
    EXPECT_FALSE(testG.isLoopExisting(i + 1))
        << " test loop false for undirected graph " << (i + 1);

}
TEST_F(GraphTest,PriorityQIterCheck) {
  vector<int> vertices(10);
  for (unsigned i = 1; i <= 10; i++)
    vertices[i - 1] = i;

  PriorityQueue<int, int> testPQ(vertices.size());

  for (unsigned i = 0; i < vertices.size(); i++)
    testPQ.insert(vertices[i], 10 - i);

  PriorityQueue<int, int>::const_iterator iterator = testPQ.begin();
  EXPECT_EQ(10, (*iterator).node);
  EXPECT_EQ(1, (*iterator).priority);
  iterator++;
  EXPECT_EQ(9, (*iterator).node);
  EXPECT_EQ(2, (*iterator).priority);
  iterator--;
  EXPECT_EQ(10, (*iterator).node);
  EXPECT_EQ(1, (*iterator).priority);
  PriorityQueue<int, int>::const_iterator terminal = ++testPQ.begin();
  EXPECT_FALSE(terminal == iterator);
  EXPECT_TRUE(terminal != iterator);
  EXPECT_EQ(9, (*(iterator + 1)).node);
  EXPECT_EQ(10, ((iterator - 1)->node));
}
TEST_F(GraphTest,PriorityQRValueCheck) {
  int size = 5;
  PriorityQueue<int, int> testPQ(size);

  for (int i = 0; i < size; i++)
    testPQ.insert((i + 1), 10 - i);

  // test PriorityQueue contains
  EXPECT_TRUE(testPQ.contains((1)));
  EXPECT_FALSE(testPQ.contains(100));

  // test PriorityQueue top and minPriority
  int minNode = testPQ.top();
  EXPECT_EQ(5, minNode);
  for (int i = 0; i < size; i++) {
    EXPECT_TRUE(testPQ.chgPrioirity((i + 1), 5 - i));
    minNode = testPQ.top();
    EXPECT_EQ((int )i + 1, minNode);
  }
  minNode = testPQ.minPrioirty();
  EXPECT_EQ(5, minNode);
  minNode = testPQ.top();
  EXPECT_EQ(4, minNode);

  //test string
  PriorityQueue<string, int> stringPQ(size);
  for (int i = 0; i < size; i++) {
    stringstream buffer;
    buffer << "test" << (i + 1);
    stringPQ.insert(buffer.str(), 10 - i);
  }

  EXPECT_TRUE(stringPQ.contains("test1"));
  EXPECT_FALSE(stringPQ.contains("test0"));

  string minStr = stringPQ.top();
  EXPECT_EQ("test5", minStr);
  for (int i = 0; i < size; i++) {
    stringstream buffer;
    buffer << "test" << (i + 1);
    EXPECT_TRUE(stringPQ.chgPrioirity(buffer.str(), 5 - i));
    minStr = stringPQ.top();
    EXPECT_EQ(buffer.str(), minStr);
  }
  minStr = stringPQ.minPrioirty();
  EXPECT_EQ("test5", minStr);
  minStr = stringPQ.top();
  EXPECT_EQ("test4", minStr);
}
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
