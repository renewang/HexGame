/*
 * Player.h
 * This file defines the declaration for the Player class.
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include "Graph.h"
#include "HexBoard.h"
/*
 * Player Class is used as a representation of player in hex board game.
 * Each instance of player class needs to be assigned one of two distinguishable colors or types: RED and BLUE.
 * The color will mark the node value in the board occupied by the player and also decide which player will make first move.
 * This class contains two constructors:
 * Player(); default constructor, not doing anything specific
 *
 * Player(HexBoard& board, hexgonValKind kind): which instantiates a player with particular type.
 *
 * Sample Usage:
 *
 * HexBoard board(5);
 * Player player(board, hexgonValKind::BLUE);
 *
 * which constructs a player on 5x5 board. This player will be assigned Blue color and make first move at the game starts.
 * The winning condition for blue player is to connect west to east side.
 * The move on the board will be marked as 'X'
 */
class Player {
 private:
#if __cplusplus > 199711L
  enum class winConditions {
    WESTTOEAST,
    NORTHTOSOUTH
  };
#define winConditions_WESTTOEAST winConditions::WESTTOEAST
#define winConditions_NORTHTOSOUTH winConditions::NORTHTOSOUTH
#else
  enum winConditions {
    WESTTOEAST,
    NORTHTOSOUTH
  };
#define winConditions_WESTTOEAST WESTTOEAST
#define winConditions_NORTHTOSOUTH NORTHTOSOUTH
#endif

  HexBoard& board;  //global board
  HexBoard playersboard;  //private board, used for Minimal Spanning Tree calculation
  hexgonValKind playerkind;  //enumeration type, used to identify the type of player
  winConditions condition;  //the winning condition which two sides need to be connected, depending on the type of player
  std::string playername;  //string representation of playerkind
  char viewlabel;  //the label on the global board

  //call Minimal Spanning Tree calculation in order to calculate the connected path for every move the player has made
  Graph<hexgonValKind, int> calMST();
  //Private Method to set the connected edges hidden from player's private board
  void setPlayerBoard(int index);

 public:
  //constructor used to specify which kind of player
  Player(HexBoard& board, hexgonValKind kind)
      : board(board),
        playerkind(kind){
    playersboard.setNumofhexgons(board.getNumofhexgons());
    //RED player always starts from North side to South side
    if (kind == hexgonValKind_RED) {
      viewlabel = 'R';
      playername = "RED";
      condition = winConditions_NORTHTOSOUTH;

    } else if (kind == hexgonValKind_BLUE) {
      //BLUE player always starts from West side to East side
      viewlabel = 'B';
      playername = "BLUE";
      condition = winConditions_WESTTOEAST;
    }
  }
  ;
  //destructor
  virtual ~Player() {
  }
  ;
  //To move to the specified location
  bool setMove(int indexofrow, int indexofcol);
  //To test if player has moved to opposite side
  bool isArriveOpposite();
  //getter for private member viewlabel
  char getViewLabel() const {
    return viewlabel;
  }
  //getter for private member playerkind
  hexgonValKind getPlayerlabel() const {
    return playerkind;
  }
  //getter for private member of playername
  const std::string getPlayername() const {
    return std::string(playername);
  }
  //setter for private member of playername
  void setPlayername(const std::string& playername) {
    this->playername = playername;
  }
  //getter for private member of player specific board
  const HexBoard& getPlayersboard() const {
    return playersboard;
  }

  inline bool getWestToEastCondition() const {
    return this->condition == winConditions_WESTTOEAST ? true : false;
  }
  inline bool getNorthToSouthCondition() const {
    return this->condition == winConditions_NORTHTOSOUTH ? true : false;
  }
  inline void resetPlayersboard(){
    this->playersboard.resetHexBoard(true);
  }
};

#endif /* PLAYER_H_ */
