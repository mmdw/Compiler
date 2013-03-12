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

void testParser() {
	Compiler::TreeBuilder iniFile;
	std::istringstream iss("int eeee() {a = b + c;}");

	try {
		Compiler::ASTBuilder::TreeNode::printTree(std::cout, iniFile.parseStream(iss), 0);
	} catch (std::string& e) {
		cout << "exception: " << e << std::endl;
	}
}
int main() {
	testParser();
	return 0;
}
