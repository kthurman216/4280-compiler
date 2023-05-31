/*
 * parser.cpp
 *
 *  Created on: Apr 10, 2023
 *      Author: Kayla
 */

#include <iostream>
#include <string>
#include <cstdlib>

#include "token.h"
#include "scanner.h"
#include "parser.h"
#include "parseTree.h"

using namespace std;

Token tk;
pTree myTree;

TreeNode * parse() {
    myTree.root = program();
    return myTree.root;
}

TreeNode * program() {
	TreeNode * next = myTree.newNode("PROGRAM");
    tk = scanner();
    next->child1 = block();
    if (tk.type == EOFTk) {
        //cout << "Parse OK" << endl;	//debugging: verify that tree parses before implementing parse tree
        return next;
    }
    else pError("Expected EOF; received " + tk.tName, tk.line); //expected EOFtk
    return NULL;
}

TreeNode * block() { //open <vars> <stats> close
	TreeNode * next = myTree.newNode("BLOCK");
    if (tk.type == OpenTk) {
        tk = scanner();
        next->child1 = vars();
        next->child2 = stats();
        if (tk.type == CloseTk) {
            tk = scanner();
            return next;
        }
        else {
            pError("Expected CLOSE token; received " + tk.tName, tk.line);
            return NULL;
        }
    }
    else {
        pError("Expected OPEN token; received " + tk.tName, tk.line);
        return NULL;
    }
}

TreeNode * vars() { //data <varList> | empty
	TreeNode * next = myTree.newNode("VARS");
    if (tk.type == DataTk) {
        tk = scanner();
        next->child1 = varList();
        return next;
    }
    else {  //empty production
        return next;
    }
}

TreeNode * varList() { //IDtk : intTk ; | IDtk : intTk <varList>
	TreeNode * next = myTree.newNode("VARLIST");
    if (tk.type == IDTk) {
    	next->nTk1 = tk;
        tk = scanner();
        if (tk.type == ColonTk) {
            tk = scanner();
            if (tk.type == IntTk) {
            	next->nTk2 = tk;
                tk = scanner();
                if (tk.type == IDTk) //if tk is ID, calls varList again
                        next->child1 = varList();
                else if (tk.type == SemicolonTk) {
                    tk = scanner();
                    return next;
                }
                else {
                    pError("Expected ID or SEMICOLON token; received" + tk.tName, tk.line);
                    return NULL;
                }
            }
            else {
                pError("Expected INTEGER token; received " + tk.tName, tk.line);
                return NULL;
            }
        }
        else {
            pError("Expected COLON token; received " + tk.tName, tk.line);
            return NULL;
        }
    }
    else {
        pError("Expected ID token; received " + tk.tName, tk.line);
        return NULL;
    }

    return next;
}

TreeNode * exp() {
    TreeNode * next = myTree.newNode("EXP");
    next->child1 = M();
    if (tk.type == DivTk) {
    	next->nTk1 = tk;
        tk = scanner();
        next->child2 = exp();
    }
    else if (tk.type == MultTk) {
    	next->nTk2 = tk;
        tk = scanner();
        next->child2 = exp();
    }
    else
        return next; //empty production

    return next;
}

TreeNode * M() {
	TreeNode * next = myTree.newNode("M");
    next->child1 = N();
    if (tk.type == AddTk) {
    	next->nTk1 = tk;
        tk = scanner();
        next->child2 = M();
    }
    else
        return next; //empty production

    return next;
}

TreeNode * N() {
	TreeNode * next = myTree.newNode("N");
    if (tk.type == TildeTk) {
    	next->nTk1 = tk;
        tk = scanner();
        next->child1 = N();
    }
    else {
		next->child1 = R();
		if (tk.type == SubTk) {
			next->nTk1 = tk;
			tk = scanner();
			next->child2 = N();
		}
    }

    return next; //empty production
}

TreeNode * R() {
	TreeNode * next = myTree.newNode("R");
    if (tk.type == IDTk) {
    	next->nTk1 = tk;
        tk = scanner();
        return next;
    }
    if (tk.type == IntTk) {
    	next->nTk1 = tk;
        tk = scanner();
        return next;
    }
    if (tk.type == LParenthTk) {
        tk = scanner();
        next->child1 = exp();
        if (tk.type == RParenthTk) {
            tk = scanner();
            return next;
        }
        else {
            pError("Expected RIGHT PARENTHESIS token; received " + tk.tName, tk.line);
            return NULL;
        }
    }
    else {
        pError("Expected ID, INTEGER, or LEFT PARENTHESIS token; received " + tk.tName, tk.line);
        return NULL;
    }
}

TreeNode * stats() {
    TreeNode * next = myTree.newNode("STATS");
    next->child1 = stat();
    next->child2 = mStat();
    return next;
}

