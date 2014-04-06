/*
 * LockableGameTree.h
 * This file defines the implementation for Parallelized Monte Carlo Game Tree and Lockable UTC Policy. Lockable UTC Policy is part of Game Tree property.
 *  Created on: Feb 11, 2014
 *      Author: renewang
 */

#ifndef LOCKABLEGAMETREE_H_
#define LOCKABLEGAMETREE_H_

#include "Global.h"
#include "GameTree.h"
#include "AbstractGameTree.h"

#ifndef NDEBUG
#include "HexBoard.h"
#include "gtest/gtest_prod.h"
#endif

#include <ostream>
#include <iostream>

#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/lockable_adapter.hpp>
#include <boost/thread/externally_locked.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/pthread/shared_mutex.hpp>

typedef boost::lockable_adapter<boost::mutex> lockable_base_type; ///< Define boost lockable_adapter as lockable_base_type
typedef boost::shared_lockable_adapter<boost::shared_mutex> lockable_share_type;///< Define boost shared_lockable_adapter as lockable_base_type
/**
 * LockableUTCPolicy class is used to provide the implementations of lockable AbstractUTCPolicy for the parallelization <br/>
 * LockableUTCPolicy(): default constructor which will initialize data members with default values <br/>
 *
 */
class LockableUTCPolicy : public lockable_share_type, public AbstractUTCPolicy {
 private:
  UTCPolicy policy; ///< UTCPolicy project object which will provide actual implementations of UTCPolicy
  hexgame::atomic<int> countforexpand;///< vertex property, semaphore for local lock to wait for expansion
  hexgame::atomic<bool> isupdated; ///< vertex property, mutex-like atomic variable for local lock to wait for expansion
  int sizeoffuturechildren; ///< this value is to record the future expandable children
  boost::condition_variable_any holdforupdate; ///< this value is used to keep thread in selection phase till the values are updated

 public:
  ///default constructor which will initialize data members with default values
  LockableUTCPolicy()
      : lockable_share_type(),
        countforexpand(0),
        isupdated(false),
        sizeoffuturechildren(0) {
  }
  ;
  ///destructor
  virtual ~LockableUTCPolicy() {
  }
  ///Print the values of Lockable UTC object, debug purpose
  std::string print() {
    return policy.print();
  }
  ///Copy constructor which should do deep copy
  LockableUTCPolicy(const LockableUTCPolicy& policy)
      : lockable_share_type() {
    this->policy = policy.policy;
    this->sizeoffuturechildren = policy.sizeoffuturechildren;
    this->countforexpand.store(policy.countforexpand.load());
  }
  ;
  //for the purpose to construct graph property
  ///Copy assignment operator which should do deep copy
  LockableUTCPolicy& operator=(const LockableUTCPolicy& policy) {
    this->policy = policy.policy;
    this->sizeoffuturechildren = policy.sizeoffuturechildren;
    this->countforexpand.store(policy.countforexpand.load());
    return *this;
  }
  ///See UTCPolicy estimate()
  double estimate();
  ///See UTCPolicy calculate()
  double calculate(AbstractUTCPolicy& parent);
  ///See UTCPolicy update()
  bool update(valuekind indexofkind, int value, int increment = 0);
  ///See UTCPolicy updateAll()
  bool updateAll(
      valuekind visitcount, int valueofvisit, int increaseofvisit,
      valuekind wincount, int valueofwin, int increaseofwin);
  ///See UTCPolicy feature()
  int feature(valuekind indexofkind) {
    return policy.feature(indexofkind);
  }
  ///See UTCPolicy getValue()
  double getValue() const {
    return policy.getValue();
  }
  ///See UTCPolicy getBalance()
  double getBalance() const {
    return policy.getBalance();
  }
  //Wait for the update of feature values from back-propagation phase in MCTS
  virtual bool waitforupdate();
  //Wait for the update of feature values from back-propagation phase in MCTS
  virtual void notifyupdatedone();
  //Get the count of threads waiting for expansion
  int getCountforexpand() {
    return countforexpand.load();
  }
  //Set the count of threads waiting for expansion
  void setCountforexpand(int value) {
    this->countforexpand.store(value);
  }
  //Add the count of threads waiting for expansion
  void addCountforexpand(int increment) {
    countforexpand.fetch_add(increment);
  }
  //Subtract the count of threads waiting for expansion
  void subCountforexpand(int decrement) {
    countforexpand.fetch_sub(decrement);
  }
  //Get the number of future children of the node selected for expansion in selection phase
  int getNumofFutureChildren() {
    return sizeoffuturechildren;
  }
  //Set the number of future children of the node selected for expansion in selection phase
  void setNumofFutureChildren(int numofchildren, int increment) {
    if (numofchildren == 0)
      sizeoffuturechildren += increment;
    else
      sizeoffuturechildren = numofchildren;
  }
  //Get the boolean indicator which shows
  bool getIsupdated() const {
    return isupdated.load();
  }
  //Set the boolean indicator which shows
  void setIsupdated(bool isupdated) {
    this->isupdated.store(isupdated);
  }
};

