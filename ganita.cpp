#include <iostream>
#include <fstream>
#include <assert.h>
#include <vector>
#include <string>

#define printp(x) std::cout<<x
#define println(x) std::cout<<x<<std::endl
#define printerr(x) std::cerr<<x<<std::endl

enum OP_TYPE {
	LITERAL,
	KEYWORD
};

enum OP_CODE {
	PLUS,
	MINUS,
	DIVIDE,
	MUL,
	PRINT,
	DUP,
	DUP2,
	EQUAL,
	COUNT // used to count total number of operation introduced to exhaust all switch cases
};

struct Token {
	int value;
	int position[2]; // store location of first charactor of the token in [row, column]
	OP_TYPE type;
	int data[3]; // store cross-referenced data. Example: for `IF` it will store location of `ELSE` and `END`
};

bool isValid(std::string& str)
{
	for (char ch : str) {
	    int v = ch;
	    if (!(ch >= 48 && ch <= 57)) {
		return false;
	    }
	}
	return true; 
}

Token parse(std::string token, int row, int col){
	assert(COUNT == 8 && "Mismatching number of keyword in tokenize()");

	Token t = {
		.value = -1,
		.position = {row, col},
		.type = KEYWORD,
		.data = {0}	
	};
	
	      if (token == "+"){
		t.value = PLUS;
	}else if (token == "-"){
		t.value = MINUS;
	}else if (token == "*"){
		t.value = MUL;
	}else if (token == "/"){
		t.value = DIVIDE;
	}else if (token == "PRINT"){
		t.value = PRINT;
	}else if (token == "DUP"){
		t.value = DUP;
	}else if (token == "2DUP"){
		t.value = DUP2;
	}else if (token == "="){
		t.value = EQUAL;
	}else{
		if (isValid(token)){
			t.value = std::stoi(token);
			t.type  = LITERAL;
		}
		else {
			printerr("Unknown token at line(" << row << "," << col<< ")");
			exit(1);
		}
	}

	return t;
}

std::vector<Token> parseLine(std::string line, int row){
	std::vector<Token> tokens;
	std::string token;
	line+= " ";
	for (int i=0; i < (int)line.size(); i++){
		char t = line[i];
		if (t == 32 || t == '\t' || t == ',' || t == '(' || t == ')'){
			if (!token.size() > 0){
				continue;
			}
			tokens.push_back(parse(token, row, i));
			token = "";
			continue;
		}
		token += t;
	}
	return tokens;
}

int pop(std::vector<int>& program){
	int value = program[program.size() - 1];
	program.pop_back();
	return value;
}

void execute(std::vector<Token> program){
	std::vector<int> exect;
	for(int i=0; i < (int)program.size(); i++){
		Token token = program[i];
		if (token.type == LITERAL){;
			exect.push_back(token.value);
		}
		if (token.type == KEYWORD){
			assert(COUNT == 8 && "Mismatching number of keyword in execute()");
			switch(token.value){
				case PLUS:
					{
						int a = pop(exect);
						int b = pop(exect);
						exect.push_back(a + b);
						break;
					}
				case MINUS:
					{
						int a = pop(exect);
						int b = pop(exect);
						exect.push_back(b - a);
						break;
					}
				case DIVIDE:
					{
						int a = pop(exect);
						int b = pop(exect);
						exect.push_back(b/a);
						break;
					}
				case MUL:
					{
						int a = pop(exect);
						int b = pop(exect);
						exect.push_back(b * a);
						break;
					}
				case PRINT:
					{
						int a = pop(exect);
						println(a);
						break;
					}
				case DUP:
					{
						int a = pop(exect);
						exect.push_back(a);
						exect.push_back(a);
						break;
					}
				case DUP2:
					{
						int a = pop(exect);
						int b = pop(exect);
						exect.push_back(b);
						exect.push_back(a);
						exect.push_back(b);
						exect.push_back(a);
						break;
					}
				case EQUAL:
					{
						int a = pop(exect);
						int b = pop(exect);
						exect.push_back(a == b);
						break;
					}
				default:
					printp("Invalid Command");
					exit(1);
					break;
			}	
		}
	}
}

int main(int argc, char* argv[]){
	
	assert( argc >= 2 );
	std::string filename = argv[1];
	std::vector<Token> program; // program stores all tokens to evalute for interpreting or compilation
	
	std::ifstream file(filename);
	if (!file.is_open()) { 
		printerr("Could not open file: " << filename); 
		return 1; 
	} 
		
	int row = 0;
	std::string line;
	while(getline(file, line)){
		std::vector<Token> tokens = parseLine(line,row);
		for(Token token : tokens){
			program.push_back(token);
		}
		row++;
	}
	execute(program);
}

