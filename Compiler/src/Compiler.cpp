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
#include "intermediate/SymbolResolver.h"
#include "intermediate/IRBuilder.h"

using namespace std;

void testSymbolResolver() {
	Compiler::SymbolResolver sr;

	try {
		sr.push();

		sr.insert("foo", Compiler::SYMBOL_INT);
		sr.insert("bar", Compiler::SYMBOL_INT);

		sr.push();
		sr.insert("bar", Compiler::SYMBOL_INT);
		sr.pop();


		sr.debug(std::cout);
	} catch (std::string& e) {
		std::cerr << e;
	}
}

void testParser() {
	Compiler::TreeBuilder iniFile;
//	std::istringstream is("int eeee() {foo(x, y, z); {1; 2; 3;} }");
	std::ifstream is ("C:/Users/user/git/Compiler/Compiler/res/test.txt");

	try {
		Compiler::ASTBuilder::TreeNode* p_tree = iniFile.parseStream(is);
		Compiler::ASTBuilder::TreeNode::printTree(std::cout, p_tree , 0);


		std::cout << "-------------------------------------------\n";

		Compiler::IRBuilder irBuilder(p_tree);

		delete p_tree;
	} catch (std::string& e) {
		cout << "exception: " << e << std::endl;
	}
}
int main() {
	testParser();
//	testSymbolResolver();
	return 0;
}