class LockableGameTree : public lockable_share_type, public AbstractGameTree {
 private:
  //for property
  //TODO remove redundancy
  typedef boost::property<boost::vertex_value_t,
      hexgame::shared_ptr<LockableUTCPolicy> > vertex_value_prop;
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

  vertex_t _root;
  basegraph thetree;
  boost::condition_variable_any holdforupdate;
  boost::condition_variable_any holdforselect;
  boost::condition_variable_any holdforexpand;
  hexgame::unordered_map<vertex_t, std::size_t> childrenoffuture;  //TODO, potential local lock (should put it in LockableUTCPolicy)
  hexgame::atomic<int> countonwait;
  hexgame::atomic<int> countforexpand;
  hexgame::atomic<bool> isblockingforexpand;

  //implement with global lock, external
  vertex_t addNode(boost::unique_lock<LockableGameTree>&,
                   std::size_t positionofchild, char color);
  std::size_t getNodeDepth(boost::shared_lock<LockableGameTree>&,
                           int indexofnode);
  void backpropagate(boost::unique_lock<LockableGameTree>&, vertex_t leaf,
                     int value, int level);bool notifyAllUpdateDone(
      boost::unique_lock<LockableGameTree>&, vertex_t leaf, int level);
  void updateNodeValue(vertex_t node);  //update the value of a given node

  //implement with global lock, internal
  vertex_t addNode(std::size_t positionofchild, char color);
  std::size_t getNodeDepth(int indexofnode);
  void backpropagate(vertex_t leaf, int value, int level);bool notifyAllUpdateDone(
      vertex_t leaf, int level);
  void initGameTree(char playerscolor, size_t indexofroot);

  //implement with local lock, external
  void updateNodeName(boost::unique_lock<LockableUTCPolicy>&, vertex_t node);  //update the value of a given node
  void updateNodeColor(boost::unique_lock<LockableUTCPolicy>&, vertex_t node,
                       char color);  //update the board position of a given node
  void updateNodePosition(boost::unique_lock<LockableUTCPolicy>&, vertex_t node,
                          std::size_t position);
  size_t getNumofChildren(boost::shared_lock<LockableUTCPolicy>&, vertex_t parent);

  //implement with local lock, internal
  bool addEdge(vertex_t source, vertex_t target);

  //no lock
  void updateNodeName(vertex_t node);  //update the value of a given node
  void updateNodeColor(vertex_t node, char color);  //update the board position of a given node
  void updateNodePosition(vertex_t node, std::size_t position);

  //for print
  class TreePrinter {
   private:
    std::ostream_iterator<std::string> _bufiter;
   public:
    TreePrinter(std::ostream_iterator<std::string>& bufiter)
        : _bufiter(bufiter) {
    }
    ;
    template<typename Node, typename Tree>
    void preorder(Node, Tree&) {
      *_bufiter++ = "(";
    }
    template<typename Node, typename Tree>
    void inorder(Node n, Tree& t) {
      *_bufiter++ = get(boost::vertex_name, t)[n]
          + get(boost::vertex_value, t)[n].get()->print();
    }
    template<typename Node, typename Tree>
    void postorder(Node, Tree&) {
      *_bufiter++ = ")";
    }
  };

#ifndef NDEBUG
  friend class ParallelTest;
  FRIEND_TEST(ParallelTest, ThreadGameTreeNode);
  FRIEND_TEST(ParallelTest, DISABLED_ThreadGameTreeUpdate);
  FRIEND_TEST(ParallelTest, ThreadIterativeGame);
  FRIEND_TEST(ParallelTestValue, ThreadEndofGameValue);
  friend void CreateTreeTask(int currentempty, LockableGameTree& gametree,
                             HexBoard& board);
  friend void UpdateTreeTask(int currentempty, LockableGameTree& gametree,
                      HexBoard& board,
                      hexgame::shared_ptr<set<int> >& ptrtochosenkids,
                      hexgame::shared_ptr<set<int> >& ptrtochosenparents);
  friend void SimulationTask(int currentempty, LockableGameTree& gametree,
                             HexBoard& board, hexgame::shared_ptr<bool>& emptyglobal);
#endif

