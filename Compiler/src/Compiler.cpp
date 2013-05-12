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
#include <cstdlib>

#include "CodeGenerator.h"
#include "TreeNode.h"
#include "TreeBuilder.h"
#include "SymbolTable.h"
#include "arg.h"

using namespace std;

int main(int argc, char ** argv) {
	Compiler::TreeBuilder iFile;
	Compiler::ASTBuilder::SymbolTable*	p_table;
	Compiler::ASTBuilder::TypeTable*	p_type;
	Compiler::ASTBuilder::TreeNode* 	p_node;

	Arg arg;
	parse_args(argc, argv, &arg);

	std::ostringstream cmd;

	cmd << arg.compiler << ' '
		<< arg.asmOutFileName << ' '
		<< arg.out;

	try {
		std::ifstream is (arg.iFileName);
		std::ofstream outAsm(arg.asmOutFileName);
		std::ostringstream oss;

		iFile.parseStream(&p_node, &p_table, &p_type, is);

		Compiler::CodeGenerator cg(p_node, p_table, p_type, outAsm, oss);

		if (arg.reportOutFileName != 0) {
			std::ofstream outReport(arg.reportOutFileName);

			p_type->debug(outReport);
			outReport << '\n';
			p_table->debug(outReport, p_type);

			outReport << "\n-=TRIPLE=-\n\n";
			outReport << oss.str();
		}
		delete p_table;
		delete p_node;

		cout << cmd.str().c_str();
		system(cmd.str().c_str());

	} catch (std::string& e) {
//		p_type->debug(std::cout);
//		p_table->debug(std::cout, p_type);
		cerr << "exception: " << e << std::endl;

		delete p_table;
		delete p_node;
	}



	return 0;
}
