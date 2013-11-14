/*
 * HexBoard.h
 */

#ifndef HEXBOARD_H_
#define HEXBOARD_H_

#include "Graph.h"
//Define 3 node kinds which can be later owned by different players (BLACK or WHITE)
//EMPTY: the node is not being occupied
//BLUE: the player marked as blue color
//RED: the player marked as red color
enum class hexgonValKind
{
	EMPTY, BLUE, RED
};
std::ostream& operator<<(std::ostream& os, hexgonValKind hexgonkind);
class HexBoard: public Graph<hexgonValKind, int>
{
private:
	//Define different node location types
	//DIAGNOLCONER: this kind of nodes possesses 2 out edges, locating at hexboard left upper and right lower corners.
	//ANTIDOAGNOLCORNER: this kind of nodes possesses 3 out edges, locating at hexboard right upper and left lower corners.
	//BOUNDARY: this kind of nodes possesses 4 out edges, locating at the boundary of hexboard other than 4 corners.
	//INTERNAL: this kind of nodes possesses 6 out edges, not locating at the boundary of hexboard.
	class HexgonLocType
	{
	private:
		HexBoard& board;
	public:
		HexgonLocType(HexBoard& board):board(board){};
		enum numLocEdges
		{
			DIAGNOLCONER = 2, ANTIDOAGNOLCORNER = 3, BOUNDARY = 4, INTERNAL = 6
		};
		numLocEdges getLocEdges(int row, int col);
	};
	int numofhexgons; //hexgon per side which constitutes numofvertices = numofhexgon*numofhexgon board
protected:
	virtual void initNode(Node& node, int index);
	virtual void initGraph();
public:
	HexBoard();
	HexBoard(unsigned);
	virtual ~HexBoard();
	inline int getNumofhexgons() const
	{
		return numofhexgons;
	}
};

#endif /* HEXBOARD_H_ */