 public:
  LockableGameTree();
  LockableGameTree(char playerslabel);
  LockableGameTree(char playerslabel, std::size_t indexofroot);
  virtual ~LockableGameTree() {
  }
  ;
  //implement with global lock, external
  std::size_t getSizeofEdges(boost::shared_lock<LockableGameTree>&);
  std::size_t getSizeofNodes(boost::shared_lock<LockableGameTree>&);
  void clearAll(boost::unique_lock<LockableGameTree>&);
  std::pair<int,int> selectMaxBalanceNode(boost::unique_lock<LockableGameTree>&,
                           int currentempty, bool isbreaktie = true);
  void getMovesfromTreeState(
      boost::unique_lock<LockableGameTree>&, int indexofnode, std::vector<int>& babywatsons,
      std::vector<int>& opponents, hexgame::unordered_set<int>& remainingmoves);
  int expandNode(boost::unique_lock<LockableGameTree>&, int indexofsource,
                 int move, char color = 'W');
  void updateNodefromSimulation(
      boost::unique_lock<LockableGameTree>& guard, int indexofnode, int winner, int level = -1);
  std::string printGameTree(boost::shared_lock<LockableGameTree>&, int index);  //print out the tree
  void setIsupdatedBackpropagation(boost::unique_lock<LockableGameTree>&,
                                   vertex_t leaf);
  bool getIsupdatedBackpropagation(boost::shared_lock<LockableGameTree>&, int indexofleaf);
  std::vector<size_t> getLeaves(boost::shared_lock<LockableGameTree>&);

  //implement with global lock, internal
  std::size_t getSizeofEdges();
  std::size_t getSizeofNodes();
  void clearAll();
  std::pair<int, double> getBestMovefromSimulation();
  std::pair<int, int> selectMaxBalanceNode(int currentempty, bool isbreaktie = true);
  void getMovesfromTreeState(
         int indexofnode, std::vector<int>& babywatsons, std::vector<int>& opponents,
         hexgame::unordered_set<int>& remainingmoves);
  int expandNode(int indexofsource, int move, char color = 'W');
  void updateNodefromSimulation(int indexofnode, int winner, int level = -1);
  std::string printGameTree(int key);  //print out the tree
  std::string name() {
    return std::string("LockableGameTree");
  }
  ;
  //implement with local locak, exteranl
  void setNodePosition(boost::unique_lock<LockableUTCPolicy>& guard,
                                         vertex_t node, size_t position);
  std::size_t getNodePosition(boost::unique_lock<LockableUTCPolicy>&, vertex_t node);
  vertex_t getParent(boost::unique_lock<LockableUTCPolicy>&, vertex_t node);
  std::vector<std::size_t> getSiblings(boost::unique_lock<LockableGameTree>&, size_t indexofnode);

  //implement with local lock, internal
  void setNodePosition(std::size_t indexofnode, std::size_t position);
  std::vector<std::size_t> getLeaves();
  std::vector<std::size_t> getSiblings(std::size_t indexofnode);
  std::size_t getNodePosition(std::size_t indexofnode);
  std::size_t getNumofChildren(std::size_t indexofnode);
  std::size_t getParent(std::size_t indexofchild);
  bool getIsupdated(int indexofnode);
  bool getIsupdatedBackpropagation(int indexofleaf);
  int getNodeValueFeature(int indexofnode, AbstractUTCPolicy::valuekind indexofkind);
};
#endif /* LOCKABLEGAMETREE_H_ */
