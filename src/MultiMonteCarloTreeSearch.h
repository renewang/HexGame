/*
 * MultiMonteCarloTreeSearch.h
 *
 *  Created on: Feb 11, 2014
 *      Author: renewang
 */

#ifndef MULTIMONTECARLOTREESEARCH_H_
#define MULTIMONTECARLOTREESEARCH_H_

#include "Player.h"
#include "HexBoard.h"
#include "GameTree.h"
#include "MonteCarloTreeSearch.h"

#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/lockable_adapter.hpp>
#include <boost/thread/externally_locked.hpp>


#ifndef NDEBUG
#include "gtest/gtest_prod.h"
#endif

class MultiMonteCarloTreeSearch : public MonteCarloTreeSearch, public boost::basic_lockable_adapter<boost::mutex> {

  typedef basic_lockable_adapter<boost::mutex> lockable_base_type;
  typedef boost::externally_locked<GameTree, MultiMonteCarloTreeSearch> treelock;

 private:
  int numofhexgons;
  //the actual playing board in the game. Need to  ensure it not to be modified during the simulation
  const HexBoard* const ptrtoboard;
  //the actual player computer plays. Need to ensure it not to be modified during the simulation
  const Player* const ptrtoplayer;
  const std::size_t numberoftrials;
  char babywatsoncolor, oppoenetcolor;

  void init();
  void task(const std::vector<int>& bwglobal, const std::vector<int>& oppglobal,
            const boost::shared_ptr<bool>& emptyglobal, int currentempty,
            treelock& lock);
  int selection(int currentempty, treelock& locker);
  int expansion(int selectnode, boost::shared_ptr<bool>& emptyindicators,
                int& portionofempty, std::vector<int>& babywatsons,
                std::vector<int>& opponents, treelock& locker);
  void backpropagation(int backupnode, int winner, treelock& locker);

#ifndef NDEBUG
  //for google test framework
  friend class MinMaxTest;
  FRIEND_TEST(MinMaxTest,CompeteHexParallelGame);
#endif

 public:
  //constructor
  MultiMonteCarloTreeSearch(const HexBoard* board, const Player* aiplayer);
  MultiMonteCarloTreeSearch(const HexBoard* board, const Player* aiplayer,
                            size_t numberoftrials);
  virtual ~MultiMonteCarloTreeSearch() {
  }
  ;
  std::string name() {
    return string("MultiMonteCarloTreeSearch");
  }
  ;
  int simulation();
};

#endif /* MULTIMONTECARLOTREESEARCH_H_ */
