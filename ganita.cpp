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
	MOD,

	GT,
	ST,
	EQUAL,

	NOT,
	//TODO: implement AND and OR

	PRINT,
	DUP,
	DUP2,

	IFF,	
	ELCE,
	WILE,
	DO,
	END,

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

//TODO: check whether any un-IF ELSE or un-IF END is present and throw valid error
//TODO: check whether WHILE has DO and END statements and if not throw valid error
//Cross-references keywords that require jump in code like IF ELSE WHILE etc
int crossrefIndice(std::vector<Token>& program){
	assert(COUNT == 17 && "Mismatching number of keyword in crossrefIndices()");
	for(int i=0; i < (int)program.size(); i++){
		Token token = program[i];
		switch(token.value){
			case IFF:
			{	
				int index = i + 1;
				bool flag = false;
				int bCounter = 0; //counts number of nested IF blocks to leave to find respective END or ELSE
				while(index < (int)program.size()){
					OP_TYPE type = program[index].type;
					if (type == KEYWORD){
						OP_CODE value = (OP_CODE)program[index].value;
						if ((value == END || value == ELCE) && bCounter == 0){
							program[i].data[0] = index;
							flag = true;
							break;
						}
						else {
							if(value == IFF){
								bCounter++;	
							}
							if(value == END){
								bCounter--;
							}
						}
					}
					index++;
				}
				if(!flag){
					printerr("IF block should always end with END! at line(" << token.position[0] + 1 << "," <<token.position[1] - 1 << "0");
					exit(1);
				}
				break;
			}
			case ELCE:
			{
			
				int index = i + 1;
				bool flag = false;
				int bCounter = 0; //counts number of nested ELSE blocks to leave to find respective END
				while(index < (int)program.size()){
					OP_TYPE type = program[index].type;
					if (type == KEYWORD){
						OP_CODE value = (OP_CODE)program[index].value;
						if (value == END && bCounter == 0){
							program[i].data[0] = index;
							flag = true;
							break;
						}
						else {
							if(value == IFF || value == ELCE){
								bCounter++;	
							}
							if(value == END){
								bCounter--;
							}
						}
					}
					index++;
				}
				if(!flag){
					printerr("ELSE block should always end with END! at line(" << token.position[0] + 1 << "," <<token.position[1] - 1<< "0");
					exit(1);
				}
				break;
			}
			case END:
			{
				
				int index = i - 1;
				bool flag = false;
				int bCounter = 0; //counts number of nested IF/WHILE blocks to leave to find respective respective while
				while(index >= 0){
					OP_TYPE type = program[index].type;
					if (type == KEYWORD){
						OP_CODE value = (OP_CODE)program[index].value;
						if (value == WILE && bCounter == 0){
							program[i].data[0] = index;
							flag = true;
							break;
						}
						else {
							if(value == IFF || value == WILE){
								bCounter++;	
							}
							if(value == END){
								bCounter--;
							}
						}
					}
					index--;
				}
				break;
			}
			case DO:
			{
				int index = i + 1;
				bool flag = false;
				int bCounter = 0; //counts number of nested IF/WHILE blocks to leave to find respective END
				while(index < (int)program.size()){
					OP_TYPE type = program[index].type;
					if (type == KEYWORD){
						OP_CODE value = (OP_CODE)program[index].value;
						if (value == END && bCounter == 0){
							program[i].data[0] = index;
							flag = true;
							break;
						}
						else {
							if(value == IFF || value == WILE){
								bCounter++;	
							}
							if(value == END){
								bCounter--;
							}
						}
					}
					index++;
				}
				if(!flag){
					printerr("WHILE block should always end with END! at line(" << token.position[0] + 1 << "," <<token.position[1] - 1<< ")");
					exit(1);
				}
				break;
			}
			default:
			{
			}
		}
	}
	return 0;
}

