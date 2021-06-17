opts=-Wall
def=

all: main

main: obj/simlib.o obj/graph.o obj/iostream.o obj/main.o obj/normal.o
	g++ $(opts) $(def) $^ -o bin/$@

obj/graph.o: src/graph.cpp header/graph.h header/simlib.h
	g++ -I./header $(opts) $(def) -c $< -o $@

obj/iostream.o: src/iostream.cpp header/graph.h
	g++ -I./header $(opts) $(def) -c $< -o $@

obj/normal.o: src/normal.cpp header/normal.h header/simlib.h
	g++ -I./header $(opts) $(def) -c $< -o $@

obj/%.o: src/%.cpp
	g++ -I./header $(opts) $(def) -c $< -o $@

obj/%.o: src/%.cpp header/%.h
	g++ -I./header $(opts) $(def) -c $< -o $@

clean:
	@rm -rf ./bin/*
	@touch ./bin/.gitkeep
	@rm -rf ./obj/*.o
