CXXFLAGS =	-O3 -Wall -Wextra -Werror -fmessage-length=0 $(OPTFLAGS)
LIBS = -lboost_system $(OPTLIBS)
SRCDIR =	src
EXEDIR =	bin
INCLUDE =	-I./$(SRCDIR) -I/opt/boost_1_55_0 $(OPTINCLUDE)
TEST_ROOT= GraphAlgoExe
TEST_SUBDIRS= GraphDebug MSTDebug HexBoardDebug AIStrategyDebug MCSTDebug MinMaxDebug98 ParallelDebug 
TARGET =	build
PREFIX?=	/usr/local
CHECKFLAG= -v --enable=all

# compile release build
all:	CXXFLAGS+= -std=c++11 -DNDEBUG
all:	$(EXEDIR)/DijkstraAlg $(EXEDIR)/KruskalMSTAlg $(EXEDIR)/HexBoardGameApp

# compile debug build
dev:	CXXFLAGS+= -g3 -pg --std=c++11
dev:	CHECKFLAG+= -std=c++11
dev:	OPTINCLUDE= -I./contrib
dev:	cppcheck all	

devold:	CXXFLAGS+= -g3 -pg
devold: CHECKFLAG+= --std=c++03
devold:	OPTINCLUDE= -I./contrib
devold: cppcheck all

.PHONY:  buildtest $(TEST_SUBDIRS)
buildtest: MAKECOMMAND = $(MAKE) all -C
buildtest: $(TEST_SUBDIRS)