Token parse(std::string token, int row, int col){
	assert(COUNT == 17 && "Mismatching number of keyword in tokenize()");

	Token t = {
		.value = -1,
		.position = {row, col},
		.type = KEYWORD,
		.data = {-1} // -1 indicates that this data is not being used
	};
	
	      if (token == "+"){
		t.value = PLUS;
	}else if (token == "-"){
		t.value = MINUS;
	}else if (token == "*"){
		t.value = MUL;
	}else if (token == "/"){
		t.value = DIVIDE;
	}else if (token == "%"){
		t.value = MOD;
	}else if (token == "NOT"){
		t.value = NOT;
	}else if (token == "PRINT"){
		t.value = PRINT;
	}else if (token == "DUP"){
		t.value = DUP;
	}else if (token == "2DUP"){
		t.value = DUP2;
	}else if (token == "="){
		t.value = EQUAL;
	}else if (token == "IF"){
		t.value = IFF;
	}else if (token == "END"){
		t.value = END;
	}else if (token == "ELSE"){
		t.value = ELCE;
	}else if (token == "WHILE"){
		t.value = WILE;
	}else if (token == "DO"){
		t.value = DO;
	}else if (token == ">"){
		t.value = GT;
	}else if (token == "<"){
		t.value = ST;
	}else{
		if (isValid(token)){
			t.value = std::stoi(token);
			t.type  = LITERAL;
		}
		else {
			printerr("Unknown token at line(" << row + 1 << "," << col - 1<< ")");
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
			if (token.rfind("//", 0) == 0){
				return tokens;	
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

	assert(COUNT == 17 && "Mismatching number of keyword in execute()");

	std::vector<int> memory;

	for(int i=0; i < (int)program.size(); i++){
		Token token = program[i];
		if (token.type == LITERAL){;
			memory.push_back(token.value);
		}
		if (token.type == KEYWORD){
			switch(token.value){
				case PLUS:
					{
						int a = pop(memory);
						int b = pop(memory);
						memory.push_back(a + b);
						break;
					}
				case MINUS:
					{
						int a = pop(memory);
						int b = pop(memory);
						memory.push_back(b - a);
						break;
					}
				case DIVIDE:
					{
						int a = pop(memory);
						int b = pop(memory);
						memory.push_back(b/a);
						break;
					}
				case MUL:
					{
						int a = pop(memory);
						int b = pop(memory);
						memory.push_back(b * a);
						break;
					}
				case MOD:
					{
						int a = pop(memory);
						int b = pop(memory);
						memory.push_back(b % a);
						break;
					}
				case NOT:
					{
						int a = pop(memory);
						memory.push_back(!a);
						break;
					}
				case PRINT:
					{
						int a = pop(memory);
						println(a);
						break;
					}
				case DUP:
					{
						int a = pop(memory);
						memory.push_back(a);
						memory.push_back(a);
						break;
					}
				case DUP2:
					{
						int a = pop(memory);
						int b = pop(memory);
						memory.push_back(b);
						memory.push_back(a);
						memory.push_back(b);
						memory.push_back(a);
						break;
					}
				case EQUAL:
					{
						int a = pop(memory);
						int b = pop(memory);
						memory.push_back(a == b);
						break;
					}
				case IFF:
					{
						int a = pop(memory);
						if (!a){
							i = token.data[0]; // It shifts pointer to next line of ELSE or END block
							
						}
						break;
					}
				case END:
					{
						if (token.data[0] >= 0){
							i = token.data[0] - 1; // shifts pointer to WHILE
						}
						break;
					}
				case ELCE:
					{	
						i = token.data[0]; // it shifts pointer to next line of END block
						break;
					}
				case DO:
					{
						int a = pop(memory);
						if (!a){
							i = token.data[0]; // shifts pointer to next line of END block
						}
						break;
					}
				case WILE:
					{
						break;
					}
				case GT:
					{

						int a = pop(memory);
						int b = pop(memory);
						memory.push_back(b > a);
						break;
					}
				case ST:
					{

						int a = pop(memory);
						int b = pop(memory);
						memory.push_back(b < a);
						break;
					}
				default:
					assert(false && "Undeteced invalid token found!");
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

	crossrefIndice(program);
	
/* To visualize all tokens and thier data
	for (Token t : program){
		println(t.type << "," << t.value << "," << t.data[0]);
	}
*/

	execute(program);
}

