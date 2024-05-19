SRC := 
INCLUDE := 
ARGS := -Wall -Wextra

build: ganita.cpp $(SRC) $(INCLUDE)
	g++ -I$(INCLUDE) $(SRC) $(ARGS) ganita.cpp -o ganita

compile: program.gt build
	./ganita -c -o app program.gt

run: program.gt build 
	./ganita program.gt
