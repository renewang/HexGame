/*
 * HexBoard.cpp
 *
 *  Created on: Nov 12, 2013
 *      Author: renewang
 */

#include "HexBoard.h"
using namespace std;

HexBoard::HexBoard(unsigned numofhexgon) :
		numofhexgons(numofhexgon)
{
	initGraph();
	numofvertices = numofhexgon * numofhexgon;
	for (unsigned i = 0; i < numofvertices; i++)
	{
		Node node;
		initNode(node, i);
		repgraph.push_back(node);
	}
	numofedges /= 2;
}

HexBoard::~HexBoard()
{
}
void HexBoard::initNode(Node& node, int index)
{
	//Assign the corresponding edges according to the hexgonloctype
	//Assign all empty to all hexgons
	node.vertexindex = (index + 1);
	node.vertexvalue = hexgonValKind::EMPTY;
	int row = index / numofhexgons;
	int col = index % numofhexgons;
	HexBoard::HexgonLocType locafinder(*this);
	HexBoard::HexgonLocType::numLocEdges type = locafinder.getLocEdges(row,
			col);
	switch (type)
	{
	case HexBoard::HexgonLocType::DIAGNOLCONER:
	{
		node.numofneighbors = 2;
		//push neighbors
		for (unsigned i = 1; i <= numofhexgons; i = i + (numofhexgons - 1))
		{
			Edge edge;
			edge.indexoffromnode = node.vertexindex;
			edge.weight = 1;
			//index + 1 or index + numofhexgons for row = 0
			//index - 1 or index - numofhexgons for row = (numofhexgons - 1)
			if (row == 0)
				edge.indexoftonode = node.vertexindex + i;
			else
				edge.indexoftonode = node.vertexindex - i;
			node.neighbors.push_back(edge);
			numofedges++;
		}
		break;
	}
	case HexBoard::HexgonLocType::ANTIDOAGNOLCORNER:
	{
		node.numofneighbors = 3;
		//push neighbors
		int diffforantidia[3] =
		{ 1, -(numofhexgons - 1), -numofhexgons };
		for (unsigned i = 0; i < 3; i++)
		{
			Edge edge;
			edge.indexoffromnode = node.vertexindex;
			edge.weight = 1;
			//index - 1, index + (numofhexgons - 1) or index + (numofhexgons) for row = 0
			//index + 1 or index - (numofhexgons -1 ) or index - (numofhexgons) for row = (numofhexgons - 1)
			if (row == 0)
				edge.indexoftonode = node.vertexindex - diffforantidia[i];
			else
				edge.indexoftonode = node.vertexindex + diffforantidia[i];
			node.neighbors.push_back(edge);
			numofedges++;
		}
		break;
	}
	case HexBoard::HexgonLocType::BOUNDARY:
	{
		node.numofneighbors = 4;
		//push neighbors
		for (unsigned i = 0; i < 4; i++)
		{
			Edge edge;
			edge.indexoffromnode = node.vertexindex;
			edge.weight = 1;
			int difffortopbottom[4] =
			{ -1, 1, (numofhexgons - 1), numofhexgons };
			int diffforleftright[4] =
			{ 1, -(numofhexgons - 1), -numofhexgons, numofhexgons };
			//north side, row = 0, index - 1. index + 1, index + (numofhexgons -1), index + numofhexgons
			//south side, row = (numofhexgons -1), index - 1. index + 1, index - (numofhexgons -1), index - numofhexgons
			//west side, col = 0, index + 1, index + numofhexgons, index - numofhexgons, index - (numofhexgons - 1)
			//east side, col = (numofhexgons -1), index - 1, index + numofhexgons, index - numofhexgons, index + (numofhexgons - 1)
			if (row == 0)			//north
				edge.indexoftonode = node.vertexindex + difffortopbottom[i];
			else if (row == (numofhexgons - 1))			//south
				edge.indexoftonode = node.vertexindex - difffortopbottom[i];
			else if (col == 0)			//west
				edge.indexoftonode = node.vertexindex + diffforleftright[i];
			else if (col == (numofhexgons - 1))			//east
				edge.indexoftonode = node.vertexindex - diffforleftright[i];
			node.neighbors.push_back(edge);
			numofedges++;
		}
		break;
	}
	case HexBoard::HexgonLocType::INTERNAL:
	{
		node.numofneighbors = 6;
		//push neighbors
		//index - 1, index - numofhexgons, index - (numofhexgons - 1),
		//index + 1, index + numofhexgons, index + (numofhexgons - 1)
		int diffforinternal[6] =
		{ -1, -(numofhexgons - 1), -numofhexgons, numofhexgons, (numofhexgons
				- 1), 1 };
		for (unsigned i = 0; i < 6; i++)
		{
			Edge edge;
			edge.indexoffromnode = node.vertexindex;
			edge.weight = 1;
			edge.indexoftonode = node.vertexindex + diffforinternal[i];
			node.neighbors.push_back(edge);
			numofedges++;
		}
		break;
	}
	}
}
void HexBoard::initGraph()
{
	numofedges = 0;
	numofvertices = 0;
	isundirected = true;
}
HexBoard::HexgonLocType::numLocEdges HexBoard::HexgonLocType::getLocEdges(
		int row, int col)
{
	if ((col - row) == 0 && (row == 0 || row == (board.numofhexgons - 1)))
		return HexBoard::HexgonLocType::DIAGNOLCONER;
	else if ((row + col) == (board.numofhexgons - 1)
			&& (row == 0 || row == (board.numofhexgons - 1)))
		return HexBoard::HexgonLocType::ANTIDOAGNOLCORNER;
	else if (row == 0 || row == (board.numofhexgons - 1) || col == 0
			|| col == (board.numofhexgons - 1))
		return HexBoard::HexgonLocType::BOUNDARY;
	else
		return HexBoard::HexgonLocType::INTERNAL;
}
ostream& operator<<(ostream& os, hexgonValKind hexgonkind)
{
	switch(hexgonkind){
	case hexgonValKind::EMPTY:
		os << "EMPTY";
		break;
	case hexgonValKind::RED:
		os << "RED";
		break;
	case hexgonValKind::BLUE:
		os << "BLUE";
		break;
	}
	return os;
}
