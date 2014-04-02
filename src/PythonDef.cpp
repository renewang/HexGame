#include <boost/python.hpp>

#include "Game.h"
#include "Player.h"
#include "HexBoard.h"
#include "Strategy.h"
#include "AbstractStrategy.h"

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
    std::cout << view << '\n';
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
  void selectStrategy(AIStrategyKind strategykind, Player& player) {
    hexgame::unique_ptr<AbstractStrategy,
        hexgame::default_delete<AbstractStrategy> > transformer(nullptr);
    ::selectStrategy(strategykind, transformer, player, board);
    aistrategy.insert(
            make_pair(player.getViewLabel(),
                      hexgame::shared_ptr<AbstractStrategy>(transformer.release())));
  }
};
BOOST_PYTHON_MODULE(libhexgame)
{
  enum_<hexgonValKind>("hexgonValKind").value("EMPTY", hexgonValKind_EMPTY)
  .value("RED", hexgonValKind_RED)
  .value("BLUE", hexgonValKind_BLUE);
  enum_<AIStrategyKind>("AIStrategyKind")
  .value("NAIVE", AIStrategyKind_NAIVE)
  .value("MCST", AIStrategyKind_MCTS)
  .value("PMCST", AIStrategyKind_PMCTS);
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
