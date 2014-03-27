/*
 * HexBoard.h
 * This file defines the declaration for the HexBoard class.
 */

#ifndef HEXBOARD_H_
#define HEXBOARD_H_

#include "Graph.h"
#include "Global.h"
/*
 * hexgonValKind enum type
 * Define 3 node kinds which can be later owned by different players (BLUE or RED)
 * EMPTY: the node is not being occupied
 * BLUE: the player marked as blue color
 * RED: the player marked as red color
 */
#if __cplusplus > 199711L
enum class hexgonValKind {
  EMPTY,
  BLUE,
  RED
};
#define hexgonValKind_EMPTY hexgonValKind::EMPTY
#define hexgonValKind_BLUE hexgonValKind::BLUE
#define hexgonValKind_RED hexgonValKind::RED
#else
enum hexgonValKind {
  EMPTY,
  BLUE,
  RED
};
#define hexgonValKind_EMPTY EMPTY
#define hexgonValKind_BLUE BLUE
#define hexgonValKind_RED RED
#endif
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
    HexBoard& board;  // HexBoard object for the purpose access enclosing class
   public:
    //constructor which passes enclosing class, HexBoard, in order to access the functionality of HexBoard
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
  //Graph property
  std::vector<Node>& repgraph;  //adjacent list implementation of Graph representation
  bool isundirected;  //indicator for a undirected graph
  unsigned numofvertices;  //size of Nodes or Vertex in Graph
  unsigned numofedges;  //size of Edges in Graph

  int numofhexgons;  //hexgon per side which constitutes numofvertices = numofhexgon*numofhexgon board
  std::vector<int> redmoves;  //vector stores the indices of hexgons marked as RED
  std::vector<int> bluemoves;  //vector stores the indices of hexgons marked as BLUE
  hexgame::shared_ptr<bool> emptyhexindicators;  //boolean array stores the indicator which shows if this hexgon is empty or not
  int numofemptyhexgons;  //number of empty hexgons

  //init emptyhexgons
  void initEmptyHexIndicators();
  //overriding base class' initNode function
  void initNode(Node& node, int index);
  //overriding base class' initGraph function
  void initGraph();

 public:
  //default constructor, doing nothing
  HexBoard();
  //constructor to initialize the board according to given hexgon size per side
  explicit HexBoard(unsigned);
  //user defined copy constructor
  HexBoard(const HexBoard& otherboard);
  //user defined copy assignment constructor
  HexBoard& operator =(const HexBoard& otherboard);
  //destructor
  virtual ~HexBoard();
  //setter and overloading function to set edge
  void setEdgeValue(int indexofhexgon);
  //getter for private member numofhexgons
  inline int getNumofhexgons() const {
    return numofhexgons;
  }
  //setter for the value of a hexgon
  void setNodeValue(int indexofnode, hexgonValKind value);
  //setter for private member numofhexgons
  void setNumofhexgons(int numofhexgon);
  //getter for private member bluemoves. Note: Used to return reference. Now return value
  inline const std::vector<int>& getBluemoves() const {
    return bluemoves;
  }
  //getter for private member emptyhexindicators. Note: Used to return reference. Now return value
  inline const hexgame::shared_ptr<bool>& getEmptyHexIndicators() const {
    return emptyhexindicators;
  }
  //getter for private member redmoves. Note: Used to return reference. Now return value
  inline const std::vector<int>& getRedmoves() const {
    return redmoves;
  }
  //getter for private member numofemptyhexgons
  inline int getNumofemptyhexgons() const {
    return numofemptyhexgons;
  }
  //restore HexBoard to the initial state
  //Input:
  //isresetedges: the boolean variable which is used to indicate if the edges are all removed too
  //TRUE: the edges will be reset as no edges present (for players board)
  //FALSE:  the edges will be reset as the initial game setup (for general board)
  //Output: None
  void resetHexBoard(bool isresetedges = true);
  //Delete the edge between the specified nodes
  //Input:
  //indexofnodefrom: the vertexindex of the source node
  //indexofnodeto, the vertexindex of the destination node
  //Output: None
  void deleteEdge(int indexofnodefrom, int indexofnodeto);
};
#endif /* HEXBOARD_H_ */
