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

#include "CodeGenerator.h"
#include "TreeNode.h"
#include "TreeBuilder.h"
#include "SymbolTable.h"

using namespace std;
void testParser() {
	Compiler::TreeBuilder iniFile;
	std::ifstream is ("C:/Users/user/git/Compiler/Compiler/src/res/test.txt");

	try {
		Compiler::ASTBuilder::TreeNode* 	p_node;
		Compiler::ASTBuilder::SymbolTable*	p_table;
		Compiler::ASTBuilder::TypeTable*	p_type;

		iniFile.parseStream(&p_node, &p_table, &p_type, is);

		Compiler::ASTBuilder::TreeNode::printTree(std::cout, p_node, 0);
		std::cout << "\n-------------------------------------------\n";
		p_table->debug(std::cout, p_type);
		p_type->debug();

		std::cout << "\n-------------------------------------------\n";
		Compiler::CodeGenerator cg(p_node, p_table, p_type);

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
