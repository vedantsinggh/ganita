#include <iomanip>
#include <cstdint>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <assert.h>
#include <vector>
#include <string>
#include <cstring>

#define printp(x) std::cout<<x
#define println(x) std::cout<<x<<std::endl
#define printerr(x) std::cerr<<x<<std::endl

/*  --------- INTERPRETER AND COMPILER DOCUMENTATION ---------
 *  
 *  We have a variable program that stores whole program as vector of tokens
 *
 *  Token holds whole data of token
 *  	.value	   : it stores value of literal or OP_CODE in case of keyword
 *	.type      : it stores type of token
 *	.position  : it stores the location of first character of keyword. it is used to throw compile time error
 *	.data	   : it stores any data required by any keyword:
 *			for IF     : data[0] holds position of it's respective ELSE block(or END block in case there is no ELSE block)
 *			for ELSE   : data[0] holds position of it's respective END block
 *			for WHILE  : data[0] holds position of it's respective END block
 *			for END    : data[0] holds position of WHILE block only.
 *
 *
 *	How Do We Branch?
 *		IF of the condition it true then it continues its execution linearly but if the condition is false it jumps to next line of either ELSE or END.
 *		ELSE it always jumps the execution to next line of END block.
 *		END in conditional branching is always used as marker. It has not functionality.
 *
 *	How Do We Loop?
 *		WHILE is always used as mark from where the looping begins .
 *		DO check the last bit to be either true or false. In case of true it continues the linear execution otherwise it jumps the execution to next line of END.
 *		END always jumps execution to while loop thus inferring re-evaluation of condition forming a conditional loop.
 *
 * */


// It is used to define type of token pushed onto the stack
enum OP_TYPE {
	LITERAL,
	KEYWORD,
	STRING,
};


// This defines all possible keywords
enum OP_CODE {

	// arithmetic operations
	PLUS,
	MINUS,
	DIVIDE,
	MUL,
	MOD,

	// comparison operations
	GT,
	ST,
	EQUAL,

	// logical operations
	NOT,
	AND,
	OR,

	// other operations
	PRINT, // pops the last element and prints it to the console
	DUP, // copies last element onto the stack
	DUP2, // copies last two element onto the stack
	MACRO,
	SWAP,
	OVER,

	// conditional and looping operations
	IFF,	
	ELCE,
	WILE,
	DO,
	END,

	COUNT // used to count total number of operation introduced to exhaust all switch cases
};

std::string getOPCODE(int index){
	
	std::string names[] = {
	"PLUS",
	"MINUS",
	"DIVIDE",
	"MUL",
	"MOD",
	"GT",
	"ST",
	"EQUAL",
	"NOT",
	"AND",
	"OR",
	"PRINT",
	"DUP",
	"DUP2",
	"MACRO",
	"SWAP",
	"OVER",
	"IFF",	
	"ELCE",
	"WILE",
	"DO",
	"END"
	 };
	return names[index];
}

// it represents every kind of valid word that would be coded in the program. Currently it could be either a number[OP_TYPE  = literal] that should be pushed onto the stack or a operation [OP_TYPE = KEYWORD]
struct Token {
	unsigned long long int value;
	int position[2]; // store location of first character of the token in [row, column]
	OP_TYPE type;
	uintptr_t address; // for tokens that require address to be stored
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

//Cross-references keywords that require jump in code like IF ELSE WHILE etc and stores them in .data section of respective token

int crossrefIndice(std::vector<Token>& program){
	assert(COUNT == 22 && "Mismatching number of keyword in crossrefIndices()");
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
					printerr("IF block should always end with `END` at line(" << token.position[0] + 1 << "," <<token.position[1] - 1 << "0");
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
					printerr("ELSE block should always end with `END` at line(" << token.position[0] + 1 << "," <<token.position[1] - 1<< "0");
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
					printerr("WHILE block should always end with `END` at line(" << token.position[0] + 1 << "," <<token.position[1] - 1<< ")");
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
	assert(COUNT == 22 && "Mismatching number of keyword in tokenize()");

	Token t = {
		.value = 0,
		.position = {row, col},
		.type = KEYWORD,
		.address = 0,
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
	}else if (token == "AND"){
		t.value = AND;
	}else if (token == "OR"){
		t.value = OR;
	}else if (token == "MACRO"){
		t.value = MACRO;
	}else if (token == "SWAP"){
		t.value = SWAP;
	}else if (token == "OVER"){
		t.value = OVER;
	}else{
		if (isValid(token)){
			t.value = std::stoi(token);
			t.type  = LITERAL;
		}
		else {

			if (token.rfind("\"", 0) == 0){

				char* copy = new char[token.size()] + 1;
				const char* k =  token.c_str();
				std::strcpy(copy,k);

				void* ptr = (void *) &(copy);
				uintptr_t address = (uintptr_t)(ptr);
				const char* s= (const char*)(address);

				t.value = token.size();
				t.address = address;//should be pointer to first character of the string
				t.type = STRING;
			}else{
				printerr("Unknown token " << token << " at line(" << row + 1 << "," << col - 1<< ")");
				exit(1);
			}
		}
	}

	return t;
}

std::vector<std::string> macros;
std::vector<std::string> macros_rep;

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

