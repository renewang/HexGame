CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

FLAGS =

OBJS =		DijkstraAlg.o

LIBS =

TARGET =	DijkstraAlg

SRCDIR =	src

EXEDIR =	bin

INCLUDE =	-I./$(SRCDIR)

$(EXEDIR)/Graph.o: $(SRCDIR)/Graph.h
	$(CXX) $(CXXFLAGS) $(FLAGS) -o $(EXEDIR)/Graph.o -c $(SRCDIR)/Graph.h $(LIBS)

$(EXEDIR)/PriorityQueue.o: $(SRCDIR)/PriorityQueue.h $(EXEDIR)/Graph.o
	$(CXX) $(CXXFLAGS) $(FLAGS) -o $(EXEDIR)/PriorityQueue.o -c $(SRCDIR)/PriorityQueue.h $(LIBS) $(INCLUDE)

$(EXEDIR)/ShortestPathAlgo.o: $(SRCDIR)/ShortestPathAlgo.h $(EXEDIR)/Graph.o $(EXEDIR)/PriorityQueue.o
	$(CXX) $(CXXFLAGS) $(FLAGS) -o $(EXEDIR)/ShortestPathAlgo.o $(SRCDIR)/ShortestPathAlgo.h $(LIBS) $(INCLUDE)

$(EXEDIR)/$(OBJS): DijkstraAlg.cpp $(EXEDIR)/ShortestPathAlgo.o $(EXEDIR)/PriorityQueue.o $(EXEDIR)/Graph.o
	$(CXX) $(CXXFLAGS) $(FLAGS) -o $(EXEDIR)/$(OBJS) -c DijkstraAlg.cpp $(LIBS) $(INCLUDE)
	
$(EXEDIR)/$(TARGET):	$(EXEDIR)/$(OBJS)
	$(CXX) $(CXXFLAGS) $(FLAGS) -o $(EXEDIR)/$(TARGET) $(EXEDIR)/$(OBJS) $(LIBS) $(INCLUDE)

all:	$(EXEDIR)/$(TARGET)

clean:
	rm -rf  $(EXEDIR)/*
