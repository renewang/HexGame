/*
 * KruskalMSTAlg.cpp
 *
 *  Created on: Nov 9, 2013
 *      Author: renewang
 */

#include <map>
#include <string>
#include <utility>
#include <iostream>

#include "Graph.h"
#include "PlainParser.h"
#include "PriorityQueue.h"
#include "MinSpanTreeAlgo.h"

using namespace std;

const char *USAGE =
    "\n\nCalculate the minimal spanning tree for a graph file via Kruskal Minimal Spanning Tree Algorithm\n\n"
        "Usage:\n\n"
        "./KruskaMSTAlg <file_path_to_sample_graph_file>\n\n"
        "file_path_to_sample_graph_file: is a text file stores number of vertices in the first line\n"
        "                                and edges information in the following format\n"
        "                                [index_of_source_node index_of_destination_node weight_of_edge]\n";

int main(int argc, char **argv) {
  if (argc != 2)
    cout << USAGE << endl;
  else {
    string filename(argv[1]);
    PlainParser parser(filename);
    Graph<string, int> graph(parser);

    map<int, int> mapalledges = graph.getAllEdgesValues();
    PriorityQueue<int, int> edgesqueue(mapalledges.size());
    for (map<int, int>::iterator iter = mapalledges.begin();
        iter != mapalledges.end(); ++iter)
      edgesqueue.insert((*iter).first, (*iter).second);

    MinSpanTreeAlgo mstalgo(graph);
    mstalgo.calculate();
    Graph<string, int> msttree = mstalgo.getMsttree();

    cout << "The total weight for Minimal Spanning Tree is " << mstalgo.getTotalminwieght() << endl;
    cout << "The Minimal Spanning Tree representation is :\n" << msttree.printMST(1) << endl;
  }
  exit(0);
}
