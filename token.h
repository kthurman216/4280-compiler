/*
 * token.h
 *
 *  Created on: Apr 10, 2023
 *      Author: Kayla
 */

#include <iostream>
#include <string>

#ifndef TOKEN_H_
#define TOKEN_H_

using namespace std;


//tokenType enumerator
enum TokenType {
    //codes for error
    TokenError = -1, KeywordError = -2,

	//empty/null token
	EmptyTk = 0,

    //identifiers and ints
    IDTk = 1001, IntTk,

    //operators
    AddTk, SubTk, MultTk, DivTk, ModTk, EqTk, DotTk, LessTk, GreaterTk, TildeTk, LessTildeTk, GreaterTildeTk,

    //delimiters and separators; Braces are {}, Brackets are []
    RParenthTk, LParenthTk, RBraceTk, LBraceTk, RBracketTk, LBracketTk, CommaTk, ColonTk, SemicolonTk, EOFTk,

    //keyword tokens
    OpenTk = 2001, CloseTk, IterTk, VoidTk, DataTk, ExitTk, ReadTk, PrintTk, MainTk, IffTk, ThenTk, LetTk, FuncTk
};

//token struct
struct Token {
    TokenType type;
    string tName;
    string name;
    int line;
};

#endif /* TOKEN_H_ */
