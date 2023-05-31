/*
 * scanner.h
 *
 *  Created on: Apr 10, 2023
 *      Author: Kayla
 */

#include <iostream>
#include <string>
#include <fstream>

#include "token.h"

using namespace std;

#ifndef SCANNER_H_
#define SCANNER_H_

extern fstream fin;

enum State {
    ERROR = -1,
    READY = 0, COMM, IDENT, INT, LKUP, END
};

void getNonWS();
void callParser(string, int); //int is text input type for target name purposes
Token scanner();
Token lookup(string);
Token keyLookup(string);
void skipComments();
void printToken(Token);
void lexError(Token);

#endif /* SCANNER_H_ */
