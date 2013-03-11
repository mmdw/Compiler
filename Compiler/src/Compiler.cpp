//============================================================================
// Name        : Compiler.cpp
// Author      : mmdw
// Version     :
// Copyright   : 
//============================================================================

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

//#include "syntax/SyntaxExp.h"
#include "TreeNode.h"
#include "TreeBuilder.h"

using namespace std;
//void testLexer() {
//	ifstream ifs("C:/Users/user/git/Compiler/res/test.txt");
//
//	Syntax::TreeNode* p_root;
//
//	Tokenizer::Tokenizer tk(ifs);
//	Syntax::SyntaxExp syntaxAnalyzer(tk, &p_root);
//
//	syntaxAnalyzer.printTree(std::cout, p_root);
//
////	while (tk.hasNext()) {
////		cout << tk.tokenToString(tk.getToken()) << " ";
////		tk.goNext();
////	}
//
//}

void testParser() {
	Compiler::TreeBuilder iniFile;
	std::istringstream iss("1*(2+3)");

	Compiler::ASTBuilder::TreeNode::printTree(std::cout, iniFile.parseStream(iss), 0);

}
int main() {
	testParser();
	return 0;
}
