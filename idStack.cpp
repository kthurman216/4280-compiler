/*
 * idStack.cpp
 *
 *  Created on: Apr 25, 2023
 *      Author: Kayla
 */

#include <iostream>
#include <string>
#include <cstdlib>

#include "idStack.h"

using namespace std;

//push function should only push ID tokens into the stack that are not ALREADY in the stack
bool IDStack :: push(Token currToken) {
	//check to see if stack is at max size or not
	if (size < 100) { //array index 99 would mean stack is at capacity
		//check if stack is empty first
		if (size < 0) {
			size++; //increment stack size
			tkStack[size] = currToken; //put token in the stack at the top (size)
			return true;	//token successfully put into stack
		}
		else { //stack has contents, verify that current ID is not already in the array
			//for (int i = 0; i <= size; i++) { //for loop to check that current id is NOT already in stack
			for (int i = size; i > size - varCount[blockCount]; i--) { //trying to make this work for local variables, if it doesn't just go back
				if (tkStack[i].name == currToken.name) { //if true, token is already defined
					//cout << "Already in stack" << endl;
					return false;	//failed to put token into stack
				}
			}	//end for loop; if token not found after cycling through stack, add it

			size++;	//increment stack size
			tkStack[size] = currToken;
			return true;
		}
	}
	else {
		//stack overflow handling
		cout << "STACK OVERFLOW" << endl;
		exit(1);
	}
}

//pop function should only be getting called at the end of a block to pop variables specific to that block from the stack
void IDStack :: pop() {
	//check if stack is empty; if so, throw error
	if (size < 0) {
		cout << "STACK EMPTY" << endl;
		exit(1);
	}
	else { //move reference to top of stack and clear name of token in case of redeclaration
		tkStack[size].name = "";
		size--;
	}
}

//verify checks that a variable is present in the stack (defined) before it can be used in its block
int IDStack :: verify(Token currToken) {
	for (int i = size; i >= 0; i--) {
	//for (int i = size; i > size - varCount[blockCount]; i--) { //global version works, this is for LOCAL, just go back if it doesn't work
		if (tkStack[i].name == currToken.name) { //token defined before use
			return i; //token successfully verified, returns distance from top of stack
		}
	}

	//if for loop exhausted without finding a match
	//cout << "Token not properly defined before use" << endl;
	return -1;	//token not found in stack
}

//this is SPECIFICALLY for the stack read function; other verify function is reading stack in reverse in assembler
int IDStack :: verifyRev(Token currToken) {
	for (int i = 0; i <= size; i++) {
	//for (int i = size; i > size - varCount[blockCount]; i--) { //global version works, this is for LOCAL, just go back if it doesn't work
		if (tkStack[i].name == currToken.name) { //token defined before use
			return i; //token successfully verified, returns distance from top of stack
		}
	}

	//if for loop exhausted without finding a match
	//cout << "Token not properly defined before use" << endl;
	return -1;	//token not found in stack
}