			bool isMacro = false;
			for (int k=0; k<(int)macros.size(); k++){
				if (macros[k] == token){
					isMacro = true;
					std::string addLine = macros_rep[k];
					std::vector<Token> addToken = parseLine(addLine, row);
					tokens.insert(tokens.end(),addToken.begin(), addToken.end());
				}
			}


			if (isMacro){
				token = "";
				continue;
			}

			Token res = parse(token, row, i);

			//TODO: add bound check and other check necessary in macro
			//TODO: add multi-line macro
			if (res.type == KEYWORD && res.value == MACRO){

				std::string mac_word = "";
				i = i+1;
				while (true){
					char ch = line[i]; 
					if (ch == ' '){
						break;
					}
					mac_word += ch;
					i++;
				}
				macros.push_back(mac_word);
				i++;	
				std::string mac_rep_word;
				std::string mac_rep_str;
				while (i < (int)line.size()){
					if (line[i] == ' '){
						if(mac_rep_word == "END"){
							break;
						}
						else{
							mac_rep_str += mac_rep_word + " ";
							mac_rep_word = "";
						}
					}else{
						mac_rep_word += line[i];
					}
					i++;
				}
				macros_rep.push_back(mac_rep_str);
			}else {
				tokens.push_back(res);
				token = "";
				continue;
			}
		}
		else if (t == '\"'){
			i++;
			token += t;
			while (i < (int)line.size()){
				char c = line[i];
				if (c == '\"'){
					break;
				}
				token += c;
				i++;
			}
		}else{
			token += t;
		}
	}
	return tokens;
}

int pop(std::vector<int>& program){
	int value = program[program.size() - 1];
	program.pop_back();
	return value;
}

void execute(std::vector<Token> program){

	assert(COUNT == 22 && "Mismatching number of keyword in execute()");

	std::vector<int> stack;

	for(int i=0; i < (int)program.size(); i++){
		Token token = program[i];
		if (token.type == LITERAL){;
			stack.push_back(token.value);
		}
		if (token.type == STRING){
			stack.push_back(token.value);
			//TODO: implement string operations
			assert(false && "String is still unimplemented");
		}
		if (token.type == KEYWORD){
			switch(token.value){
				case PLUS:
					{
						int a = pop(stack);
						int b = pop(stack);
						stack.push_back(a + b);
						break;
					}
				case MINUS:
					{
						int a = pop(stack);
						int b = pop(stack);
						stack.push_back(b - a);
						break;
					}
				case DIVIDE:
					{
						int a = pop(stack);
						int b = pop(stack);
						stack.push_back(b/a);
						break;
					}
				case MUL:
					{
						int a = pop(stack);
						int b = pop(stack);
						stack.push_back(b * a);
						break;
					}
				case MOD:
					{
						int a = pop(stack);
						int b = pop(stack);
						stack.push_back(b % a);
						break;
					}
				case NOT:
					{
						int a = pop(stack);
						stack.push_back(!a);
						break;
					}
				case PRINT:
					{
						int a = pop(stack);
						println(a);
						break;
					}
				case DUP:
					{
						int a = pop(stack);
						stack.push_back(a);
						stack.push_back(a);
						break;
					}
				case DUP2:
					{
						int a = pop(stack);
						int b = pop(stack);
						stack.push_back(b);
						stack.push_back(a);
						stack.push_back(b);
						stack.push_back(a);
						break;
					}
				case EQUAL:
					{
						int a = pop(stack);
						int b = pop(stack);
						stack.push_back(a == b);
						break;
					}
				case IFF:
					{
						int a = pop(stack);
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
						int a = pop(stack);
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

						int a = pop(stack);
						int b = pop(stack);
						stack.push_back(b > a);
						break;
					}
				case ST:
					{

						int a = pop(stack);
						int b = pop(stack);
						stack.push_back(b < a);
						break;
					}
				case AND:
					{
						int a = pop(stack);
						int b = pop(stack);
						stack.push_back(b && a);
						break;
					}
				case OR:
					{
						int a = pop(stack);
						int b = pop(stack);
						stack.push_back(b || a);
						break;
					}
				case OVER:
					{
						int a = pop(stack);
						int b = pop(stack);
						stack.push_back(b);
						stack.push_back(a);
						stack.push_back(b);
						break;
					}
				case SWAP:
					{
						int a = pop(stack);
						int b = pop(stack);
						stack.push_back(a);
						stack.push_back(b);
						break;
					}
				case MACRO:
					{
						assert(false && "unexpectedly encountered macro!");	
					}
				default:
					assert(false && "Undetected invalid token found!");
					exit(1);
					break;
			}	
		}
	}
}

