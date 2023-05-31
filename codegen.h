/*
 * codegen.h
 *
 *  Created on: Apr 30, 2023
 *      Author: Kayla
 */

#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>

#include "parseTree.h"

using namespace std;

#ifndef CODEGEN_H_
#define CODEGEN_H_

string newName(char);
void generate(pTree, IDStack&, FILE *);



#endif /* CODEGEN_H_ */
