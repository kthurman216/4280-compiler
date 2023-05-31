/*
 * statsem.h
 *
 *  Created on: Apr 23, 2023
 *      Author: Kayla
 */

#include <iostream>
#include <string>

#include "parseTree.h"
#include "idStack.h"

#ifndef STATSEM_H_
#define STATSEM_H_

//global function
void semanticCheck(pTree, IDStack);
void semanticError(int, Token); //takes int for error type and token that threw error


#endif /* STATSEM_H_ */
