SRC := 
INCLUDE := 
ARGS := -Wall -Wextra

build: ganita.cpp $(SRC) $(INCLUDE)
	g++ -I$(INCLUDE) $(SRC) $(ARGS) ganita.cpp -o ganita

run: program.gt build 
	./ganita program.gt
