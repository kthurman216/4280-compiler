/*
 * parser.h
 *
 *  Created on: Apr 10, 2023
 *      Author: Kayla
 */

#include <iostream>
#include <fstream>
#include <cstdlib>

#include "token.h"
#include "scanner.h"
#include "parseTree.h"

using namespace std;

#ifndef PARSER_H_
#define PARSER_H_

TreeNode * parse();
TreeNode * program();
TreeNode * block();
TreeNode * vars();
TreeNode * varList();
TreeNode * exp();
TreeNode * M();
TreeNode * N();
TreeNode * R();
TreeNode * stats();
TreeNode * mStat();
TreeNode * stat();
TreeNode * in();
TreeNode * out();
TreeNode * iff();
TreeNode * loop();
TreeNode * assign();
TreeNode * RO();
void pError(string, int);

#endif /* PARSER_H_ */
