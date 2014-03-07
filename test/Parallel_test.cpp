/*
 * Boost_test.cpp
 *
 *  Created on: Jan 29, 2014
 *      Author: renewang
 */

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "Game.h"
#include "HexBoard.h"
#include "LockableGameTree.h"
#include "MockLockableUTCPolicy.h"

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
  srand(static_cast<unsigned>(clock()));
  {
    strict_lock<recursive_mutex> lock(mutex_);
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
        strict_lock<recursive_mutex> lock(mutex_);
        cerr << DEBUGHEADER() << boost::this_thread::get_id()
             << " has slept for " << secondtosleep << " seconds" << endl;
      }
    }
  }

  int secondtosleep = rand() % 60;
  boost::this_thread::sleep_for(boost::chrono::seconds(secondtosleep));
    strict_lock<recursive_mutex> lock(mutex_);
    cerr << DEBUGHEADER() << boost::this_thread::get_id()
         << " has slept for " << secondtosleep << endl;
    for (unsigned i = 0; i < sharedvec.size(); ++i){
      sharedvec.at(i).updateAll(MockLockableUTCPolicy::visitcount, 0, 1,
                                MockLockableUTCPolicy::wincount, 0, 1);
      sharedvec.at(i).notifyupdatedone();
    }
}
void CreateSharedVecTask(
    vector<hexgame::shared_ptr<MockLockableUTCPolicy> >& sharedvec) {
  srand(static_cast<unsigned>(clock()));

  {
    strict_lock<recursive_mutex> lock(mutex_);
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
        strict_lock<recursive_mutex> lock(mutex_);
        cerr << DEBUGHEADER() << boost::this_thread::get_id()
             << " has slept for " << secondtosleep << "seconds" << endl;
      }
    }
  }
  for (unsigned i = 0; i < sharedvec.size(); ++i){
    sharedvec.at(i).get()->updateAll(MockLockableUTCPolicy::visitcount, 0, 1,
                                     MockLockableUTCPolicy::wincount, 0, 1);
    sharedvec.at(i).get()->notifyupdatedone();
  }
}
void CreateTreeTask(int currentempty, LockableGameTree& gametree,
                    HexBoard& board) {
  int indexofchild = gametree.selectMaxBalanceNode(currentempty, false);
  int numofchildren = gametree.getNumofChildren(
      gametree.getParent(indexofchild));
  int level = gametree.getNodeDepth(gametree.getParent(indexofchild)),
      totalsize = gametree.getNumofTotalNodes(), indexofparent = gametree.getParent(indexofchild);
  {
    strict_lock<recursive_mutex> lock(mutex_);
    cerr << DEBUGHEADER() << boost::this_thread::get_id() << " "
         << indexofparent << ";has a new child;" << indexofchild
         << ";currently has;" << numofchildren << ";should have;"
         << (currentempty - level) << ";total size of tree;" << totalsize
         << endl;
    EXPECT_TRUE(numofchildren != 0);
  }
  int move = rand() % (board.getSizeOfVertices());
  gametree.setNodePosition(indexofchild, move);
  int winner = rand() % 2;
  gametree.updateNodefromSimulation(indexofchild, -1);
}
//create value-parameterized tests, test with numberoftrials (equivalently number of threads)
class ParallelTest : public ::testing::TestWithParam< hexgame::tuple<int, int> > {
  virtual void SetUp() {
    numberofthreads = static_cast<size_t>(hexgame::get<0>(GetParam()));
    currentempty = hexgame::get<1>(GetParam());
  }
  ;
  virtual void TearDown() {
  }
  ;
  protected:
  size_t numberofthreads = 1000;
  int currentempty;
};
TEST_F(ParallelTest, ThreadPropResizeFailed) {
  vector<MockLockableUTCPolicy> sharedvec;

  for (size_t i = 0; i < numberofthreads; ++i) {
    sharedvec.push_back(MockLockableUTCPolicy());
    sharedvec.back().update(MockLockableUTCPolicy::visitcount, 0, 1);
    EXPECT_CALL(sharedvec.at(i), OnConstruct()).Times(
        ::testing::AtMost(1));
    EXPECT_CALL(sharedvec.at(i), OnDestroy()).Times(
        ::testing::AtMost(1));
    EXPECT_CALL(sharedvec.at(i), notifyupdatedone()).Times(
        ::testing::AtLeast(1)).WillRepeatedly(::testing::Return());
  }
  thread_group threads;
  for (size_t i = 0; i < numberofthreads; ++i)
    threads.create_thread(
        boost::bind(&CreateVecTask, boost::ref(sharedvec)));
  threads.join_all();
}
TEST_F(ParallelTest, ThreadPropResize) {
  vector<hexgame::shared_ptr<MockLockableUTCPolicy> > sharedvec;

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
  int numofhexgon = 3;
  HexBoard board(numofhexgon);
  LockableGameTree gametree('B');

  Game hexboardgame(board);
  currentempty = board.getNumofemptyhexgons();

  thread_group threads;
  for (size_t i = 0; i < numberofthreads; ++i)
    threads.create_thread(
        boost::bind(&CreateTreeTask, currentempty, boost::ref(gametree),
                    boost::ref(board)));
  threads.join_all();

  //check children
  for (size_t i = 0; i <= numberofthreads; ++i) {
    int numofchildren = gametree.getNumofChildren(i);
    int level = gametree.getNodeDepth(i);
    EXPECT_TRUE(numofchildren <= (currentempty - level));
  }
  cout << gametree.printGameTree(0) << endl;
}
TEST_P(ParallelTest, ThreadEndofGameValue) {
  int numofhexgon = 3;
  HexBoard board(numofhexgon);
  LockableGameTree gametree('B');

  Game hexboardgame(board);

  thread_group threads;
  for (size_t i = 0; i < numberofthreads; ++i)
    threads.create_thread(
        boost::bind(&CreateTreeTask, currentempty, boost::ref(gametree),
                    boost::ref(board)));
  threads.join_all();

  //check children
  cout << "current size of tree:" << gametree.getNumofTotalNodes() << endl;
  cout << gametree.printGameTree(0);
  cout << "current size of available moves:" << currentempty << endl;
  cout << "current size of thread numbers:" << numberofthreads << endl;
  for (size_t i = 0; i < gametree.getNumofTotalNodes(); ++i) {
    int numofchildren = gametree.getNumofChildren(i);
    int level = gametree.getNodeDepth(i);
    cout << "index:" <<  i << ";level:" << level << ";number of children:" << numofchildren << endl;
    EXPECT_TRUE(numofchildren <= (currentempty - level));
  }
}
INSTANTIATE_TEST_CASE_P(OnTheFlySetThreadNumbertoH,
                        ParallelTest,
                        ::testing::Combine(Range(1, 104, 17), Range(2, 10)));
INSTANTIATE_TEST_CASE_P(OnTheFlySetThreadNumbertoT,
                        ParallelTest,
                        ::testing::Combine(Range(100, 3100, 500), Range(2, 10)));
int main(int argc, char **argv) {
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
