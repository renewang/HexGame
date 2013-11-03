/*
 * ShortestPathAlgo.h
 *
 */
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cassert>

#include "Graph.h"
#include "PriorityQueue.h"
#include "ShortestPathAlgo.h"

using namespace std;

const char *USAGE =
		"\n\nCalculate the average shorted path from node 1 to the other nodes\n\n"
				"Usage:\n\n"
				"./DijkstraAlg <number_of_vertices> <density> <maximal_distance>\n\n"
				"number_of_vertices>: is used to specify the number of vertices in a grandom graph\n"
				"density: is used to decide the connectedness of a random graph and its value has to \n"
				"         be not less than 0 and not greater than 1\n"
				"maximal_distance: is used to decide the maximal distance of an edge. \n"
				"         A value of edge will be constrained between the range [1, <maximal_distance>]";

int main(int argc, char **argv)
{
	if (argc != 4)
		cout << USAGE << endl;
	else
	{
		int numOfVertices = atoi(argv[1]);
		float density = atof(argv[2]);
		float distance = atof(argv[3]);

		assert(numOfVertices > 0);
		assert(density >= 0 && density <= 1);
		assert(distance > 1);

		Graph<string, double> graph(numOfVertices, density, distance);
		ShortestPathAlgo<string, double> algo(graph);
		double shortestPathCost = algo.averagePathSize(1);

		cout << "average shortest path from node 1 to the other nodes = "
				<< shortestPathCost << " for density " << density << endl;
	}
	exit(0);
}
