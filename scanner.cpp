/*
 * scanner.cpp
 *
 *  Created on: Apr 10, 2023
 *      Author: Kayla
 */

#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>
#include <fstream>

#include "token.h"
#include "scanner.h"
#include "parser.h"
#include "parseTree.h"
#include "idStack.h"
#include "statsem.h"
#include "codegen.h"

using namespace std;

fstream fin;

static char thisChar;
static char lookahead;
static int lineNum = 1;
static State currentState;
static int instLen;

static string typeNames[38] = {/*error*/ "LEXICAL ERROR", //index 0
/*id and int*/ "IDENTIFIER", "INTEGER", //index 1-2
/*operators*/ "PLUS", "MINUS", "MULTIPLICATION", "DIVISION", "MODULUS", "EQUAL", "DOT", "LESS THAN",
/*operators*/ "GREATER THAN", "TILDE", "LESS TILDE", "GREATER TILDE", //index 3-14
/*delims*/ "RIGHT PARENTHESIS", "LEFT PARENTHESIS", "RIGHT BRACE", "LEFT BRACE", "RIGHT BRACKET", "LEFT BRACKET",
/*delims*/ "COMMA", "COLON", "SEMICOLON", "EOF", //index 15-24
/*keywords*/ "OPEN", "CLOSE", "ITER", "VOID", "DATA", "EXIT", "READ", "PRINT", "MAIN", "IFF", "THEN", "LET", "FUNC"}; //index 25-37

//is working and keeps line count
void getNonWS() {
    while (isspace(thisChar)) {
        //check if current char is new line and increments line count
        if (thisChar == '\n')
            lineNum++;
        //gets next character since current character is WS
        thisChar = fin.get();
        lookahead = fin.peek();
    }
}

//is working
void skipComments() {
    while (currentState == COMM) {
        //gets next char
        thisChar = fin.get();
        lookahead = fin.peek();

        //increment line count
        if (thisChar == '\n')
            lineNum++;

        //if next char is @, no longer in commenting mode, otherwise continue skipping comments
        if (thisChar == '@') {
            //reset state to break loop
            currentState = READY;

            //skip ahead to next character
            thisChar = fin.get();
            //set lookahead again
            lookahead = fin.peek();
        }
    }
}

Token scanner() {
    Token nextToken;
    currentState = READY;
    instLen = 0;

    //get first character of next token and lookahead
    thisChar = fin.get();
    lookahead = fin.peek();

    //while true loop allows for continuing until non ws or comment found
    while (true) {
        //comment handling
        //if current character is @ and not already commenting, set state to commenting
        if (currentState != COMM && thisChar == '@') {
            currentState = COMM;
        }
        skipComments();

        getNonWS();

        //if character is a letter or underscore, handling IDENTIFIER
        if (isalpha(thisChar) || thisChar == '_') {
            //add character to token's name
            nextToken.name = thisChar;
            instLen++;
            // thisChar = fin.get();
            // lookahead = fin.peek();
            //change state to identifier
            currentState = IDENT;
        }
        //if character is number, handling INTEGER
        else if (isdigit(thisChar)) {
            nextToken.name = thisChar;
            instLen++;
            // thisChar = fin.get();
            // lookahead = fin.peek();
            //change state to integer
            currentState = INT;
        }
        //if character is some form of punctuation, call lookup function
        else if (ispunct(thisChar) && thisChar != '@') {
            currentState = LKUP;
            nextToken.name = thisChar;
            nextToken = lookup(nextToken.name);
            return nextToken;
        }
        //if character is EOF token
        else if (thisChar == EOF) {
            nextToken.type = EOFTk;
            nextToken.name = "EOF";
            nextToken.tName = typeNames[nextToken.type - 1000];
            nextToken.line = lineNum;
            currentState = END;
            return nextToken;
        }

        //state IDENTIFIER handling
        while (currentState == IDENT) {
            while (instLen < 8 && isalnum(lookahead)) {
                thisChar = fin.get();
                lookahead = fin.peek();
                nextToken.name += thisChar;
                instLen++;
            }
            //no longer in IDENT state
            currentState = END;
            //sets other token data
            nextToken.line = lineNum;
            nextToken.type = IDTk;
            nextToken.tName = typeNames[nextToken.type - 1000];
            //if current identifier doesn't start with _, checks for keyword match
            if (nextToken.name[0] != '_') {
                nextToken = keyLookup(nextToken.name);
            }
            return nextToken;
        }

        //state INT handling
        while (currentState == INT) {
            while (instLen < 8 && isdigit(lookahead)) {
                thisChar = fin.get();
                lookahead = fin.peek();
                nextToken.name += thisChar;
                instLen++;
            }

            currentState = END;
            nextToken.line = lineNum;
            nextToken.type = IntTk;
            nextToken.tName = typeNames[nextToken.type - 1000];
            return nextToken;
        }
    }

    return nextToken;
}

