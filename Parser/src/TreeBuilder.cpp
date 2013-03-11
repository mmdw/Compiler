/*
 * TreeBuilder.cpp
 *
 *  Created on: 10.03.2013
 *      Author: user
 */
#include "Scanner.h"
#include "../headers/TreeBuilder.h"

namespace Compiler {

	ASTBuilder::TreeNode* TreeBuilder::parseStream(std::istream &iniStream) {
		ASTBuilder::Scanner scanner(&iniStream);
		ASTBuilder::TreeNode* node;
		ASTBuilder::Parser parser(scanner, &node);
		parser.parse();

		return node;
	}
}
