/*
 * KruskalMSTAlg.cpp
 * This file defines the main function for Minimal Spanning Tree (MST) calculation by using Kruskal's algorithm.
 * This application will output the total weight of found MST, tree representation of MST and the half of included edges
 * (symmetric edges in undirected graph will not be included).
 * Please refer to the USAGE to know how to execute this application to invoke Kruskal's algorithm for
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

    MinSpanTreeAlgo<string, int> mstalgo(graph);
    mstalgo.calculate();
    Graph<string, int> msttree = mstalgo.getMsttree();

    cout << "\nThe total weight for Minimal Spanning Tree is "
         << mstalgo.getTotalminwieght() << endl;
    cout
        << "\nThe Minimal Spanning Tree representation is  (index starting from 1):\n"
        << msttree.printMST(1) << endl;
    cout << "\nInclude edges as following (index starting from 0, skipping symmetric edges) :\n";
    map<int, int> mapmstedges = msttree.getAllEdgesValues();
    cout << msttree.getSizeOfVertices() << endl;
    for (map<int, int>::iterator iter = mapmstedges.begin();
        iter != mapmstedges.end(); ++iter) {
      int row = (*iter).first / msttree.getSizeOfVertices();
      int col = (*iter).first % msttree.getSizeOfVertices();
      cout << left << setw(3) << row << " " << left << setw(3) << col << left
           << setw(3) << (*iter).second << endl;
    }
  }
  exit(0);
}
