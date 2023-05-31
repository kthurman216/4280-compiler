/*
 * main.cpp
 *
 *  Created on: Apr 10, 2023
 *      Author: Kayla
 */

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cctype>
#include <cstdio>

#include "token.h"
#include "scanner.h"
#include "parser.h"

using namespace std;

static void exitError(string s);  //helper function to display file error messaging
static void testFile(string, fstream &);  //helper function to test that file opens
static void handleInput(string, ofstream &);

int main(int argc, char const *argv[])
{
    fstream test;
    ofstream textOut;
    string file;
    string userInput;

    if (argc > 2)
            exitError("Do not enter more than 2 arguments.");
	if (argc == 1) { //we need some work here
	     //gets user input for processing
	     file = "ketvg5_temptext";
	     handleInput(file, textOut);
	     callParser(file, 2);

	     //remove temp file after processing
	     if (remove("ketvg5_temptext.sp23") != 0) {
		     exitError("Failed to delete file.");
	     }
	}
	else {
		//gets and validates input from file
		file = string(argv[1]);
		//test that file opens properly before parsing
		testFile(file, test);
		//cout << file << endl;
		//fstream fin(file.c_str());
		callParser(file, 1);
		fin.close();
	}

    return 0;
}

static void exitError(string s) {
	cout << s << endl;
	exit(1);
}

//tests that file opens successfully
static void testFile (string fName, fstream &myFile) {
	//string that holds file name entered by user
	string fileName = fName + ".sp23";

	//attempt to open file given by user as argument
	myFile.open(fileName.c_str());

	//if file fails to open, passes error message and exits
	if (!myFile) {
		exitError("File " + fileName + " invalid. Verify and enter file name without type extension and try again.");
	}
	else {
		//cout << "File successfully opened." << endl;
    		myFile.close();
	}
}

static void handleInput (string fName, ofstream &myFile) {
	//string to hold file name for handling stdin
	string fileName = fName + ".sp23";
	//string to hold the actual input
	string userInput;
	string fullInput;

	//open file
	myFile.open(fileName.c_str(), ios::out);

	if (!myFile) {
	     exitError("File did not open");
	}

	//get user input
	cout << "Please input tokens. Press enter key when done." << endl;
	while (getline(cin, userInput)) {
	     fullInput += userInput;

	     if (userInput.compare("EOF")) {
		     break;
	    }

	     userInput = "";
	}
	//puts user input into file to be handled by testScanner
	myFile << fullInput;

	myFile.close();
}



