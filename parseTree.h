/*
 * parseTree.h
 *
 *  Created on: Apr 11, 2023
 *      Author: Kayla
 */
#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>

#include "token.h"
#include "idStack.h"

using namespace std;

#ifndef PARSETREE_H_
#define PARSETREE_H_

struct TreeNode;

class pTree {

public:
		//root pointer
		TreeNode *root;

		//PARSE TREE OPERATIONS
		TreeNode * newNode(string);
		void destroySubTree(TreeNode *);
		void setDepth(TreeNode *, int);
		void semanticTraversal(TreeNode *, Token &, IDStack &); //refactored to traverse tree and validate static semantics
		void buildTarget(TreeNode *, IDStack &, FILE*);
		void printNode(TreeNode *);
		//constructor
		pTree()
			{ root = NULL; }

		//destructor
		~pTree()
		{ destroySubTree(root); }


};

struct TreeNode {
		//label, taken from function that creates node
		string label;
		//store tokens (in the case of varList?)
		Token nTk1;
		Token nTk2;
		//variable to store node depth
		int depth;
		//pointer to node children (should not be more than 4 children)
		struct TreeNode *child1;
		struct TreeNode *child2;
		struct TreeNode *child3;
		struct TreeNode *child4;
	};

#endif /* PARSETREE_H_ */
