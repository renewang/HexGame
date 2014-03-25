#include <boost/python.hpp>

#include "Game.h"
#include "Player.h"
#include "HexBoard.h"
#include "Strategy.h"
#include "AbstractStrategy.h"
#include "AbstractStrategyImpl.h"
#include "MonteCarloTreeSearch.h"
#include "MultiMonteCarloTreeSearch.h"

using namespace boost::python;
class HexGamePyEngine {
 public:
#if __cplusplus > 199711L
  HexGamePyEngine(unsigned numofhexgon)
      : board(HexBoard(numofhexgon)),
        redplayer(Player(board, hexgonValKind::RED)),
        blueplayer(Player(board, hexgonValKind::BLUE)),
        hexboardgame(Game(board)) {
  }
#else
  HexGamePyEngine(unsigned numofhexgon)
  : board(HexBoard(numofhexgon)),
  redplayer(Player(board, RED)),
  blueplayer(Player(board, BLUE)),
  hexboardgame(Game(board)) {
  }
#endif
  ~HexGamePyEngine() {
  }
  bool setRedPlayerMove(int indexofhexgon) {
    if (indexofhexgon) {
      int row = (indexofhexgon - 1) / board.getNumofhexgons() + 1;
      int col = (indexofhexgon - 1) % board.getNumofhexgons() + 1;
      return hexboardgame.setMove(redplayer, row, col);
    }
    return false;
  }
  bool setBluePlayerMove(int indexofhexgon) {
    if (indexofhexgon) {
      int row = (indexofhexgon - 1) / board.getNumofhexgons() + 1;
      int col = (indexofhexgon - 1) % board.getNumofhexgons() + 1;
      return hexboardgame.setMove(blueplayer, row, col);
    }
    return false;
  }
  int genRedPlayerMove() {
    return hexboardgame.genMove(*aistrategy['R']);
  }
  int genBluePlayerMove() {
    return hexboardgame.genMove(*aistrategy['B']);
  }
  void showView() {
    std::string view = hexboardgame.showView(redplayer, blueplayer);
/*    string alphabets = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for(int i = 1; i <= board.getNumofhexgons(); ++i)
    view.replace(to_string(i), std::string(alphabets[i-1]));*/
    std::cout <<  view << '\n';
  }
  std::string getWinner() {
    return hexboardgame.getWinner(redplayer, blueplayer);
  }
  unsigned getNumofhexgons() {
    return board.getNumofhexgons();
  }
  void setNumofhexgons(unsigned numofhexgon) {
    board.setNumofhexgons(numofhexgon);
  }
  hexgonValKind getNodeValue(int indexofhexgon) {
    return board.getNodeValue(indexofhexgon);
  }
  void resetGame() {
    hexboardgame.resetGame(redplayer, blueplayer);
  }
  void setRedPlayerStrategy(AIStrategyKind strategykind) {
    selectStrategy(strategykind, redplayer);
  }
  void setBluePlayerStrategy(AIStrategyKind strategykind) {
    selectStrategy(strategykind, blueplayer);
  }
 private:
  HexBoard board;
  Player redplayer;  //north to south, 'O'
  Player blueplayer;  //west to east, 'X'
  Game hexboardgame;
  hexgame::unordered_map<char, hexgame::shared_ptr<AbstractStrategy> > aistrategy;
//TODO, duplicate code
  void selectStrategy(AIStrategyKind strategykind, Player& player) {
    switch (strategykind) {
      case AIStrategyKind_NAIVE:
      std::cout <<"\n"<< player.getPlayername()<<" uses naive strategy" << endl;
      aistrategy.insert(make_pair(player.getViewLabel(), hexgame::shared_ptr<AbstractStrategy>(new Strategy(&board, &player))));
      break;
      case AIStrategyKind_PMCST:
      std::cout <<"\n"<< player.getPlayername()<<" uses PMCST strategy" << endl;
      aistrategy.insert(make_pair(player.getViewLabel(), hexgame::shared_ptr<AbstractStrategy>(new MultiMonteCarloTreeSearch(&board, &player))));
      break;
      default:
      std::cout <<"\n"<< player.getPlayername()<< " uses MCST strategy" << endl;
      aistrategy.insert(make_pair(player.getViewLabel(), hexgame::shared_ptr<AbstractStrategy>(new MonteCarloTreeSearch(&board, &player))));
      break;
    }
  }
};
BOOST_PYTHON_MODULE(libhexgame)
{
  enum_<hexgonValKind>("hexgonValKind").value("EMPTY", hexgonValKind_EMPTY)
  .value("RED", hexgonValKind_RED)
  .value("BLUE", hexgonValKind_BLUE);
  enum_<AIStrategyKind>("AIStrategyKind")
  .value("NAIVE", AIStrategyKind_NAIVE)
  .value("MCST", AIStrategyKind_MCST)
  .value("PMCST", AIStrategyKind_PMCST);
  class_<HexGamePyEngine>("HexGamePyEngine", init<unsigned>())
  .def("showView", &HexGamePyEngine::showView)
  .def("setRedPlayerMove", &HexGamePyEngine::setRedPlayerMove)
  .def("setBluePlayerMove", &HexGamePyEngine::setBluePlayerMove)
  .def("genRedPlayerMove", &HexGamePyEngine::genRedPlayerMove)
  .def("genBluePlayerMove", &HexGamePyEngine::genBluePlayerMove)
  .def("getWinner", &HexGamePyEngine::getWinner)
  .def("getNodeValue", &HexGamePyEngine::getNodeValue)
  .def("resetGame", &HexGamePyEngine::resetGame)
  .def("setRedPlayerStrategy", &HexGamePyEngine::setRedPlayerStrategy)
  .def("setBluePlayerStrategy", &HexGamePyEngine::setBluePlayerStrategy)
  .add_property("numofhexgons", &HexGamePyEngine::getNumofhexgons,
      &HexGamePyEngine::setNumofhexgons);
}