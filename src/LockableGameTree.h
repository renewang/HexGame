/*
 * LockableGameTree.h
 *
 *  Created on: Feb 11, 2014
 *      Author: renewang
 */

#ifndef LOCKABLEGAMETREE_H_
#define LOCKABLEGAMETREE_H_

#include "GameTree.h"

#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/lockable_adapter.hpp>

class LockableGameTree : public GameTree, public boost::basic_lockable_adapter<
    boost::recursive_mutex> {

  //TODO redundant declarations
  //for property
  typedef boost::property<boost::vertex_value_t, UTCPolicy> vertex_value_prop;
  typedef boost::property<boost::vertex_color_t, boost::default_color_type,
      vertex_value_prop> vertex_color_prop;
  typedef boost::property<boost::vertex_name_t, std::string, vertex_color_prop> vertex_name_prop;
  typedef boost::property<boost::vertex_position_t, std::size_t,
      vertex_name_prop> vertex_final_prop;

  //for graph and tree
  typedef boost::adjacency_list<boost::listS, boost::vecS,
      boost::bidirectionalS, vertex_final_prop> basegraph;  //adjacency_list<OutEdgeListS,VertexListS,DirectedS>

  //for graph traits
  typedef boost::graph_traits<basegraph>::vertex_descriptor vertex_t;
  typedef boost::graph_traits<basegraph>::edge_descriptor edge_t;
  typedef boost::graph_traits<basegraph>::vertex_iterator vertex_iter;
  typedef boost::graph_traits<basegraph>::adjacency_iterator adj_vertex_iter;
  typedef boost::graph_traits<basegraph>::in_edge_iterator in_edge_iter;
  typedef boost::graph_traits<basegraph>::out_edge_iterator out_edge_iter;

  //for property map
  typedef boost::property_map<basegraph, boost::vertex_name_t>::type vertex_namemap_t;
  typedef boost::property_map<basegraph, boost::vertex_color_t>::type vertex_colormap_t;
  typedef boost::property_map<basegraph, boost::vertex_value_t>::type vertex_valuemap_t;
  typedef boost::property_map<basegraph, boost::vertex_position_t>::type vertex_positionmap_t;
  typedef boost::property_map<basegraph, boost::vertex_index_t>::type vertex_indexmap_t;

  //for tree
  typedef boost::iterator_property_map<std::vector<vertex_t>::iterator,
      boost::property_map<basegraph, boost::vertex_index_t>::type> parent_map_t;
  typedef boost::graph_as_tree<basegraph, parent_map_t> gametree;

 private:
  basegraph& thetree;
  vertex_namemap_t vertexname;
  vertex_colormap_t vertexcolor;
  vertex_valuemap_t vertexvalue;
  vertex_positionmap_t vertexposition;
  vertex_indexmap_t vertexindex;
  vertex_t _root;

  void initGameTree();

 public:
  LockableGameTree();
  LockableGameTree(char playerslabel);
  LockableGameTree(char playerslabel, std::size_t indexofroot);
  virtual ~LockableGameTree() {
  }
  ;
  int selectMaxBalanceNode(int currentempty, bool isbreaktie = true);
  int expandNode(int indexofsource, int move, char color = 'W');
  void setNodePosition(std::size_t indexofnode, std::size_t position);
  int updateNodefromSimulation(int indexofnode, int winner);
  void backpropagatefromSimulation(int indexofnode, int value, int level = -1);
  std::string name() {
    return std::string("LockableGameTree");
  }
  ;
};
#endif /* LOCKABLEGAMETREE_H_ */
