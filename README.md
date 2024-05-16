# Ganita(.gt)
A mathematical language crafted to solve complex maths
<divider/>

## References
[Reverse Polish Notation](https://en.wikipedia.org/wiki/Reverse_Polish_notation) <br>
	[Forth](https://en.wikipedia.org/wiki/Reverse_Polish_notation)

## Introduction	
Ganita means <i><b>Maths</b></i>. The language shall be used to solve maths problems.

Following example takes 2 input and pushes max of the two onto the stack: <br>
<b>THIS LANGUAGE IS STILL UNDER DEVELOPMENT. IT MAY OR MAY NOT LOOK LIKE THIS ONCE FINISHED</b><br>
`:MAX (int,int) -> int > IF POP ELSE SWAP POP` <br>
`2 3 MAX WRITE` <br>

This prints 3 to the console.

`
10 WHILE DUP 0 > DO
	DUP 2 % 0 NOT = IF DUP PRINT END
	1 -
END
`

this prints odd numbers from 9 to 0

## Documentation
Language works on concept of stacks.
Every data is pushed and pop though data in a linear fashion.
There are various operation for data handling and manipulation.

All of implemented commands are given below:

`*/-+%` : Performs basic arithmetic by poping last 2 elements and pushing the result

`PRINT` : Pops the last element and prints it to the console

`DUP`   : Copies the last element onto the stack

`2DUP`  : Copies the last 2 elements onto the stack

`NOT`   : Flips the last element 

`=><`   : Performs basic condtional check by poping last 2 elements and pushing the result

`<condition> IF ..ELSE ..END` : conditional branching

`WHILE <condtion> DO ..END`   : conditional looping`
