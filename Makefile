SRC := 
INCLUDE := 
ARGS := -Wall -Wextra

build: lexer.cpp $(SRC) $(INCLUDE)
	g++ -I$(INCLUDE) $(SRC) $(ARGS) lexer.cpp -o lexer

run: program.gt build 
	./lexer program.gt
