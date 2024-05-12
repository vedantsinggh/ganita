#include <iostream>
#include <fstream>
#include <cassert>

#define print(x) std::cout<<x
#define println(x) std::cout<<x<<std::endl
#define printer(x) std::cerr<<x<<std::endl

int count = 0;

std::string* lexLine(std::string line){
	std::string* tokens = new std::string[1024];	
	std::string token;
	line += " ";

	for (char t: line){
		if (t == 32 || t == '\t'){
			if (!token.size() > 0){
				continue;
			}
			tokens[count] = token;
			token = "";
			count++;
			continue;
		}
		token += t;
	}
	return tokens;
}

int main(int argc, char* argv[]){
	
	assert( argc >= 2 );
	char* filename = argv[1];
	
	std::ifstream file(filename);
	if (!file.is_open()) { 
		printer("Could not open file: " << filename); 
		return 1; 
	} 

	std::string line;
	while(getline(file, line)){
		std::string* tok = lexLine(line);
		for(int i = 0; i < count; i++){
			println(tok[i]);
		}
		count = 0;
	}
}
