/*
 * codegen.cpp
 *
 *  Created on: Apr 30, 2023
 *      Author: Kayla
 */

#include <iostream>
#include <fstream>
#include <string>

#include "codegen.h"

using namespace std;

static int tempVars = 0;
static int labelVars = 0;

string newName(char type) {
	string name;
	if (type == 'T') {
		name += 'T';
		name += to_string(tempVars);
		tempVars++; 		//add tempvars to the end of name then increment count
	}
	else if (type == 'L') {
		name += 'L';
		name += to_string(labelVars);
		labelVars++; 		//add labelvars to the end of name then increment
	}

	return name;
}

void generate(pTree tree, IDStack &stack, FILE * outFile) {
	tree.buildTarget(tree.root, stack, outFile);	//build target file

	//print stop to file
	fprintf(outFile, "STOP\n");

	//print temp variables to file
	for (int i = 0; i < tempVars; i++) {
		fprintf(outFile, "T%d 0\n", i);
	}
}