Token lookup(string sp) {
    Token spToken;
    int matchFound = -1;

    string spChars[21] = {"+", "-", "*", "/", "%", "=", ".", "<", ">", "~", "<~", ">~", ")", "(", "}", "{", "]", "[", ",", ":", ";"};

    if (thisChar == '<' && lookahead == '~') {
        //move to next character
        thisChar = fin.get();
        lookahead = fin.peek();

        currentState = END;
        spToken.type = LessTildeTk;
        spToken.name = spChars[10];
        spToken.line = lineNum;
        spToken.tName = typeNames[spToken.type - 1000];
        return spToken;
    }

    else if (thisChar == '>' && lookahead == '~') {
        //move to next char
        thisChar = fin.get();
        lookahead = fin.peek();
        currentState = END;

        spToken.type = GreaterTildeTk;
        spToken.name = spChars[11];
        spToken.line = lineNum;
        spToken.tName = typeNames[spToken.type - 1000];
        return spToken;
    }
    else {
        for (int i = 0; i < 21; i++) {
            if (sp.compare(spChars[i]) == 0) {
                spToken.type = static_cast<TokenType>(1003 + i);
                spToken.name = sp;
                spToken.line = lineNum;
                spToken.tName = typeNames[spToken.type - 1000];
                matchFound = 0;

                currentState = END;
                return spToken;
            }
        }
    }

    if (matchFound < 0) {
        spToken.type = TokenError;
        spToken.name = sp;
        spToken.line = lineNum;
        spToken.tName = typeNames[0];
        currentState = END;
        lexError(spToken);
    }

    return spToken;
}

Token keyLookup(string idWord) {
    Token keyToken;
    int matchFound = -1;

    string keywords[13] = {"open", "close", "iter", "void", "data", "exit", "read", "print", "main", "iff", "then", "let", "func"};

    for (int i=0; i < 13; i++) {
        if (idWord.compare(keywords[i]) == 0) {
            keyToken.type = static_cast<TokenType>(2001 + i);
            keyToken.name = idWord;
            keyToken.line = lineNum;
            keyToken.tName = typeNames[keyToken.type - 1976];
            matchFound = 0;
            return keyToken;
        }
    }
    if (matchFound < 0) {

        keyToken.type = KeywordError;
        keyToken.name = idWord;
        keyToken.line = lineNum;
        keyToken.tName = typeNames[0];
	lexError(keyToken);
    }

    return keyToken;
}

void printToken(Token thisToken) {
    //print valid ids, ints
    if (thisToken.type == 1001 || thisToken.type == 1002) {
        cout << thisToken.tName << ": " << thisToken.name << ", LINE # " << thisToken.line << endl;
    }
    //print keywords (don't need instance listed)
    else if (thisToken.type > 2000) {
        cout << "KEYWORD: " << thisToken.tName << ", LINE # " << thisToken.line << endl;
    }
    //print operators
    else if (thisToken.type > 1002 && thisToken.type < 1015) {
        cout << "OPERATOR: " << thisToken.tName << ", LINE # " << thisToken.line << endl;
    }
    //print delimiters
    else if (thisToken.type >= 1015 && thisToken.type < 2000) {
        cout << "DELIMITER: " << thisToken.tName << ", LINE # " << thisToken.line << endl;
    }
}

void lexError(Token errToken) {
	//unrecognized alphabet error
	if (errToken.type == TokenError) {
		cout << errToken.tName << ": UNRECOGNIZED CHARACTER " << errToken.name << " AT LINE # " << errToken.line << endl;
		fin.close();
		remove("ketvg5_temptext.sp23");
		exit(TokenError);
	}

	//unrecognized keyword/identifier
	if (errToken.type == KeywordError) {
		cout << errToken.tName << ": INVALID KEYWORD/IDENTIFIER " << errToken.name << " AT LINE # " << errToken.line << endl;
		fin.close();
		remove("ketvg5_temptext.sp23");
		exit(KeywordError);
	}
}

void callParser(string fileName, int inputType) {	//input type 1 for file, 2 for stdin
	pTree newTree;
    string parseFile = fileName + ".sp23";
    string outName;
    FILE* fout;

    fin.open(parseFile.c_str());

    //Token currentToken;
    //get first token
    //currentToken = scanner();
    //while (currentToken.type != EOFTk) {
    //    printToken(currentToken);
    //    currentToken = scanner();
    //}
    newTree.root = parse();
    newTree.setDepth(newTree.root, 0);
    //actions to be taken on parse tree will happen here
    //id stack for parse tree
    IDStack identifiers;
    semanticCheck(newTree, identifiers);

    if (inputType == 1)
    	outName = fileName + ".asm";
    else
    	outName = "a.asm";

    //create target file
    fout = fopen(outName.c_str(), "w");

    generate(newTree, identifiers, fout);

    fin.close();
    fclose(fout);

    cout << outName << endl;
}
