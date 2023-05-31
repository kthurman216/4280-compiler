/*
 * idStack.h
 *
 *  Created on: Apr 25, 2023
 *      Author: Kayla
 */

#include <iostream>
#include <string>

#include "token.h"

using namespace std;

#ifndef IDSTACK_H_
#define IDSTACK_H_

/*		PSEUDOCODE
 * 	Stack will be a class; main structure of the stack will be an array container
 *  Stack has a max size of 100; if user tries to push more than 100 items onto stack, stack overflow error
 *  Stack properties are:
 *  Array of ID tokens (default to 100 tks)
 *  int size to store size of stack
 *  BLOCKCOUNT INT AND VARCOUNT ARRAY
 *  blockCount tracks the block currently in; increments at the START of a BLOCK (labeled in tree node)
 *  inside a VARLIST block, when an ID is initialized varCount[blockCount] is incremented
 *  at the end of the block, check the label; if it is BLOCK, pop stack until varCount[blockCount] is 0, decrement blockCount, continue
 *  Push, pop, and verify methods?
 *  PUSH: int size starts at -1; when first item is pushed onto stack, size++; insert token at stack[size] (index 0; first index of array)
 *  POP: if size is < 0, stack is empty, error. Else, "remove" (ignore?) the item at stack[size], size--
 *  VERIFY: if size < 0, stack is empty, error. Else, for each loop from stack[0] to stack[size], check current ID against IDs in stack
 */

//MAX STACK SIZE IS 100

class IDStack {
public:
	Token tkStack[100]; //array container for stack structure
	int size;
	int blockCount; //keep count of the number of blocks
	int varCount[100]; //keeps track of the variables in each block up to 100 blocks

	//constructor
	IDStack()
		{ size = blockCount = -1; } //initialize first array index and block count to -1, signifying stack is empty
	//stack methods
	bool push(Token);
	void pop();
	int verify(Token);
	int verifyRev(Token);
};




#endif /* IDSTACK_H_ */
