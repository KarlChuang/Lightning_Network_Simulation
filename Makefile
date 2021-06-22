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

test_50-100000:
	./bin/main ./test/50-100000/default_50.in
	./bin/main ./test/50-100000/opt_50.in
	python3.6 ./data_process/merge.py ./test/50-100000

test_%:
	./bin/main ./test/$*/default_10.in
	./bin/main ./test/$*/default_20.in
	./bin/main ./test/$*/default_50.in
	./bin/main ./test/$*/opt_10.in
	./bin/main ./test/$*/opt_20.in
	./bin/main ./test/$*/opt_50.in
	python3.6 ./data_process/merge.py ./test/$*

clean:
	@rm -rf ./bin/*
	@touch ./bin/.gitkeep
	@rm -rf ./obj/*.o