void compile(std::vector<Token> program, std::string output_file){
	
	std::ofstream fptr (output_file + ".asm");
#define insert(x) fptr << x << std::endl
	insert("global _start");
	insert("section .data");
	insert("	dot db \".\", 0");
	insert("	newline db 10");
	insert("section .bss");
	insert("	intBUFF: resq 30");
	insert("	intBUFFptr: resq 1");
	insert("	decCHARbuff: resb 1");
	insert("section .text");
	insert("_start:");

	for(int i=0; i < (int)program.size(); i++){
		Token token = program[i];
		if (token.type == LITERAL){;
			fptr<<"	mov r8, __float64__("<<token.value<<".0"<<")"<<std::endl;  //converting int to float  
			insert("	push r8");
			insert("	xor r8, r8");
		}
		if (token.type == KEYWORD){
			switch(token.value){
				case PLUS:
				{
					insert("	pop rax");
					insert("	movq xmm0, rax");
					insert("	pop rbx");
					insert("	movq xmm1, rbx");
					insert("	addsd xmm0, xmm1");
					insert("	movq r8, xmm0");
					insert("	push r8");
					insert("	xor rax, rax");	
					insert("	xor rbx, rbx");	
					break;
				}
				case MINUS:
				{
					insert("	pop rax");
					insert("	movq xmm0, rax");
					insert("	pop rbx");
					insert("	movq xmm1, rbx");
					insert("	subsd xmm0, xmm1");
					insert("	movq r8, xmm0");
					insert("	push r8");
					insert("	xor rax, rax");	
					insert("	xor rbx, rbx");	
					break;
				}
				case DIVIDE:
				{
					insert("	pop rax");
					insert("	movq xmm1, rax");
					insert("	pop rbx");
					insert("	movq xmm0, rbx");
					insert("	divsd xmm0, xmm1");
					insert("	movq r8, xmm0");
					insert("	push r8");
					insert("	xor rax, rax");	
					insert("	xor rbx, rbx");	
					break;
				}
				case MUL:
				{	
					insert("	pop rax");
					insert("	movq xmm0, rax");
					insert("	pop rbx");
					insert("	movq xmm1, rbx");
					insert("	mulsd xmm0, xmm1");
					insert("	movq r8, xmm0");
					insert("	push r8");
					insert("	xor rax, rax");	
					insert("	xor rbx, rbx");	
					break;
				}
				case MOD:
				{	
					insert("	pop rax");
					insert("	movq xmm1, rax");
					insert("	xor rax, rax");
					insert("	pop rbx");
					insert("	movq xmm0, rbx");
					insert("	cvtsd2si rax, xmm0");
					insert("	cvtsd2si rbx, xmm1");
					insert("	div rbx");
					insert("	cvtsi2sd xmm0, rdx");
					insert("	movq rax, xmm0");
					insert("	push rax");
					insert("	xor rax, rax");
					insert("	xor rbx, rbx");
					insert("	xor rdx, rdx");
					break;
				}                                                              
				case DUP:
				{
					insert("	pop rbx");
					insert("	push rbx");
					insert("	push rbx");
					insert("	xor rbx, rbx");
					break;
					
				}
				case DUP2:
				{
					insert("	pop rbx");
					insert("	pop rcx");
					insert("	push rcx");
					insert("	push rbx");
					insert("	push rcx");
					insert("	push rbx");
					insert("	xor rbx, rbx");
					insert("	xor rcx, rcx");
					break;
				}
				
				case PRINT:
				{
					insert("	pop rax");
					insert("	call _printRAXfloat");
					insert("	mov rax, 1");
					insert("	mov rdi, 1");
					insert("	mov rsi, newline");
					insert("	mov rdx, 1");
					insert("	syscall");
					break;
				}
				
				case GT:
				{
					insert("	pop r8");
					insert("	pop r9");				
					insert("	cmp r9, r8");				
					fptr<<"	jg _gt"<<token.position[0]<<token.position[1]<<std::endl;				
					insert("	mov rax, __float64__(0.0)");				
					insert("	push rax");
					fptr<<"	jmp _gtend"<<token.position[0]<<token.position[1]<<std::endl;				
					fptr<<"_gt"<<token.position[0]<<token.position[1]<<":"<<std::endl;				
					insert("	mov rax, __float64__(1.0)");				
					insert("	push rax");				
					fptr<<"_gtend"<<token.position[0]<<token.position[1]<<":"<<std::endl;				
					break;
				}
				case ST:
				{
					insert("	pop r8");
					insert("	pop r9");				
					insert("	cmp r9, r8");				
					fptr<<"	jl _lt"<<token.position[0]<<token.position[1]<<std::endl;				
					insert("	mov rax, __float64__(0.0)");				
					insert("	push rax");
					fptr<<"	jmp _ltend"<<token.position[0]<<token.position[1]<<std::endl;				
					fptr<<"_lt"<<token.position[0]<<token.position[1]<<":"<<std::endl;				
					insert("	mov rax, __float64__(1.0)");				
					insert("	push rax");				
					fptr<<"_ltend"<<token.position[0]<<token.position[1]<<":"<<std::endl;				
					break;
				}
				case EQUAL:
				{
					insert("	pop r8");
					insert("	pop r9");				
					insert("	cmp r9, r8");				
					fptr<<"	je _eq"<<token.position[0]<<token.position[1]<<std::endl;				
					insert("	mov rax, __float64__(0.0)");				
					insert("	push rax");
					fptr<<"	jmp _eqend"<<token.position[0]<<token.position[1]<<std::endl;				
					fptr<<"_eq"<<token.position[0]<<token.position[1]<<":"<<std::endl;				
					insert("	mov rax, __float64__(1.0)");				
					insert("	push rax");				
					fptr<<"_eqend"<<token.position[0]<<token.position[1]<<":"<<std::endl;				
					break;
				}
				case NOT:
				{
					insert("	pop r8");
					insert("	mov rax, __float64__(0.0)");				
					insert("	cmp r8, rax");				
					fptr<<"	je _not"<<token.position[0]<<token.position[1]<<std::endl;				
					insert("	mov r9, __float64__(0.0)");				
					insert("	push r9");
					fptr<<"	jmp _notend"<<token.position[0]<<token.position[1]<<std::endl;				
					fptr<<"_not"<<token.position[0]<<token.position[1]<<":"<<std::endl;				
					insert("	mov r9, __float64__(1.0)");				
					insert("	push r9");				
					fptr<<"_notend"<<token.position[0]<<token.position[1]<<":"<<std::endl;				
					break;
				}	
				case AND:
				{
					insert("	pop r8");
					insert("	pop r9");								
					insert("	and r9, r8");				
					insert("	push r9");					
					insert("	xor r8, r8");	
					insert("	xor r9, r9");				
					break;
				}
				case OR:
				{
					insert("	pop r8");
					insert("	pop r9");				
					insert("	or r9, r8");
					insert("	push r9");
					insert("	xor r8, r8");				
					insert("	xor r9, r9");						
					break;
				}
				default:
					assert(false && "Undetected invalid token found!");
					exit(1);
					break;
				}
			}
		}	

	//syscall for termination of program		
	insert("	mov rax, 0x3c");
	insert("	mov rdi, 1");
	insert("	syscall");
	insert("	");
	insert("_printRAXfloat:");
	insert("	movq xmm0, rax");			
	insert("	push rax");			
	insert("	roundsd xmm0, xmm0, 0x3");			
	insert("	xor rax, rax");			
	insert("	cvtsd2si rax, xmm0");			
	insert("	call _printRAXint");			
	insert("	mov rax, 1");			
	insert("	mov rdi, 1");			
	insert("	mov rsi, dot");			
	insert("	mov rdx, 1");
	insert("	syscall");
	insert("	mov rbx, 1");				
	insert("_printDEC:");				
	insert("	inc rbx");				
	insert("	pop rax");				
	insert("	movq xmm0, rax");				
	insert("	roundsd xmm1, xmm0, 0x3");				
	insert("	subsd xmm0, xmm1");				
	insert("	mov r8, __float64__(10.0)");				
	insert("	movq xmm1, r8");				
	insert("	mulsd xmm0, xmm1");				
	insert("	movq rax, xmm0");				
	insert("	push rax");				
	insert("	roundsd xmm0, xmm0, 0x3");				
	insert("	cvtsd2si rax, xmm0");				
	insert("	add rax, 48");				
	insert("	mov [decCHARbuff], rax");				
	insert("	mov rax, 1");				
	insert("	mov rdi, 1");				
	insert("	mov rsi, decCHARbuff");				
	insert("	mov rdx, 1");				
	insert("	syscall");				
	insert("	cmp rbx, 6");				
	insert("	jle _printDEC");				
	insert("	pop rax");				
	insert("	xor rax, rax");				
	insert("	ret");				
	insert("_printRAXint:");			
	insert("	mov rcx, intBUFF");			
	insert("	mov [intBUFFptr], rcx");			
	insert("_printRAXintLoop1:");			
	insert("	mov rdx, 0");
	insert("	mov rbx, 10");			
	insert("	div rbx");			
	insert("	add rdx, 48");			
	insert("	push rax");			
	insert("	mov rcx, [intBUFFptr]");			
	insert("	mov [rcx], dl");			
	insert("	inc rcx");			
	insert("	mov [intBUFFptr], rcx");			
	insert("	pop rax");			
	insert("	cmp rax, 0");			
	insert("	jne _printRAXintLoop1");			
	insert("_printRAXintLoop2:");			
	insert("	mov rcx, [intBUFFptr]");			
	insert("	mov rax, 1");			
	insert("	mov rdi, 1");			
	insert("	mov rsi, rcx");			
	insert("	mov rdx, 1");			
	insert("	syscall");			
	insert("	mov rcx, [intBUFFptr]");			
	insert("	dec rcx");			
	insert("	mov [intBUFFptr], rcx");			
	insert("	cmp rcx, intBUFF");			
	insert("	jge _printRAXintLoop2");			
	insert("	ret");			
	fptr.close();
	

	//TODO: check validity of command and res codes
	std::string command = "nasm -f elf64 -o " + output_file + ".o " + output_file + ".asm";
	system(command.c_str());

	std::string out = "ld -o" + output_file + " " + output_file +".o";
	system(out.c_str());
	
	std::string clean = "rm " + output_file + ".o "+ output_file + ".asm" ;
	system(clean.c_str());

}

