/*
 * LockableGameTree.h
 *
 *  Created on: Feb 11, 2014
 *      Author: renewang
 */

#ifndef LOCKABLEGAMETREE_H_
#define LOCKABLEGAMETREE_H_

#include "Global.h"
#include "GameTree.h"

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

#ifndef NDEBUG
extern boost::recursive_mutex mutex_;
#endif

typedef boost::lockable_adapter<boost::mutex> lockable_base_type;
class LockableUTCPolicy : public lockable_base_type {
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
  enum valuekind {
    visitcount = 0,
    wincount
  };
  LockableUTCPolicy()
      : lockable_base_type(),
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
      : lockable_base_type(),
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
  virtual bool waitforupdate(boost::unique_lock<LockableUTCPolicy>&);
  double estimate();
  double calculate(LockableUTCPolicy& parent);
  virtual void notifyupdatedone();bool update(valuekind indexofkind, int value,
                                              int increment = 0);bool updateAll(
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
    boost::unique_lock<LockableUTCPolicy> lock(*this);
    return countforexpand;
  }
  void setCountforexpand(int value) {
    boost::unique_lock<LockableUTCPolicy> lock(*this);
    this->countforexpand.store(value);
  }
  void addCountforexpand(int increment) {
    boost::unique_lock<LockableUTCPolicy> lock(*this);
    countforexpand.fetch_add(increment);
  }
  void subCountforexpand(int decrement) {
    boost::unique_lock<LockableUTCPolicy> lock(*this);
    countforexpand.fetch_sub(decrement);
  }
  int getNumofFutureChildren() {
    boost::unique_lock<LockableUTCPolicy> lock(*this);
    return sizeoffuturechildren;
  }
  void setNumofFutureChildren(int numofchildren, int increment) {
    boost::unique_lock<LockableUTCPolicy> lock(*this);
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

  class LockableGameTree : public lockable_base_type {
    //TODO redundant declarations
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

   private:
    vertex_namemap_t vertexname;
    vertex_colormap_t vertexcolor;
    vertex_valuemap_t vertexvalue;
    vertex_positionmap_t vertexposition;
    vertex_indexmap_t vertexindex;
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
    std::size_t getNodeDepth(boost::unique_lock<LockableGameTree>&,
                             int indexofnode);
    void backpropagate(boost::unique_lock<LockableGameTree>&, vertex_t leaf,
                       int value, int level);
    bool notifyAllUpdateDone(boost::unique_lock<LockableGameTree>&, vertex_t leaf, int level);
    void updateNodeValue(boost::unique_lock<LockableGameTree>&, vertex_t node); //update the color of a given node

    //implement with global lock, internal
    vertex_t addNode(std::size_t positionofchild, char color);
    std::size_t getNodeDepth(int indexofnode);
    void backpropagate(vertex_t leaf, int value, int level);
    bool notifyAllUpdateDone(vertex_t leaf, int level);
    void initGameTree(char playerscolor, size_t indexofroot);

    //implement with local lock, external
    void updateNodeName(boost::unique_lock<LockableUTCPolicy>&, vertex_t node); //update the value of a given node
    void updateNodeColor(boost::unique_lock<LockableUTCPolicy>&, vertex_t node, char color); //update the board position of a given node
    void updateNodePosition(boost::unique_lock<LockableUTCPolicy>&, vertex_t node, std::size_t position);

    //implement with local lock, internal
    bool addEdge(vertex_t source, vertex_t target);
    vertex_t getParent(vertex_t node);

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
    friend class ParallelTest;FRIEND_TEST(ParallelTest, ThreadGameTreeNode);
    FRIEND_TEST(ParallelTest, ThreadEndofGameValue);
    friend void CreateTreeTask(int currentempty, LockableGameTree& gametree,
                               HexBoard& board);
    friend void UpdateTreeTask(int currentempty, LockableGameTree& gametree,
                        HexBoard& board, hexgame::shared_ptr< set<int> >& ptrtochosenkids);
#endif

   public:
    LockableGameTree();
    LockableGameTree(char playerslabel);
    LockableGameTree(char playerslabel, std::size_t indexofroot);

    //implement with global lock, external
    int selectMaxBalanceNode(boost::unique_lock<LockableGameTree>&,
                             int currentempty, bool isbreaktie = true);
    void getMovesfromTreeState(boost::unique_lock<LockableGameTree>&,
                               int indexofchild, std::vector<int>& babywatsons,
                               std::vector<int>& opponents);
    int expandNode(boost::unique_lock<LockableGameTree>&, int indexofsource,
                   int move, char color = 'W');
    int updateNodefromSimulation(boost::unique_lock<LockableGameTree>&,
                                 int indexofnode, int winner);
    void backpropagatefromSimulation(boost::unique_lock<LockableGameTree>&,
                                     int indexofnode, int value,
                                     int level = -1);
    string printGameTree(boost::unique_lock<LockableGameTree>&, int index); //print out the tree
    std::size_t getNumofTotalNodes(boost::unique_lock<LockableGameTree>&);
    void setIsupdatedBackpropagation(boost::unique_lock<LockableGameTree>&, vertex_t leaf);

    //implement with global lock, internal
    int selectMaxBalanceNode(int currentempty, bool isbreaktie = true);
    void getMovesfromTreeState(int indexofchild, std::vector<int>& babywatsons,
                                   std::vector<int>& opponents);
    int expandNode(int indexofsource, int move, char color = 'W');
    int updateNodefromSimulation(int indexofnode, int winner);
    void backpropagatefromSimulation(int indexofnode, int value,
                                     int level = -1);
    std::string printGameTree(int key); //print out the tree
    std::size_t getNumofTotalNodes();
    std::string name() {
      return std::string("LockableGameTree");
    }
    ;

    //implement with local lock, internal
    void setNodePosition(std::size_t indexofnode, std::size_t position);
    std::vector<std::size_t> getLeaves();
    std::vector<std::size_t> getSiblings(std::size_t indexofnode);
    std::size_t getNodePosition(std::size_t indexofnode);
    std::size_t getNumofChildren(std::size_t indexofnode);
    std::size_t getParent(std::size_t indexofchild, bool);
    bool getIsupdated(int indexofnode);
    bool getIsupdatedBackpropagation(int indexofleaf);

  };
#endif /* LOCKABLEGAMETREE_H_ */
