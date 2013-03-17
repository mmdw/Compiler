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

#include "intermediate/CodeGenerator.h"
#include "TreeNode.h"
#include "TreeBuilder.h"
#include "SymbolTable.h"

using namespace std;

void testParser() {
	Compiler::TreeBuilder iniFile;
	std::ifstream is ("C:/Users/user/git/Compiler/Compiler/res/test.txt");

	try {
		Compiler::ASTBuilder::TreeNode* 	p_node;
		Compiler::ASTBuilder::SymbolTable*	p_table;

		iniFile.parseStream(&p_node, &p_table, is);

		Compiler::ASTBuilder::TreeNode::printTree(std::cout, p_node, 0);
		std::cout << "\n-------------------------------------------\n";
		p_table->debug(std::cout);

		std::cout << "\n-------------------------------------------\n";
		Compiler::CodeGenerator cg(p_node, p_table);

		delete p_table;
		delete p_node;

	} catch (std::string& e) {
		cout << "exception: " << e << std::endl;
	}
}
int main() {
	testParser();
	return 0;
}
