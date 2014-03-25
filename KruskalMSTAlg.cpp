/*
 * KruskalMSTAlg.cpp
 * This file defines the main function for Minimal Spanning Tree (MST) calculation by using Kruskal's algorithm.
 * This application will output the total weight of found MST, tree representation of MST and the half of included edges
 * (symmetric edges in undirected graph will not be included).
 * Please refer to the USAGE to know how to execute this application to invoke Kruskal's algorithm for
 */

#include <map>
#include <string>
#include <memory>
#include <utility>
#include <iostream>

#include "Graph.h"
#include "Global.h"
#include "PlainParser.h"
#include "PriorityQueue.h"
#include "MinSpanTreeAlgo.h"

using namespace std;

const char *USAGE =
    "\n\nCalculate the minimal spanning tree for a graph file via different algorithms\n\n"
        "Usage:\n\n"
        "./KruskaMSTAlg <file_path_to_sample_graph_file> [algorithm]\n\n"
        "file_path_to_sample_graph_file: is a text file stores number of vertices in the first line\n"
        "                                and edges information in the following format\n"
        "                                [index_of_source_node index_of_destination_node weight_of_edge]\n"
        "algorithm[unionfind(default)|dfs|prim]: the algorithms choices to calculate Minimal Spanning Tree\n"
        "unionfind:                    : use kruskal + unionfind to calculate Minimal Spanning Tree\n"
        "dfs:                          : use kruskal + dfs to calculate Minimal Spanning Tree\n"
        "prim:                         : use prim to calculate Minimal Spanning Tree\n";

int main(int argc, char **argv) {
  unsigned algochoice = 1;
  if (argc != 2 && argc != 3)
    cout << USAGE << endl;
  else {

    if (argc == 3) {
      string algorithm = string(argv[2]);
      if (algorithm.compare("dfs") == 0)
        algochoice = 2;
      else if (algorithm.compare("prim") == 0)
        algochoice = 3;
    }

    string filename(argv[1]);
    PlainParser parser(filename);
    Graph<string, int> graph(parser);

    map<int, int> mapalledges = graph.getAllEdgesValues();
    PriorityQueue<int, int> edgesqueue(mapalledges.size());
    for (map<int, int>::iterator iter = mapalledges.begin();
        iter != mapalledges.end(); ++iter)
      edgesqueue.insert((*iter).first, (*iter).second);

    MinSpanTreeAlgo<string, int> mstalgo(graph);
    hexgame::unique_ptr<AbstractAlgorithm, hexgame::default_delete<AbstractAlgorithm> > ptrtoalgo(nullptr);
    switch (algochoice) {
      case 2:
        ptrtoalgo.reset(new MinSpanTreeAlgo<string, int>::Kruskals(mstalgo));
        break;
      case 3:
        ptrtoalgo.reset(new MinSpanTreeAlgo<string, int>::Prim(mstalgo));
        break;
      default:
        ptrtoalgo.reset(new MinSpanTreeAlgo<string, int>::UnionFind(mstalgo));
    }
    mstalgo.calculate(*ptrtoalgo);
    Graph<string, int> msttree = mstalgo.getMsttree();

    cout << "\nUsing algorithm "<< ptrtoalgo->name() << endl;
    cout << "\nThe total weight for Minimal Spanning Tree is "
         << mstalgo.getTotalminwieght() << endl;
    cout
        << "\nThe Minimal Spanning Tree representation is  (index starting from 1):\n"
        << msttree.printMST(1) << endl;
    cout
        << "\nInclude edges as following (index starting from 0, skipping symmetric edges) :\n";
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
