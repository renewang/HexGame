CXXFLAGS =	-O0 -g -Wall -fmessage-length=0

FLAGS =	-std=c++11 

LIBS =

SRCDIR =	src

EXEDIR =	bin

INCLUDE =	-I./$(SRCDIR)

#compile DijkstraAlg exe
$(EXEDIR)/Graph.o: $(SRCDIR)/Graph.h
	$(CXX) $(CXXFLAGS) $(FLAGS) -o $(EXEDIR)/Graph.o -c $(SRCDIR)/Graph.h $(LIBS)

$(EXEDIR)/PriorityQueue.o: $(SRCDIR)/PriorityQueue.h $(EXEDIR)/Graph.o
	$(CXX) $(CXXFLAGS) $(FLAGS) -o $(EXEDIR)/PriorityQueue.o -c $(SRCDIR)/PriorityQueue.h $(LIBS) $(INCLUDE)

$(EXEDIR)/ShortestPathAlgo.o: $(SRCDIR)/ShortestPathAlgo.h $(EXEDIR)/Graph.o $(EXEDIR)/PriorityQueue.o
	$(CXX) $(CXXFLAGS) $(FLAGS) -o $(EXEDIR)/ShortestPathAlgo.o $(SRCDIR)/ShortestPathAlgo.h $(LIBS) $(INCLUDE)

$(EXEDIR)/DijkstraAlg.o: DijkstraAlg.cpp $(EXEDIR)/ShortestPathAlgo.o $(EXEDIR)/PriorityQueue.o $(EXEDIR)/Graph.o
	$(CXX) $(CXXFLAGS) $(FLAGS) -o $(EXEDIR)/DijkstraAlg.o -c DijkstraAlg.cpp $(LIBS) $(INCLUDE)
	
$(EXEDIR)/DijkstraAlg:	$(EXEDIR)/DijkstraAlg.o
	$(CXX) $(CXXFLAGS) $(FLAGS) -o $(EXEDIR)/DijkstraAlg $(EXEDIR)/DijkstraAlg.o $(LIBS) $(INCLUDE)

#compile KruskalMSTAlg	
$(EXEDIR)/PlainParser.o: $(SRCDIR)/PlainParser.cpp
	$(CXX) $(CXXFLAGS) $(FLAGS) -o $(EXEDIR)/PlainParser.o -c $(SRCDIR)/PlainParser.cpp $(LIBS) $(INCLUDE)
	
$(EXEDIR)/MinSpanTreeAlgo.o: $(SRCDIR)/MinSpanTreeAlgo.h $(EXEDIR)/PriorityQueue.o $(EXEDIR)/Graph.o
	$(CXX) $(CXXFLAGS) $(FLAGS) -o $(EXEDIR)/MinSpanTreeAlgo.o -c $(SRCDIR)/MinSpanTreeAlgo.h $(LIBS) $(INCLUDE)

$(EXEDIR)/KruskalMSTAlg.o: KruskalMSTAlg.cpp $(EXEDIR)/MinSpanTreeAlgo.o $(EXEDIR)/PlainParser.o
	$(CXX) $(CXXFLAGS) $(FLAGS) -o $(EXEDIR)/KruskalMSTAlg.o -c KruskalMSTAlg.cpp $(LIBS) $(INCLUDE)
		
$(EXEDIR)/KruskalMSTAlg: $(EXEDIR)/KruskalMSTAlg.o $(EXEDIR)/MinSpanTreeAlgo.o $(EXEDIR)/PlainParser.o $(EXEDIR)/PriorityQueue.o $(EXEDIR)/Graph.o
	$(CXX) $(CXXFLAGS) $(FLAGS) -o $(EXEDIR)/KruskalMSTAlg $(EXEDIR)/KruskalMSTAlg.o $(EXEDIR)/PlainParser.o $(LIBS) $(INCLUDE)
	
#compile HexBoardGameApp
$(EXEDIR)/HexBoard.o: $(SRCDIR)/HexBoard.cpp $(EXEDIR)/Graph.o
	$(CXX) $(CXXFLAGS) $(FLAGS) -o $(EXEDIR)/HexBoard.o -c $(SRCDIR)/HexBoard.cpp $(LIBS) $(INCLUDE)

$(EXEDIR)/Player.o: $(SRCDIR)/Player.cpp $(EXEDIR)/HexBoard.o $(EXEDIR)/MinSpanTreeAlgo.o
	$(CXX) $(CXXFLAGS) $(FLAGS) -o $(EXEDIR)/Player.o -c $(SRCDIR)/Player.cpp $(LIBS) $(INCLUDE)
	
$(EXEDIR)/Game.o: $(SRCDIR)/Game.cpp $(EXEDIR)/Player.o $(EXEDIR)/HexBoard.o
	$(CXX) $(CXXFLAGS) $(FLAGS) -o $(EXEDIR)/Game.o -c $(SRCDIR)/Game.cpp $(LIBS) $(INCLUDE)
	
$(EXEDIR)/HexBoardGameApp.o: HexBoardGameApp.cpp $(EXEDIR)/Game.o $(EXEDIR)/Player.o $(EXEDIR)/HexBoard.o
	$(CXX) $(CXXFLAGS) $(FLAGS) -o $(EXEDIR)/HexBoardGameApp.o -c HexBoardGameApp.cpp $(LIBS) $(INCLUDE)
	
$(EXEDIR)/HexBoardGameApp: $(EXEDIR)/HexBoardGameApp.o $(EXEDIR)/Game.o $(EXEDIR)/Player.o $(EXEDIR)/HexBoard.o
	$(CXX) $(CXXFLAGS) $(FLAGS) -o $(EXEDIR)/HexBoardGameApp $(EXEDIR)/HexBoardGameApp.o $(EXEDIR)/Game.o $(EXEDIR)/Player.o $(EXEDIR)/HexBoard.o $(LIBS) $(INCLUDE)

all:	$(EXEDIR)/DijkstraAlg $(EXEDIR)/KruskalMSTAlg $(EXEDIR)/HexBoardGameApp

clean:
	rm -rf  $(EXEDIR)/*
