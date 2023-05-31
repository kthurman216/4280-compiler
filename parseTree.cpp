/*
 * parseTree.cpp
 *
 *  Created on: Apr 11, 2023
 *      Author: Kayla
 */

#include <iostream>
#include <string>
#include <stdio.h>

#include "parseTree.h"
#include "codegen.h"
#include "statsem.h"

using namespace std;

TreeNode* pTree :: newNode(string label) {
	TreeNode* nextNode = NULL;

	//creates new node
	nextNode = new TreeNode;

	nextNode->label = label;

	//initialize 4 children nodes
	nextNode->child1 = nextNode->child2 = nextNode->child3 = nextNode->child4 = NULL;

	return nextNode;
}

void pTree :: destroySubTree(TreeNode * nodePtr) {
	//if current pointer is valid, checks next level for valid pointers to get to bottom of tree, destroys sub trees from bottom up
	if (nodePtr) {
		if (nodePtr->child1) {
			destroySubTree(nodePtr->child1);
		}
		if (nodePtr->child2) {
			destroySubTree(nodePtr->child2);
		}
		if (nodePtr->child3) {
			destroySubTree(nodePtr->child3);
		}
		if (nodePtr->child4) {
			destroySubTree(nodePtr->child4);
		}

		//delete nodePtr;
	}

}

void pTree :: setDepth(TreeNode * nodePtr, int depth) {
	if (nodePtr) {
		nodePtr->depth = depth;
		setDepth(nodePtr->child1, depth + 1);
		setDepth(nodePtr->child2, depth + 1);
		setDepth(nodePtr->child3, depth + 1);
		setDepth(nodePtr->child4, depth + 1);
	}
}

//function refactored to take in to checked against/modified the ID stack; returns if successful, else throws semantic error
void pTree :: semanticTraversal(TreeNode * nodePtr, Token &errToken, IDStack &stack) {
	//when encounters null node, returns
	if (nodePtr == NULL)
		return; //default return?

	//action to be taken at each node in parse tree; previously printNode was called here
	//printNode(nodePtr);	//print node for debugging static semantics

	if (nodePtr->label == "BLOCK") { //starting a new block, increment block count and set varCount to 0
		stack.blockCount++;	//increment blockCount
		stack.varCount[stack.blockCount] = 0;
	}

	//check if in VARLIST (declaring a variable)
	if (nodePtr->label == "VARLIST" && nodePtr->nTk1.type == IDTk) { //if first token at the node is an ID, push it onto the stack
		if (stack.push(nodePtr->nTk1) == false) { //if push returns false, token already in array, throw error
			errToken = nodePtr->nTk1;
			semanticError(1, errToken);
		}
		else									//if push returns true, token successfully put in stack, increment varCount
			stack.varCount[stack.blockCount]++;
	}
			//if not in VARLIST but token is an ID
	if (nodePtr->label != "VARLIST" && nodePtr->nTk1.type == IDTk) {
			if (stack.verify(nodePtr->nTk1) == -1) {	//if verify returns -1, token undefined before use, throw error
				errToken = nodePtr->nTk1;
				semanticError(2, errToken);
			}
	}

	//traverse children in order
	semanticTraversal(nodePtr->child1, errToken, stack);
	semanticTraversal(nodePtr->child2, errToken, stack);
	semanticTraversal(nodePtr->child3, errToken, stack);
	semanticTraversal(nodePtr->child4, errToken, stack);

	//before closing out of a block, pop the variables from the block off of the stack, then decrement the block count
	if (nodePtr->label == "BLOCK") {
		while (stack.varCount[stack.blockCount] > 0) {
			stack.pop(); //pop the variable off the stack
			stack.varCount[stack.blockCount]--; //decrement the varCount;
		}
		//decrement the blockCount after resetting varCount
		stack.blockCount--;
	}

	//default return
	return;
}

