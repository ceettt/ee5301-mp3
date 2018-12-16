### Makefile for EE 5301 MP3 ###

CXXFLAGS	= -std=c++11 -Wall -Wextra -g
CXX		= g++ $(CXXFLAGS)


floorplan: floorplan.o util.o module.o graph.o 
	$(CXX) -o $@ $^

floorplan.o: floorplan.cpp module.hpp util.hpp
	$(CXX) -c $<

util.o: util.cpp module.hpp
	$(CXX) -c $<

module.o: module.cpp module.hpp
	$(CXX) -c $<

graph.o: graph.cpp graph.hpp
	$(CXX) -c $<

.PHONY: clean tarball

clean:
	rm -f *.o floorplan *~ *.txt *# *.csv

tarball: clean
	tar --exclude='.[^/]*' -zcvf ../MP3_chen5202.tgz ./