TreeNode * mStat() {
	TreeNode * next = myTree.newNode("MSTAT");
    if ((tk.type == ReadTk)
    || (tk.type == PrintTk)
    || (tk.type == OpenTk)
    || (tk.type == IffTk)
    || (tk.type == IterTk)
    || (tk.type == IDTk)) {
    	//will store token in block before next token is gotten
        next->child1 = stat();
        next->child2 = mStat();
        return next;
    }
    //else
        //return next;

    return next;
}

TreeNode * stat() {
	TreeNode * next = myTree.newNode("STAT");
    if (tk.type == ReadTk) {
        next->child1 = in();
        return next;
    }
    else if (tk.type == PrintTk) {
        next->child1 = out();
        return next;
    }
    else if (tk.type == OpenTk) {
        next->child1 = block();
        return next;
    }
    else if (tk.type == IffTk) {
        next->child1 = iff();
        return next;
    }
    else if (tk.type == IterTk) {
        next->child1 = loop();
        return next;
    }
    else if (tk.type == IDTk) {
    	//will store token in node before next token is gotten
        next->child1 = assign();
        return next;
    }
    else {
        pError("Expected READ, PRINT, OPEN, IFF, ITER, or ID token; received " + tk.tName, tk.line);
        return NULL;
    }
}

TreeNode * in() {
	TreeNode * next = myTree.newNode("IN");
    if (tk.type == ReadTk) {
        tk = scanner();
        if (tk.type == GreaterTk) {
            tk = scanner();
            if (tk.type == IDTk) {
            	next->nTk1 = tk;
                tk = scanner();
                if (tk.type == SemicolonTk) {
                    tk = scanner();
                    return next;
                }
                else pError("Expected SEMICOLON token; received " + tk.tName, tk.line);
            }
            else pError("Expected ID token; received " + tk.tName, tk.line);
        }
        else pError("Expected GREATER token; received " + tk.tName, tk.line);
    }

    return NULL;
}

TreeNode * out() {
	TreeNode * next = myTree.newNode("OUT");
    if (tk.type == PrintTk) {
        tk = scanner();
        if (tk.type == LessTk) {
            tk = scanner();
            next->child1 = exp();
            if (tk.type == SemicolonTk) {
                tk = scanner();
                return next;
            }
            else pError("Expected SEMICOLON token; received " + tk.tName, tk.line);
        }
        else pError("Expected LESS token; received " + tk.tName, tk.line);
    }

    return NULL;
}

TreeNode * iff() {
	TreeNode * next = myTree.newNode("IF");
    if (tk.type == IffTk) {
        tk = scanner();
        if (tk.type == LBracketTk) {
            tk = scanner();
            next->child1 = exp();
            next->child2 = RO();
            next->child3 = exp();
            if (tk.type == RBracketTk) {
                tk = scanner();
                next->child4 = stat();
                return next;
            }
            else pError("Expected RIGHT BRACKET token; received " + tk.tName, tk.line);
        }
        else pError("Expected LEFT BRACKET token; received " + tk.tName, tk.line);
    }

    return NULL;
}

TreeNode * loop() {
	TreeNode * next = myTree.newNode("LOOP");
    if (tk.type == IterTk) {
        tk = scanner();
        //cout << tk.tName << endl;
        if (tk.type == LBracketTk) {
            tk = scanner();
            next->child1 = exp();
            next->child2 = RO();
            next->child3 = exp();
            if (tk.type == RBracketTk) {
                tk = scanner();
                next->child4 = stat();
                return next;
            }
            else pError("Expected RIGHT BRACKET token; received " + tk.tName, tk.line);
        }
        else pError("Expected LEFT BRACKET token; received " + tk.tName, tk.line);
    }

    return NULL;
}

TreeNode * assign() {
	TreeNode * next = myTree.newNode("ASSIGN");
    if (tk.type == IDTk) {
    	next->nTk1 = tk;
        tk = scanner();
        if (tk.type == EqTk) {
            tk = scanner();
            next->child1 = exp();
            if (tk.type == SemicolonTk) {
                tk = scanner();
                return next;
            }
            else pError("Expected SEMICOLON token; received " + tk.tName, tk.line);
        }
        else pError("Expected EQUAL token; received " + tk.tName, tk.line);
    }

    return NULL;
}

TreeNode * RO() {
	TreeNode * next = myTree.newNode("RO");
    if ((tk.type == LessTildeTk)
     || (tk.type == GreaterTildeTk)
     || (tk.type == LessTk)
     || (tk.type == GreaterTk)
     || (tk.type == EqTk)
     || (tk.type == TildeTk)) {
    	next->nTk1 = tk;
        tk = scanner();
        return next;
    }
    else pError("Expected LESS TILDE, GREATER TILDE, LESS, GREATER, EQUAL, or TILDE token; received " + tk.tName, tk.line);

    return NULL;
}

void pError(string p, int line) {
    cout << p << ", line # " << line << endl;
    exit(1);
}