void pTree::buildTarget(TreeNode * nodePtr, IDStack &stack, FILE* outFile) {	//input type 1 for file, 2 for stdin
	string var;
	string goTo;
	string loopExit;

	//when encounters null node, returns
	if (nodePtr == NULL)
		return; //default return?

	//processing for BLOCK
	if (nodePtr->label == "BLOCK") {
		//START OF BLOCK
					stack.blockCount++;	//increment blockCount
					stack.varCount[stack.blockCount] = 0;

					//traverse children in order
					buildTarget(nodePtr->child1, stack, outFile);
					buildTarget(nodePtr->child2, stack, outFile);
					buildTarget(nodePtr->child3, stack, outFile);
					buildTarget(nodePtr->child4, stack, outFile);

					//END OF BLOCK
					while (stack.varCount[stack.blockCount] > 0) {
						stack.pop(); //pop the variable off the stack
						fprintf(outFile, "POP\n");	//print pop to target
						stack.varCount[stack.blockCount]--; //decrement the varCount;
					}
					//decrement the blockCount after resetting varCount
					stack.blockCount--;
	}
	//processing for EXP
	else if (nodePtr->label == "EXP") {
		if (nodePtr->nTk1.type == MultTk || nodePtr->nTk1.type == DivTk) { //fill this in later
			//process second child first
			buildTarget(nodePtr->child2, stack, outFile);
			var = newName('T');
			fprintf(outFile, "STORE %s\n", var.c_str());	//put value of second child in temp variable
			//process first child
			buildTarget(nodePtr->child1, stack, outFile);
			//perform correct operation on value in var
			if (nodePtr->nTk1.type == MultTk) {
				fprintf(outFile, "MULT %s\n", var.c_str());
			}
			else if (nodePtr->nTk1.type == DivTk) {
				fprintf(outFile, "DIV %s\n", var.c_str());
			}
		}
		else
			buildTarget(nodePtr->child1, stack, outFile);
	}
	//processing for M
	else if (nodePtr->label == "M") {
		if (nodePtr->nTk1.type == AddTk) {
			//process second child first
			buildTarget(nodePtr->child2, stack, outFile);
			var = newName('T');
			fprintf(outFile, "STORE %s\n", var.c_str());	//put value of second child in temp var
			//process first child
			buildTarget(nodePtr->child1, stack, outFile);
			fprintf(outFile, "ADD %s\n", var.c_str());		//add value in temp var to acc
		}
		else
			buildTarget(nodePtr->child1, stack, outFile);
	}
	//processing for N
	else if (nodePtr->label == "N") {
		if (nodePtr->nTk1.type == TildeTk) {
			//get the value from the child node first
			buildTarget(nodePtr->child1, stack, outFile);
			//multiply the value by -1 for negation?
			fprintf(outFile, "MULT -1\n");
		}
		else if (nodePtr->nTk1.type == SubTk) {
			//process second child first
			buildTarget(nodePtr->child2, stack, outFile);
			var = newName('T');
			fprintf(outFile, "STORE %s\n", var.c_str());	//value of second child in temp var
			//process first child
			buildTarget(nodePtr->child1, stack, outFile);
			fprintf(outFile, "SUB %s\n", var.c_str());		//subtract value in var from acc
		}
		else
			buildTarget(nodePtr->child1, stack, outFile);
	}
	//processing for R
	else if (nodePtr->label == "R") {
		if (nodePtr->child1 == NULL) {	//null: does not contain exp
			if (nodePtr->nTk1.type == IntTk) {
				fprintf(outFile, "LOAD %s\n", nodePtr->nTk1.name.c_str());
			}
			else if (nodePtr->nTk1.type == IDTk) {
				fprintf(outFile, "STACKR %d\n", (stack.verify(nodePtr->nTk1)));	//get the value stored in the stack
			}
		}
		else {
			buildTarget(nodePtr->child1, stack, outFile);
		}
	}
	else if (nodePtr->label == "OUT") {
		//process exp first
		buildTarget(nodePtr->child1, stack, outFile);
		//get a name for a temp variable
		var = newName('T');
		//store then write variable
		fprintf(outFile, "STORE %s\n", var.c_str());
		fprintf(outFile, "WRITE %s\n", var.c_str());
	}
	else if (nodePtr->label == "VARLIST") {
		//push variable to stack
		stack.push(nodePtr->nTk1);	//already validated in static semantics, so should work
		fprintf(outFile, "PUSH\n");	//print push to target
		stack.varCount[stack.blockCount]++;	//increment stack varCount
		//load int to be stored
		fprintf(outFile, "LOAD %s\n", nodePtr->nTk2.name.c_str());
		fprintf(outFile, "STACKW %d\n", 0);
		//process next child if there is one
		buildTarget(nodePtr->child1, stack, outFile);
	}
	else if (nodePtr->label=="IN") {
		//get a new temp variable name
		var = newName('T');
		//target should read input into temp var, load into acc, then modify the variable on the stack
		fprintf(outFile, "READ %s\n", var.c_str());
		fprintf(outFile, "LOAD %s\n", var.c_str());
		fprintf(outFile, "STACKW %d\n", stack.verifyRev(nodePtr->nTk1));
	}
	else if (nodePtr->label == "ASSIGN") {
		//process child first
		buildTarget(nodePtr->child1, stack, outFile);
		//find variable in stack and set it equal to first child
		fprintf(outFile, "STACKW %d\n", stack.verify(nodePtr->nTk1));
	}
	else if (nodePtr->label == "IF") {
		//process rightmost child in if statement first
		buildTarget(nodePtr->child3, stack, outFile);
		//new variable to store the value of child 3 in
		var = newName('T');
		fprintf(outFile, "STORE %s\n", var.c_str());
		//process first child in if statement next
		buildTarget(nodePtr->child1, stack, outFile);
		//subtract value of rightmost child from first child to get value for relational operator
		fprintf(outFile, "SUB %s\n", var.c_str());
		//label for go to statement
		goTo = newName('L');
		//process second child (relational operator)
		if (nodePtr->child2->nTk1.type == GreaterTk) {
					//child 1 > child 3 if child3 - child1 is more than 0 (hope my label stays :')); don't go to label
					fprintf(outFile, "BRZNEG %s\n", goTo.c_str());
		}
		else if (nodePtr->child2->nTk1.type == LessTk) {
					//child1 < child3 if child 3 - child1 is NEGATIVE; don't go to label
					fprintf(outFile, "BRZPOS %s\n", goTo.c_str());
		}
		else if (nodePtr->child2->nTk1.type == EqTk) {
					//child1 == child3 if result is zero, if result is positive or negative go to label
					fprintf(outFile, "BRPOS %s\n", goTo.c_str());
					fprintf(outFile, "BRNEG %s\n", goTo.c_str());
		}
		else if (nodePtr->child2->nTk1.type == LessTildeTk) {
					//child1 <= child3 if result if 0 or negative; go to label if result if positive only
					fprintf(outFile, "BRPOS %s\n", goTo.c_str());
		}
		else if (nodePtr->child2->nTk1.type == GreaterTildeTk) {
					fprintf(outFile, "BRNEG %s\n", goTo.c_str());
		}
		else if (nodePtr->child2->nTk1.type == TildeTk) {
					//if result is zero, child1 == child3, go to label
					fprintf(outFile, "BRZERO %s\n", goTo.c_str());
		}
		else {	//in case something weird happens, proceed as in case of tilde token (not equal)
					fprintf(outFile, "BRZERO %s\n", goTo.c_str());
		}
		//process fourth child (statements)
		buildTarget(nodePtr->child4, stack, outFile);
		fprintf(outFile, "%s: NOOP\n", goTo.c_str());
	}
//	else if (nodePtr->label == "RO") {	//this doesn't work... goTo out of scope probably
//
//	}
	else if (nodePtr->label == "LOOP") {
		//get goto first
		goTo = newName('L');
		fprintf(outFile, "%s: NOOP\n", goTo.c_str());	//if RO is met, loop back to top of block
		//var for storing child3
		var = newName('T');
		//process third child first
		buildTarget(nodePtr->child3, stack, outFile);
		//store third child
		fprintf(outFile, "STORE %s\n", var.c_str());
		//process child1
		buildTarget(nodePtr->child1, stack, outFile);
		fprintf(outFile, "SUB %s\n", var.c_str());
		//get name for goto to exit loop; has to be different other one goes to top of loop :/
		loopExit = newName('L');
		//process second child (relational operator)
		if (nodePtr->child2->nTk1.type == GreaterTk) {
					//child 1 > child 3 if child3 - child1 is more than 0 (hope my label stays :')); don't go to label
					fprintf(outFile, "BRZNEG %s\n", loopExit.c_str());
		}
		else if (nodePtr->child2->nTk1.type == LessTk) {
					//child1 < child3 if child 3 - child1 is NEGATIVE; don't go to label
					fprintf(outFile, "BRZPOS %s\n", loopExit.c_str());
		}
		else if (nodePtr->child2->nTk1.type == EqTk) {
					//child1 == child3 if result is zero, if result is positive or negative go to label
					fprintf(outFile, "BRPOS %s\n", loopExit.c_str());
					fprintf(outFile, "BRNEG %s\n", loopExit.c_str());
		}
		else if (nodePtr->child2->nTk1.type == LessTildeTk) {
					//child1 <= child3 if result if 0 or negative; go to label if result if positive only
					fprintf(outFile, "BRPOS %s\n", loopExit.c_str());
		}
		else if (nodePtr->child2->nTk1.type == GreaterTildeTk) {
					fprintf(outFile, "BRNEG %s\n", loopExit.c_str());
		}
		else if (nodePtr->child2->nTk1.type == TildeTk) {
					//if result is zero, child1 == child3, go to label
					fprintf(outFile, "BRZERO %s\n", loopExit.c_str());
		}
		else {	//in case something weird happens, proceed as in case of tilde token (not equal)
					fprintf(outFile, "BRZERO %s\n", loopExit.c_str());
		}
		//process child4 (operations inside the loop)
		buildTarget(nodePtr->child4, stack, outFile);
		//go back to top of loop
		fprintf(outFile, "BR %s\n", goTo.c_str());
		//exit loop
		fprintf(outFile, "%s: NOOP\n", loopExit.c_str());
	}
	//processing for other nodes, just preorder
	else {
		buildTarget(nodePtr->child1, stack, outFile);
		buildTarget(nodePtr->child2, stack, outFile);
		buildTarget(nodePtr->child3, stack, outFile);
		buildTarget(nodePtr->child4, stack, outFile);
	}
}

void pTree :: printNode(TreeNode * nodePtr) {
	printf("%*c%s", nodePtr->depth*2, ' ', nodePtr->label.c_str());

	//if node has token with name not empty string
	if (nodePtr->nTk1.name != "") {
		printf(": %s %s", nodePtr->nTk1.tName.c_str(), nodePtr->nTk1.name.c_str());
	}
	if (nodePtr->nTk2.name != "") {
		printf(": %s %s", nodePtr->nTk2.tName.c_str(), nodePtr->nTk2.name.c_str());
	}
	//next line
	cout << endl;
}
