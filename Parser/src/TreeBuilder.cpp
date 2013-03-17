/*
 * TreeBuilder.cpp
 *
 *  Created on: 10.03.2013
 *      Author: user
 */
#include "Scanner.h"
#include "../headers/TreeBuilder.h"
#include "StringHolder.h"

namespace Compiler {

	void TreeBuilder::parseStream(ASTBuilder::TreeNode** pp_node, ASTBuilder::SymbolTable** pp_table,
			std::istream &iniStream) {

		ASTBuilder::Scanner scanner(&iniStream);

		*pp_table = new ASTBuilder::SymbolTable;

		ASTBuilder::SymbolResolver resolver(*pp_table);

		ASTBuilder::Parser parser(scanner, pp_node, &resolver);

		resolver.push();
		parser.parse();
		resolver.pop();

		ASTBuilder::StringHolder::clear();
	}
}
