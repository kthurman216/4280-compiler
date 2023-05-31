/*
 * statsem.cpp
 *
 *  Created on: Apr 23, 2023
 *      Author: Kayla
 */

#include <iostream>
#include <string>
#include <cstdlib>

#include "statsem.h"

using namespace std;


void semanticCheck(pTree parseTree, IDStack stack) {
	Token errToken; //token for error reporting

	parseTree.semanticTraversal(parseTree.root, errToken, stack);

	//if tree does not throw error during traversal, prints that semantics are ok and exits
	//cout << "Static semantics OK" << endl;
	//exit(0);	//why do i run into a memory leak if i don't exit here???
	//return;	//successful return instead of exit?
}

void semanticError(int errorType, Token errorTk) {
	if (errorType == 1) {
		cout << "SEMANTIC ERROR: Variable " << errorTk.name << " at line " << errorTk.line << " previously defined" << endl;
		exit(1);
	}
	else { //if not error type 1, error type 2
		cout << "SEMANTIC ERROR: Undefined identifier used, " << errorTk.name << " at line " << errorTk.line << endl;
		exit(2);
	}
}
