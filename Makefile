CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

FLAGS =

LIBS =

SRCDIR =	src

EXEDIR =	bin

INCLUDE =	-I./$(SRCDIR)

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
	
$(EXEDIR)/PlainParser.o: $(SRCDIR)/PlainParser.cpp
	$(CXX) $(CXXFLAGS) $(FLAGS) -o $(EXEDIR)/PlainParser.o -c $(SRCDIR)/PlainParser.cpp $(LIBS) $(INCLUDE)
	
$(EXEDIR)/MinSpanTreeAlgo.o: $(SRCDIR)/MinSpanTreeAlgo.h $(EXEDIR)/PriorityQueue.o $(EXEDIR)/Graph.o
	$(CXX) $(CXXFLAGS) $(FLAGS) -o $(EXEDIR)/MinSpanTreeAlgo.o -c $(SRCDIR)/MinSpanTreeAlgo.h $(LIBS) $(INCLUDE)

$(EXEDIR)/KruskalMSTAlg.o: KruskalMSTAlg.cpp $(EXEDIR)/MinSpanTreeAlgo.o $(EXEDIR)/PlainParser.o
	$(CXX) $(CXXFLAGS) $(FLAGS) -o $(EXEDIR)/KruskalMSTAlg.o -c KruskalMSTAlg.cpp $(LIBS) $(INCLUDE)
		
$(EXEDIR)/KruskalMSTAlg: $(EXEDIR)/KruskalMSTAlg.o $(EXEDIR)/MinSpanTreeAlgo.o $(EXEDIR)/PlainParser.o $(EXEDIR)/PriorityQueue.o $(EXEDIR)/Graph.o
	$(CXX) $(CXXFLAGS) $(FLAGS) -o $(EXEDIR)/KruskalMSTAlg $(EXEDIR)/KruskalMSTAlg.o bin/PlainParser.o $(LIBS) $(INCLUDE)
	

all:	$(EXEDIR)/DijkstraAlg $(EXEDIR)/KruskalMSTAlg

clean:
	rm -rf  $(EXEDIR)/*
