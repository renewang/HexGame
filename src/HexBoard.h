/*
 * HexBoard.h
 * This file defines the declaration for the HexBoard class.
 */

#ifndef HEXBOARD_H_
#define HEXBOARD_H_

#include <memory>
#include "Graph.h"
/*
 * hexgonValKind enum type
 * Define 3 node kinds which can be later owned by different players (BLUE or RED)
 * EMPTY: the node is not being occupied
 * BLUE: the player marked as blue color
 * RED: the player marked as red color
 */
enum class hexgonValKind {
  EMPTY,
  BLUE,
  RED
};
//Overloading function for hexgonValKind to output the value
std::ostream& operator<<(std::ostream& os, hexgonValKind hexgonkind);
/*
 * HexBoard class is used as a representation of hex board and inherited from Graph
 * Several additional members are added in order to specialize the board
 * This class contains two constructors:
 * HexBoard(): default constructor, not doing anything particular
 * HexBoard(unsigned): constructor used to initialize the board according to the given number of hexgons per side
 * Sample Usage:
 * HexBoard board(5);
 * board.isAdjacent(1, 2) => return TRUE
 * board.isAdjacent(1, 3) => return FALSE
 * which generate 5x5 board
 */
class HexBoard : public Graph<hexgonValKind, int> {
 private:
  //HexgonLocType inner class
  class HexgonLocType {
   private:
    HexBoard& board;  // in order to access enclosing class
   public:
    HexgonLocType(HexBoard& board)
        : board(board) {
    }
    ;
    //enum type numLocEdges
    //Define different node location types
    //DIAGNOLCONER: this kind of nodes possesses 2 out edges, locating at hexboard left upper and right lower corners.
    //ANTIDOAGNOLCORNER: this kind of nodes possesses 3 out edges, locating at hexboard right upper and left lower corners.
    //BOUNDARY: this kind of nodes possesses 4 out edges, locating at the boundary of hexboard other than 4 corners.
    //INTERNAL: this kind of nodes possesses 6 out edges, not locating at the boundary of hexboard.
    enum numLocEdges {
      DIAGNOLCONER = 2,
      ANTIDOAGNOLCORNER = 3,
      BOUNDARY = 4,
      INTERNAL = 6
    };
    //calculate the type of numLocEdge given row and column
    numLocEdges getLocEdges(int row, int col);
  };
  int numofhexgons;  //hexgon per side which constitutes numofvertices = numofhexgon*numofhexgon board

  std::vector<int> redmoves;
  std::vector<int> bluemoves;
  std::shared_ptr<bool> emptyhexindicators;
  int numofemptyhexgons;

  //init emptyhexgons
  void initEmptyHexIndicators();

 protected:
  //overriding base class' initNode function
  virtual void initNode(Node& node, int index);
  //overriding base class' initGraph function
  virtual void initGraph();
 public:
  //default constructor, doing nothing
  HexBoard();
  //constructor to initialize the board according to given hexgon size per side
  HexBoard(unsigned);
  //destructor
  virtual ~HexBoard();
  //setter and overloading function to set edge
  void setEdgeValue(int indexofhexgon);
  //getter for private member numofhexgons
  inline int getNumofhexgons() const {
    return numofhexgons;
  }
  void setNodeValue(int indexofnode, hexgonValKind value);
  //setter for private member numofhexgons
  void setNumofhexgons(int numofhexgon);
  const std::vector<int>& getBluemoves() const {
    return bluemoves;
  }

  inline const std::shared_ptr<bool>& getEmptyHexIndicators() const {
    return emptyhexindicators;
  }

  inline const std::vector<int>& getRedmoves() const {
    return redmoves;
  }
  inline int getNumofemptyhexgons() const {
    return numofemptyhexgons;
  }
};
#endif /* HEXBOARD_H_ */
