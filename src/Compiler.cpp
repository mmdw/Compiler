//============================================================================
// Name        : Compiler.cpp
// Author      : mmdw
// Version     :
// Copyright   : 
//============================================================================

#include <iostream>
#include <fstream>
#include <string>

#include "syntax/SyntaxExp.h"

using namespace std;
void testLexer() {
	ifstream ifs("C:/Users/user/git/Compiler/res/test.txt");

	Syntax::TreeNode* p_root;

	Tokenizer::Tokenizer tk(ifs);
	Syntax::SyntaxExp syntaxAnalyzer(tk, &p_root);

	syntaxAnalyzer.printTree(std::cout, p_root);

//	while (tk.hasNext()) {
//		cout << tk.tokenToString(tk.getToken()) << " ";
//		tk.goNext();
//	}

}
int main() {
	testLexer();
	return 0;
}