int main(int argc, char* argv[]){
	
	assert( argc >= 2 ); //TODO: add proper documentation

	std::string executable = argv[0];
	bool isCompiling = false;
	std::string output_file = "app";
	std::string filename = "";


	//TODO: add flags to build either ELF executable or to object files or just asm
	//TODO: add verbose flag
	for (int i = 1; i < argc; ++i) {
		std::string arg = argv[i];
		if (arg == "-c") {
			isCompiling = true;
		} else if (arg == "-o") {
			if (i + 1 < argc) {
				output_file = argv[++i];
			} else {
				printerr("Output file not specified after -o");
				exit(1);
			}
		} else {
			filename = arg;
			break; // Assume the filename is the last relevant argument
		}
	}

	if (filename.empty()) {
		printerr("Filename not provided or invalid");
		exit(1);
	}

	std::vector<Token> program; // program stores all tokens to evaluate for interpreting or compilation
	
	std::ifstream file(filename);
	if (!file.is_open()) { 
		printerr("Could not open file: " << filename); 
		return 1; 
	} 
		
	int row = 0; // stores line number 
	std::string line;
	while(getline(file, line)){
		std::vector<Token> tokens = parseLine(line,row);
		for(Token token : tokens){
			program.push_back(token);
		}
		row++;
	}

#if 0 // to visualize the program stack		
	println("_____________");
	for (Token t : program){
		if (t.type == KEYWORD)
			println(getOPCODE(t.value));
		else
			println(t.value);
	}
	println("_____________");
#endif

	crossrefIndice(program);

	if (!isCompiling){
		execute(program);
	}else{
		compile(program, output_file);
	}
}

