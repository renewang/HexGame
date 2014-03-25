#ifndef ABSTRACTSTRATEGY_H_
#define ABSTRACTSTRATEGY_H_

#include <vector>
#include <string>

#include "Player.h"
#include "HexBoard.h"

#if __cplusplus > 199711L
enum class AIStrategyKind {
  NAIVE = 1,
  MCST,
  PMCST
};
#define AIStrategyKind_NAIVE AIStrategyKind::NAIVE
#define AIStrategyKind_MCST AIStrategyKind::MCST
#define AIStrategyKind_PMCST AIStrategyKind::PMCST
#else
enum AIStrategyKind {
  NAIVE,
  MCST,
  PMCST
};
#define AIStrategyKind_NAIVE NAIVE
#define AIStrategyKind_MCST MCST
#define AIStrategyKind_PMCST PMCST
#endif
class AbstractStrategy {

 protected:
  //check if the winner exists for this stage of simulation
  virtual int checkWinnerExist(std::vector<int>&, std::vector<int>&) = 0;
  virtual bool isWinner(std::vector<int>& test, bool iswestoeast) = 0;
  virtual int simulation(int currentempty) = 0;
  virtual int genNextRandom(hexgame::shared_ptr<bool>& emptyindicators, int& proportionofempty) = 0;

 public:
  AbstractStrategy() {
  }
  ;
  virtual ~AbstractStrategy() {
  }
  ;
  //called by the client
  virtual int genMove() = 0;
  virtual std::string name() = 0;
};

#endif /* ABSTRACTSTRATEGY_H_ */