.PHONY: clean cleanall cleantest
cleanall: clean cleantest
clean:
	rm -rf  $(EXEDIR)/*

cleantest: MAKECOMMAND= $(MAKE) clean -C
cleantest: $(TEST_SUBDIRS)

#unit tests
.PHONY: tests
#tests: buildtest valgrind
tests: buildtest
tests:
	for dir in $(TEST_SUBDIRS); do \
		sh -x ~/Script/GraphAlgoExe_test_auto.sh $(TEST_ROOT)/$$dir; \
	done

$(TEST_SUBDIRS):
	$(MAKECOMMAND) $(TEST_ROOT)/$@

#TODO doesn't work!	
#valgrind:
#export VALGRIND="valgrind --log-file=/tmp/valgrind-%p.log"

cppcheck:
	cppcheck $(CHECKFLAG) $(SRCDIR) 2&> "/tmp/cppcheck-`date +%Y-%m-%d`.log" 

#compile DijkstraAlg exe
$(EXEDIR)/Graph.o: $(SRCDIR)/Graph.h
	$(CXX) $(CXXFLAGS)  -o $(EXEDIR)/Graph.o -c $(SRCDIR)/Graph.h $(LIBS) $(INCLUDE)

$(EXEDIR)/PriorityQueue.o: $(SRCDIR)/PriorityQueue.h $(EXEDIR)/Graph.o
	$(CXX) $(CXXFLAGS)  -o $(EXEDIR)/PriorityQueue.o -c $(SRCDIR)/PriorityQueue.h $(LIBS) $(INCLUDE)

$(EXEDIR)/ShortestPathAlgo.o: $(SRCDIR)/ShortestPathAlgo.h $(EXEDIR)/Graph.o $(EXEDIR)/PriorityQueue.o
	$(CXX) $(CXXFLAGS)  -o $(EXEDIR)/ShortestPathAlgo.o $(SRCDIR)/ShortestPathAlgo.h $(LIBS) $(INCLUDE)

$(EXEDIR)/DijkstraAlg.o: DijkstraAlg.cpp $(EXEDIR)/ShortestPathAlgo.o $(EXEDIR)/PriorityQueue.o $(EXEDIR)/Graph.o
	$(CXX) $(CXXFLAGS)  -o $(EXEDIR)/DijkstraAlg.o -c DijkstraAlg.cpp $(LIBS) $(INCLUDE)
	
$(EXEDIR)/DijkstraAlg:	$(EXEDIR)/DijkstraAlg.o
	$(CXX) $(CXXFLAGS)  -o $(EXEDIR)/DijkstraAlg $(EXEDIR)/DijkstraAlg.o $(LIBS) $(INCLUDE)

#compile KruskalMSTAlg	
$(EXEDIR)/PlainParser.o: $(SRCDIR)/PlainParser.cpp
	$(CXX) $(CXXFLAGS)  -o $(EXEDIR)/PlainParser.o -c $(SRCDIR)/PlainParser.cpp $(LIBS) $(INCLUDE)

$(EXEDIR)/AbstractAlgorithm.o: $(SRCDIR)/AbstractAlgorithm.h
	$(CXX) $(CXXFLAGS)  -o $(EXEDIR)/AbstractAlgorithm.o -c $(SRCDIR)/AbstractAlgorithm.h $(LIBS) $(INCLUDE)
	
$(EXEDIR)/MinSpanTreeAlgo.o: $(SRCDIR)/MinSpanTreeAlgo.h $(EXEDIR)/PriorityQueue.o $(EXEDIR)/Graph.o $(EXEDIR)/AbstractAlgorithm.o
	$(CXX) $(CXXFLAGS)  -o $(EXEDIR)/MinSpanTreeAlgo.o -c $(SRCDIR)/MinSpanTreeAlgo.h $(LIBS) $(INCLUDE)

$(EXEDIR)/KruskalMSTAlg.o: KruskalMSTAlg.cpp $(EXEDIR)/MinSpanTreeAlgo.o $(EXEDIR)/PlainParser.o
	$(CXX) $(CXXFLAGS)  -o $(EXEDIR)/KruskalMSTAlg.o -c KruskalMSTAlg.cpp $(LIBS) $(INCLUDE)
		
$(EXEDIR)/KruskalMSTAlg: $(EXEDIR)/KruskalMSTAlg.o $(EXEDIR)/MinSpanTreeAlgo.o $(EXEDIR)/PlainParser.o $(EXEDIR)/PriorityQueue.o $(EXEDIR)/Graph.o $(EXEDIR)/AbstractAlgorithm.o
	$(CXX) $(CXXFLAGS)  -o $(EXEDIR)/KruskalMSTAlg $(EXEDIR)/KruskalMSTAlg.o $(EXEDIR)/PlainParser.o $(LIBS) $(INCLUDE)
	
#compile HexBoardGameApp
$(EXEDIR)/HexBoard.o: $(SRCDIR)/HexBoard.cpp $(EXEDIR)/Graph.o
	$(CXX) $(CXXFLAGS)  -o $(EXEDIR)/HexBoard.o -c $(SRCDIR)/HexBoard.cpp $(LIBS) $(INCLUDE)

$(EXEDIR)/Player.o: $(SRCDIR)/Player.cpp $(EXEDIR)/HexBoard.o $(EXEDIR)/MinSpanTreeAlgo.o
	$(CXX) $(CXXFLAGS)  -o $(EXEDIR)/Player.o -c $(SRCDIR)/Player.cpp $(LIBS) $(INCLUDE)

$(EXEDIR)/AbstractStrategy.o: $(SRCDIR)/AbstractStrategy.h $(SRCDIR)/AbstractStrategyImpl.h $(EXEDIR)/Player.o $(EXEDIR)/HexBoard.o
	$(CXX) $(CXXFLAGS)  -o $(EXEDIR)/AbstractStrategy.o -c $(SRCDIR)/AbstractStrategyImpl.cpp $(LIBS) $(INCLUDE)

$(EXEDIR)/Strategy.o: $(SRCDIR)/Strategy.cpp $(EXEDIR)/Player.o $(EXEDIR)/HexBoard.o $(EXEDIR)/PriorityQueue.o $(EXEDIR)/AbstractStrategy.o
	$(CXX) $(CXXFLAGS)  -o $(EXEDIR)/Strategy.o -c $(SRCDIR)/Strategy.cpp $(LIBS) $(INCLUDE)
	
$(EXEDIR)/GameTree.o: $(SRCDIR)/GameTree.h 
	$(CXX) $(CXXFLAGS)  -o $(EXEDIR)/GameTree.o -c $(SRCDIR)/GameTree.cpp $(LIBS) $(INCLUDE)
	
$(EXEDIR)/MonteCarloTreeSearch.o: $(EXEDIR)/Player.o $(EXEDIR)/HexBoard.o $(EXEDIR)/PriorityQueue.o $(EXEDIR)/AbstractStrategy.o $(EXEDIR)/GameTree.o
	$(CXX) $(CXXFLAGS)  -o $(EXEDIR)/MonteCarloTreeSearch.o -c $(SRCDIR)/MonteCarloTreeSearch.cpp $(LIBS) $(INCLUDE)
	
$(EXEDIR)/Game.o: $(SRCDIR)/Game.cpp $(EXEDIR)/Player.o $(EXEDIR)/HexBoard.o $(EXEDIR)/Strategy.o $(EXEDIR)/MonteCarloTreeSearch.o
	$(CXX) $(CXXFLAGS)  -o $(EXEDIR)/Game.o -c $(SRCDIR)/Game.cpp $(LIBS) $(INCLUDE)
	
$(EXEDIR)/HexBoardGameApp.o: HexBoardGameApp.cpp $(EXEDIR)/Game.o $(EXEDIR)/Player.o $(EXEDIR)/HexBoard.o 
	$(CXX) $(CXXFLAGS)  -o $(EXEDIR)/HexBoardGameApp.o -c HexBoardGameApp.cpp $(LIBS) $(INCLUDE)
	
$(EXEDIR)/HexBoardGameApp: $(EXEDIR)/HexBoardGameApp.o $(EXEDIR)/Game.o $(EXEDIR)/Player.o $(EXEDIR)/HexBoard.o
	$(CXX) $(CXXFLAGS)  -o $(EXEDIR)/HexBoardGameApp $(EXEDIR)/HexBoardGameApp.o $(EXEDIR)/Game.o $(EXEDIR)/Player.o $(EXEDIR)/HexBoard.o $(EXEDIR)/AbstractStrategy.o $(EXEDIR)/Strategy.o $(EXEDIR)/GameTree.o $(EXEDIR)/MonteCarloTreeSearch.o $(LIBS) $(INCLUDE)
