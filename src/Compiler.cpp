//============================================================================
// Name        : Compiler.cpp
// Author      : mmdw
// Version     :
// Copyright   : 
//============================================================================

#include <iostream>
#include <fstream>
#include <string>

#include "tokenizer/Tokenizer.h"

using namespace std;
void testLexer() {
	ifstream ifs("C:/Users/user/git/Compiler/Compiler/res/test.txt");

	Tokenizer::Tokenizer tk(ifs);

	while (tk.hasNext()) {
		Tokenizer::Token token = tk.getToken();

		cout << tk.tokenToString(token) << endl;
	}
}
int main() {
	testLexer();
	return 0;
}
