/*
 * LockableGameTree.h
 *
 *  Created on: Feb 11, 2014
 *      Author: renewang
 */

#ifndef LOCKABLEGAMETREE_H_
#define LOCKABLEGAMETREE_H_

#include "Global.h"
#include "AbstractGameTree.h"

#ifndef NDEBUG
#include "HexBoard.h"
#include "gtest/gtest_prod.h"
#endif

#include <ostream>
#include <iostream>

#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/lockable_adapter.hpp>
#include <boost/thread/externally_locked.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/pthread/shared_mutex.hpp>

typedef boost::lockable_adapter<boost::mutex> lockable_base_type;
typedef boost::shared_lockable_adapter<boost::shared_mutex> lockable_share_type;
class LockableUTCPolicy : public lockable_share_type, public AbstractUTCPolicy {
 private:
  double value;  //this value is calculated by a value function (estimate) across simulated sample and needs to be maximized to get the best move
  double balance;  //this value is calculated by search policy, usually U (calculate) to balance exploration (random) and exploitation (greedy choice) and needs to be maximized to get the potential candidate
  const double coefficient;
  std::size_t numoffeatures;
  std::vector<int> featureholder;
  hexgame::atomic<int> countforexpand;  //vertex property, semaphore for local lock.
  hexgame::atomic<bool> isupdated;
  int sizeoffuturechildren;
  boost::condition_variable_any holdforupdate;
 public:
  LockableUTCPolicy()
      : lockable_share_type(),
        value(0.0),
        balance(value),
        coefficient(2.0),
        numoffeatures(2),
        featureholder(std::vector<int>(numoffeatures)),
        countforexpand(0),
        isupdated(false),
        sizeoffuturechildren(0) {
    std::fill(featureholder.begin(), featureholder.end(), 0);
  }
  ;
  virtual ~LockableUTCPolicy() {
  }
  std::string print() {
    std::stringstream buffer;
    buffer << " [" << value << "|" << balance << "|"
           << featureholder.at(wincount) << "|" << featureholder.at(visitcount)
           << "] ";
    return buffer.str();
  }
  LockableUTCPolicy(const LockableUTCPolicy& policy)
      : lockable_share_type(),
        coefficient(2.0) {
    this->value = policy.value;
    this->balance = policy.balance;
    this->numoffeatures = policy.numoffeatures;
    this->featureholder = policy.featureholder;
    this->sizeoffuturechildren = policy.sizeoffuturechildren;
    this->countforexpand.store(policy.countforexpand.load());
  }
  ;
  //for the purpose to construct graph property
  LockableUTCPolicy& operator=(const LockableUTCPolicy& policy) {
    this->value = policy.value;
    this->balance = policy.balance;
    this->numoffeatures = policy.numoffeatures;
    this->featureholder = policy.featureholder;
    this->sizeoffuturechildren = policy.sizeoffuturechildren;
    this->countforexpand.store(policy.countforexpand.load());
    return *this;
  }
  virtual bool waitforupdate();
  double estimate();
  double calculate(AbstractUTCPolicy& parent);
  virtual void notifyupdatedone();
  bool update(valuekind indexofkind, int value, int increment = 0);
  bool updateAll(
      valuekind visitkind, int valueofvisit, int increaseofvisit,
      valuekind winkind, int valueofwin, int increaseofwin);
  int feature(valuekind indexofkind) {
    return featureholder.at(indexofkind);
  }
  double getValue() const {
    return value;
  }

  double getBalance() const {
    return balance;
  }

  int getCountforexpand() {
    return countforexpand.load();
  }
  void setCountforexpand(int value) {
    this->countforexpand.store(value);
  }
  void addCountforexpand(int increment) {
    countforexpand.fetch_add(increment);
  }
  void subCountforexpand(int decrement) {
    countforexpand.fetch_sub(decrement);
  }
  int getNumofFutureChildren() {
    return sizeoffuturechildren;
  }
  void setNumofFutureChildren(int numofchildren, int increment) {
    if (numofchildren == 0)
      sizeoffuturechildren += increment;
    else
      sizeoffuturechildren = numofchildren;
  }
  bool getIsupdated() const {
    return isupdated.load();
  }
  void setIsupdated(bool isupdated) {
    this->isupdated.store(isupdated);
  }
};

class LockableGameTree : public lockable_share_type, public AbstractGameTree {
 private:
  //for property
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
  void updateNodeValue(boost::unique_lock<LockableGameTree>&, vertex_t node);  //update the color of a given node

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
  size_t getNumofChildren(boost::unique_lock<LockableUTCPolicy>&, vertex_t parent);

  //implement with local lock, internal
  bool addEdge(vertex_t source, vertex_t target);
  vertex_t getParent(vertex_t node);

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
  int updateNodefromSimulation(
      boost::unique_lock<LockableGameTree>& guard, int indexofnode, int winner, int level = -1);
  std::string printGameTree(boost::shared_lock<LockableGameTree>&, int index);  //print out the tree
  std::size_t getNumofTotalNodes(boost::shared_lock<LockableGameTree>&);
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
  int updateNodefromSimulation(int indexofnode, int winner, int level = -1);
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
  std::size_t getParent(std::size_t indexofchild, bool);
  bool getIsupdated(int indexofnode);
  bool getIsupdatedBackpropagation(int indexofleaf);
  int getNodeValueFeature(int indexofnode, AbstractUTCPolicy::valuekind indexofkind);
};
#endif /* LOCKABLEGAMETREE_H_ */
