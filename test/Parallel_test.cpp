/*
 * Boost_test.cpp
 *
 *  Created on: Jan 29, 2014
 *      Author: renewang
 */

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "Game.h"
#include "Player.h"
#include "HexBoard.h"
#include "LockableGameTree.h"
#include "MockLockableUTCPolicy.h"
#include "MultiMonteCarloTreeSearch.h"

#include <set>
#include <vector>
#include <iostream>
#include <algorithm>

#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/strict_lock.hpp>

using namespace std;
using namespace boost;
using namespace testing;

#if __cplusplus <= 199711L
class Util {
private:
  hexgame::shared_ptr<bool>& emptyindicators;
public:
  Util(hexgame::shared_ptr<bool>& indicators):emptyindicators(indicators) {}
  void operator()(int i) {
    if(i!=0)
    emptyindicators.get()[i-1] = false;
  }
};
struct Indicator {
  bool operator()(int i) {
    return i==0;
  }
};
#endif

//Failing test case for resize locked condition variable (CreateSharedVecTask). Should not use for actual verification for code
//Usage:
//vector<MockLockableUTCPolicy> sharedvec;
//sharedvec.push_back(MockLockableUTCPolicy()); //call OnConstruct()/OnDestroy() without being registered
//sharedvec.back().update(MockLockableUTCPolicy::visitcount, 0, 1);
//EXPECT_CALL(sharedvec.back(), OnConstruct()).Times(::testing::AtMost(1));
//EXPECT_CALL(sharedvec.back(), OnDestroy()).Times(::testing::AtMost(1));
//EXPECT_CALL(sharedvec.back(), waitforupdate()).Times(::testing::AtLeast(1))
//        .WillRepeatedly(::testing::Return(void()));
//EXPECT_CALL(sharedvec.back(), notifyupdatedone()).Times(::testing::AtLeast(1))
//    .WillRepeatedly(::testing::Return(void()));
//threads.create_thread(boost::bind(&CreateVecTask, boost::ref(sharedvec)));
void CreateVecTask(vector<MockLockableUTCPolicy>& sharedvec) {
  srand(static_cast<unsigned>(time(NULL)));
  {
    strict_lock<recursive_mutex> lock(_recursive_mutex);
    sharedvec.resize(sharedvec.size() + 1);  //Expect call updateAll once will fail due to vector resize and reconstruct objects.
    sharedvec.at(sharedvec.size() - 1) = MockLockableUTCPolicy();
    EXPECT_CALL(sharedvec.back(), OnConstruct()).Times(::testing::AtMost(1));
    EXPECT_CALL(sharedvec.back(), OnDestroy()).Times(::testing::AtMost(1));
    EXPECT_CALL(sharedvec.back(), waitforupdate()).Times(::testing::AtLeast(1))
        .WillRepeatedly(::testing::Return(true));
    EXPECT_CALL(sharedvec.back(), notifyupdatedone()).Times(
        ::testing::AtLeast(1)).WillRepeatedly(::testing::Return());
  }

  for (unsigned i = 0; i < sharedvec.size(); ++i) {
    if (sharedvec.at(i).feature(MockLockableUTCPolicy::visitcount) == 0) {
      //simulate timed-wait
      {
        int secondtosleep = rand() % 60;
        boost::this_thread::sleep_for(boost::chrono::seconds(secondtosleep));
        strict_lock<recursive_mutex> lock(_recursive_mutex);
        DEBUGHEADER();
        cerr << boost::this_thread::get_id() << " has slept for "
             << secondtosleep << " seconds" << endl;
      }
    }
  }

  int secondtosleep = rand() % 60;
  boost::this_thread::sleep_for(boost::chrono::seconds(secondtosleep));
  strict_lock<recursive_mutex> lock(_recursive_mutex);
  DEBUGHEADER();
  cerr << boost::this_thread::get_id() << " has slept for " << secondtosleep
       << " seconds" << endl;
  for (unsigned i = 0; i < sharedvec.size(); ++i) {
    sharedvec.at(i).updateAll(MockLockableUTCPolicy::visitcount, 0, 1,
                              MockLockableUTCPolicy::wincount, 0, 1);
    sharedvec.at(i).notifyupdatedone();
  }
}
void CreateSharedVecTask(
    vector<hexgame::shared_ptr<MockLockableUTCPolicy> >& sharedvec) {
  srand(static_cast<unsigned>(time(NULL)));

  {
    strict_lock<recursive_mutex> lock(_recursive_mutex);
    sharedvec.resize(sharedvec.size() + 1);
    sharedvec.at(sharedvec.size() - 1) = hexgame::shared_ptr<
        MockLockableUTCPolicy>(new MockLockableUTCPolicy());
    EXPECT_CALL((*sharedvec.back().get()), OnConstruct()).Times(
        ::testing::AtMost(1));
    EXPECT_CALL((*sharedvec.back().get()), OnDestroy()).Times(
        ::testing::AtMost(1));
    EXPECT_CALL((*sharedvec.back().get()), waitforupdate()).Times(
        ::testing::AtLeast(1)).WillRepeatedly(::testing::Return(true));
    EXPECT_CALL((*sharedvec.back().get()), notifyupdatedone()).Times(
        ::testing::AtLeast(1)).WillRepeatedly(::testing::Return());
  }

  for (unsigned i = 0; i < sharedvec.size(); ++i) {
    if (sharedvec.at(i).get()->feature(MockLockableUTCPolicy::visitcount)
        == 0) {
      sharedvec.at(i).get()->waitforupdate();
      //simulate timed-wait
      {
        int secondtosleep = rand() % 60;
        boost::this_thread::sleep_for(boost::chrono::seconds(secondtosleep));
        strict_lock<recursive_mutex> lock(_recursive_mutex);
        DEBUGHEADER();
        cerr << boost::this_thread::get_id() << " has slept for "
             << secondtosleep << " seconds" << endl;
      }
    }
  }
  for (unsigned i = 0; i < sharedvec.size(); ++i) {
    sharedvec.at(i).get()->updateAll(MockLockableUTCPolicy::visitcount, 0, 1,
                                     MockLockableUTCPolicy::wincount, 0, 1);
    sharedvec.at(i).get()->notifyupdatedone();
  }
}
void CreateTreeTask(int currentempty, LockableGameTree& gametree,
                    HexBoard& board) {
  pair<int, int> selectresult = gametree.selectMaxBalanceNode(currentempty, false);
  int selectnode = selectresult.first;
  int indexofchild = selectnode;
  int level = selectresult.second;
  if (level != currentempty)
    indexofchild = gametree.expandNode(selectnode, 0);
  int move = rand() % (board.getSizeOfVertices());
  gametree.setNodePosition(indexofchild, move);
  int winner = rand() % 2;
  gametree.updateNodefromSimulation(indexofchild, winner);
}
void SimulationTask(int currentempty, LockableGameTree& gametree,
                    HexBoard& board, hexgame::shared_ptr<bool>& emptyglobal) {
  pair<int, int> selectresult = gametree.selectMaxBalanceNode(currentempty,
  false);
  int selectnode = selectresult.first;
  int indexofchild = selectnode;
  int level = selectresult.second;
  if (level != currentempty)
    indexofchild = gametree.expandNode(selectnode, 0);

  vector<int> babywatsons, opponents;
  hexgame::shared_ptr<bool> emptyindicators = hexgame::shared_ptr<bool>(
      new bool[board.getSizeOfVertices()], hexgame::default_delete<bool[]>());
  copy(emptyglobal.get(), emptyglobal.get() + board.getSizeOfVertices(),
       emptyindicators.get());
  {
    strict_lock<recursive_mutex> lock(_recursive_mutex);
    for (int i = 0; i < board.getSizeOfVertices(); ++i)
      EXPECT_EQ(emptyindicators.get()[i], emptyglobal.get()[i]);
  }
  hexgame::unordered_set<int> remainingmoves;
  for (int i = 0; i < board.getSizeOfVertices(); ++i) {
    if (emptyindicators.get()[i])
      remainingmoves.insert(i + 1);
  }
  assert(static_cast<int>(remainingmoves.size()) == currentempty);
  gametree.getMovesfromTreeState(indexofchild, babywatsons, opponents,
                                 remainingmoves);

  //align empty indicators with state of game tree node
#if __cplusplus > 199711L
  for_each(babywatsons.begin(), babywatsons.end(),
           [&](int i) {if(i!=0) emptyindicators.get()[i-1] = false;});
  for_each(opponents.begin(), opponents.end(),
           [&](int i) {if(i!=0) emptyindicators.get()[i-1] = false;});
#else
  Util functor(emptyindicators);
  for_each(babywatsons.begin(), babywatsons.end(), functor);
  for_each(opponents.begin(), opponents.end(), functor);
#endif

  assert(count(babywatsons.begin(), babywatsons.end(), 0) == 0);
  assert(count(opponents.begin(), opponents.end(), 0) == 0);

  int winner = rand() % 2;
  gametree.updateNodefromSimulation(indexofchild, winner);
}
//create value-parameterized tests, test with numberoftrials (equivalently number of threads)
class ParallelTest : public ::testing::Test {
  virtual void SetUp() {
    numberofthreads = 64;
    currentempty = 9;
    numofhexgon = 3;
    numberoftrials = 2048;
  }
  ;
  virtual void TearDown() {
  }
  ;
 protected:
  size_t numberofthreads;
  int currentempty;
  int numofhexgon;
  int numberoftrials;
};
//create value-parameterized tests, test with numberoftrials (equivalently number of threads)
class ParallelTestValue : public ::testing::TestWithParam<
    hexgame::tuple<int, int, int> > {
  virtual void SetUp() {
    numberofthreads = static_cast<size_t>(hexgame::get<0>(GetParam()));
    currentempty = hexgame::get<1>(GetParam());
    numofhexgon = hexgame::get<2>(GetParam());
    numberoftrials = 2048;
  }
  ;
  virtual void TearDown() {
  }
  ;
 protected:
  size_t numberofthreads;
  int currentempty;
  int numofhexgon;
  int numberoftrials;
};
TEST_F(ParallelTest, DISABLED_ThreadPropResizeFailed) {
  vector<MockLockableUTCPolicy> sharedvec;
  numberofthreads = 10;
  for (size_t i = 0; i < numberofthreads; ++i) {
    sharedvec.push_back(MockLockableUTCPolicy());
    sharedvec.back().update(MockLockableUTCPolicy::visitcount, 0, 1);
    EXPECT_CALL(sharedvec.at(i), OnConstruct()).Times(::testing::AtMost(1));
    EXPECT_CALL(sharedvec.at(i), OnDestroy()).Times(::testing::AtMost(1));
    EXPECT_CALL(sharedvec.at(i), notifyupdatedone()).Times(
        ::testing::AtLeast(1)).WillRepeatedly(::testing::Return());
  }
  thread_group threads;
  for (size_t i = 0; i < numberofthreads; ++i)
    threads.create_thread(boost::bind(&CreateVecTask, boost::ref(sharedvec)));
  threads.join_all();
}
TEST_F(ParallelTest, ThreadPropResize) {
  vector<hexgame::shared_ptr<MockLockableUTCPolicy> > sharedvec;
  numberofthreads = 10;
  for (size_t i = 0; i < numberofthreads; ++i) {
    sharedvec.push_back(
        hexgame::shared_ptr<MockLockableUTCPolicy>(
            new MockLockableUTCPolicy()));
    sharedvec.back().get()->update(MockLockableUTCPolicy::visitcount, 0, 1);
    EXPECT_CALL((*sharedvec.at(i).get()), OnConstruct()).Times(
        ::testing::AtMost(1));
    EXPECT_CALL((*sharedvec.at(i).get()), OnDestroy()).Times(
        ::testing::AtMost(1));
    EXPECT_CALL((*sharedvec.at(i).get()), notifyupdatedone()).Times(
        ::testing::AtLeast(1)).WillRepeatedly(::testing::Return());
  }

  thread_group threads;
  for (size_t i = 0; i < numberofthreads; ++i)
    threads.create_thread(
        boost::bind(&CreateSharedVecTask, boost::ref(sharedvec)));
  threads.join_all();
}
TEST_F(ParallelTest, ThreadGameTreeNode) {
  LockableGameTree gametree('B');
  HexBoard board(numofhexgon);
  currentempty = board.getNumofemptyhexgons();

  cout << "current size of thread numbers:" << numberofthreads << endl;
  for (size_t j = 0; j < (numberoftrials / numberofthreads); ++j) {
    thread_group threads;
    for (size_t i = 0; i < numberofthreads; ++i)
      threads.create_thread(
          boost::bind(&CreateTreeTask, currentempty, boost::ref(gametree),
                      boost::ref(board)));
    threads.join_all();
  }
  //check children
  for (size_t i = 0; i < gametree.getSizeofNodes(); ++i) {
    int numofchildren = gametree.getNumofChildren(i);
    int level = gametree.getNodeDepth(i);
    cout << "index:" << i << ";level:" << level << ";number of children:"
             << numofchildren << endl;
    EXPECT_TRUE(numofchildren <= (currentempty - level));
  }
  cout << gametree.printGameTree(0) << endl;
}
TEST_P(ParallelTestValue, ThreadEndofGameValue) {
  LockableGameTree gametree('B');
  HexBoard board(numofhexgon);
  hexgame::shared_ptr<bool> emptyglobal = board.getEmptyHexIndicators();
  srand(static_cast<unsigned>(time(NULL)));
  vector<int> numbers(board.getSizeOfVertices());
  for (int i = 0; i < board.getSizeOfVertices(); i++)
    numbers[i] = i;
  std::random_shuffle(numbers.begin(), numbers.end());

  for (int i = board.getSizeOfVertices(); i > currentempty; i--)
    emptyglobal.get()[numbers[i - 1]] = false;

  int randempty = std::count(emptyglobal.get(),
                             emptyglobal.get() + board.getSizeOfVertices(),
                             true);
  ASSERT_EQ(randempty, currentempty);

  cout << "current size of available moves:" << currentempty << endl;
  cout << "current size of thread numbers:" << numberofthreads << endl;

  for (size_t j = 0; j < (numberoftrials / numberofthreads); ++j) {
    thread_group threads;
    for (size_t i = 0; i < numberofthreads; ++i)
      threads.create_thread(
          boost::bind(&SimulationTask, currentempty, boost::ref(gametree),
                      boost::ref(board), boost::ref(emptyglobal)));
    threads.join_all();
  }
  //check children
  cout << "current size of tree:" << gametree.getSizeofNodes() << endl;
  cout << gametree.printGameTree(0);
  //check number of children per node
  for (size_t i = 0; i < gametree.getSizeofNodes(); ++i) {
    int numofchildren = gametree.getNumofChildren(i);
    int level = gametree.getNodeDepth(i);
    cout << "index:" << i << ";level:" << level << ";number of children:"
         << numofchildren << endl;
    EXPECT_TRUE(numofchildren <= (currentempty - level));
  }
  //need to be all updated
  vector<size_t> leaves = gametree.getLeaves();
  for (vector<size_t>::iterator iter = leaves.begin(); iter != leaves.end();
      ++iter) {
    EXPECT_EQ(gametree.getNumofChildren(*iter), 0);
    EXPECT_TRUE(gametree.getIsupdated(*iter));
    EXPECT_TRUE(gametree.getIsupdatedBackpropagation(*iter));
  }
  //check all positions at the same level cannot be repeating
  map<int, vector<int> > trackmovesatlevel;
  for (size_t i = 1; i < gametree.getSizeofNodes(); ++i) {
    int pos = gametree.getNodePosition(i);
    int parent = gametree.getParent(i);
    if (trackmovesatlevel.count(parent) == 0) {
      vector<int> posholder;
      trackmovesatlevel.insert(make_pair(parent, posholder));
    }
    trackmovesatlevel[parent].push_back(pos);
  }
  //check redundancy for siblings
  for (map<int, vector<int> >::iterator iter = trackmovesatlevel.begin();
      iter != trackmovesatlevel.end(); ++iter) {
    cout << "Parent:" << iter->first << ":";
    vector<int> posholder = iter->second;
    for (vector<int>::iterator it = posholder.begin(); it != posholder.end();
        ++it)
      cout << *it << " ";
    cout << endl;
    sort(posholder.begin(), posholder.end());
    for (unsigned i = 0; i < (posholder.size() - 1); ++i)
      EXPECT_NE(posholder[i], posholder[i + 1]);
  }
  //check redundancy for ancestral-child
  for (vector<size_t>::iterator iter = leaves.begin(); iter != leaves.end();
      ++iter) {
    int level = gametree.getNodeDepth(*iter);
    int indexofparent = *iter;
    vector<int> posholder;
    while (indexofparent != 0) {
      size_t pos = gametree.getNodePosition(indexofparent);
      posholder.push_back(pos);
      indexofparent = gametree.getParent(indexofparent);
    }
    EXPECT_EQ(posholder.size(), level);
    if (!posholder.empty()) {
      cout << "Leaf:" << *iter << ":";
      for (vector<int>::iterator it = posholder.begin(); it != posholder.end();
          ++it)
        cout << *it << " ";
      cout << endl;
      sort(posholder.begin(), posholder.end());
      for (unsigned j = 0; j < (posholder.size() - 1); ++j)
        EXPECT_NE(posholder[j], posholder[j + 1]);
    }
  }
}
TEST_F(ParallelTest, ThreadIterativeGame) {
  LockableGameTree gametree('B');
  HexBoard board(numofhexgon);
  hexgame::shared_ptr<bool> emptyglobal = board.getEmptyHexIndicators();

  cout << "current size of available moves:" << currentempty << endl;
  cout << "current size of thread numbers:" << numberofthreads << endl;

  for (size_t j = 0; j < (numberoftrials / numberofthreads); ++j) {
    thread_group threads;
    for (size_t i = 0; i < numberofthreads; ++i)
      threads.create_thread(
          boost::bind(&SimulationTask, currentempty, boost::ref(gametree),
                      boost::ref(board), boost::ref(emptyglobal)));
    EXPECT_EQ(threads.size(), numberofthreads);
    threads.join_all();
  }

  //check children
  cout << "current size of tree:" << gametree.getSizeofNodes() << endl;
  cout << gametree.printGameTree(0);
  //check number of children per node
  for (size_t i = 0; i < gametree.getSizeofNodes(); ++i) {
    int numofchildren = gametree.getNumofChildren(i);
    int level = gametree.getNodeDepth(i);
    cout << "index:" << i << ";level:" << level << ";number of children:"
         << numofchildren << endl;
    EXPECT_TRUE(numofchildren <= (currentempty - level));
  }
  //need to be all updated
  vector<size_t> leaves = gametree.getLeaves();
  EXPECT_TRUE(static_cast<int>(leaves.size()) <= numberoftrials);
  for (vector<size_t>::iterator iter = leaves.begin(); iter != leaves.end();
      ++iter) {
    EXPECT_EQ(gametree.getNumofChildren(*iter), 0);
    EXPECT_TRUE(gametree.getIsupdated(*iter));
    EXPECT_TRUE(gametree.getIsupdatedBackpropagation(*iter));
  }
  //check all positions at the same level cannot be repeating
  map<int, vector<int> > trackmovesatlevel;
  for (size_t i = 1; i < gametree.getSizeofNodes(); ++i) {
    int pos = gametree.getNodePosition(i);
    int parent = gametree.getParent(i);
    if (trackmovesatlevel.count(parent) == 0) {
      vector<int> posholder;
      trackmovesatlevel.insert(make_pair(parent, posholder));
    }
    trackmovesatlevel[parent].push_back(pos);
  }
  //check redundancy for siblings
  for (map<int, vector<int> >::iterator iter = trackmovesatlevel.begin();
      iter != trackmovesatlevel.end(); ++iter) {
    cout << "Parent:" << iter->first << ":";
    vector<int> posholder = iter->second;
    for (vector<int>::iterator it = posholder.begin(); it != posholder.end();
        ++it)
      cout << *it << " ";
    cout << endl;
    sort(posholder.begin(), posholder.end());
    for (unsigned i = 0; i < (posholder.size() - 1); ++i)
      EXPECT_NE(posholder[i], posholder[i + 1]);
  }
  //check redundancy for ancestral-child
  for (vector<size_t>::iterator iter = leaves.begin(); iter != leaves.end();
      ++iter) {
    int level = gametree.getNodeDepth(*iter);
    int indexofparent = *iter;
    vector<int> posholder;
    while (indexofparent != 0) {
      size_t pos = gametree.getNodePosition(indexofparent);
      posholder.push_back(pos);
      indexofparent = gametree.getParent(indexofparent);
    }
    EXPECT_EQ(posholder.size(), level);
    if (!posholder.empty()) {
      cout << "Leaf:" << *iter << ":";
      for (vector<int>::iterator it = posholder.begin(); it != posholder.end();
          ++it)
        cout << *it << " ";
      cout << endl;
      sort(posholder.begin(), posholder.end());
      for (unsigned j = 0; j < (posholder.size() - 1); ++j)
        EXPECT_NE(posholder[j], posholder[j + 1]);
    }
  }
}
INSTANTIATE_TEST_CASE_P(
    OnTheFlySetThreadNumber, ParallelTestValue,
    ::testing::Combine(Values(4), Range(1, 26, 1), Values(5)));
INSTANTIATE_TEST_CASE_P(
    DISABLED_OnTheFlySetThreadNumberFixempty, ParallelTestValue,
    ::testing::Combine(Range(2, 18, 2), Values(9, 25, 121), Values(3, 5, 11)));
int main(int argc, char **argv) {
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
