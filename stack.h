#include <iostream>

#ifndef STACK_H
#define STACK_H

enum OP_CODE {
	plus,
	minus,
	divide,
	mul
};

enum OP_TYPE {
	literal=0,
	command,
};

struct OP{
	OP_TYPE type;
	int value;	
};

class Stack{
public:
	OP ops[1024];
	int count;
	
	Stack();
	OP pop();
	void print();
	int push(OP op);
};

#endif // STACK_H


#ifdef STACK_IMPLEMENTATION

Stack::Stack(){
	count = 0;
}

int Stack::push(OP op){
	ops[count] = op;
	return count++;
}

void Stack::print(){
	for(int i=0; i < count; i++){
		std::cout << ops[i].type << " " << ops[i].value << std::endl;
	}
}

OP Stack::pop(){
	return ops[--count];
}
#endif // STACK_IMPLEMENTATION
