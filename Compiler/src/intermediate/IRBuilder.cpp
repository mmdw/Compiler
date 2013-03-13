/*
 * IRBuilder.cpp
 *
 *  Created on: 13.03.2013
 *      Author: user
 */

#include <algorithm>
#include <iostream>
#include "IRBuilder.h"

namespace Compiler  {

IRBuilder::IRBuilder(ASTBuilder::TreeNode* p_tree) : p_tree(p_tree) {
	fillTables();

	sr.debug(std::cout);
	constTable.debug(std::cout);
}

IRBuilder::~IRBuilder() {
}

void IRBuilder::walk(ASTBuilder::TreeNode* p_node) {
	if (p_node->value == "BLOCK") {
		sr.push();
	}

	if (p_node->value == "DEFINE_VARIABLE") {
		const std::string& typeName   = p_node->at(0)->value;
		const std::string& identifier = p_node->at(1)->value;

		if (typeName == "int") {
			sr.insert(identifier, Compiler::SYMBOL_INT);
		} else if (typeName == "float") {
			sr.insert(identifier, Compiler::SYMBOL_FLOAT);
		} else {
			throw std::string("unknown type");
		}
	} else if (p_node->value == "CONST_INT") {
		constTable.put(p_node->at(0)->value, Compiler::CONST_INT);
	} else if (p_node->value == "CONST_FLOAT") {
		constTable.put(p_node->at(0)->value, Compiler::CONST_FLOAT);
	} else {
		for (std::list<Compiler::ASTBuilder::TreeNode*>::iterator it = p_node->childs.begin();
				it != p_node->childs.end(); ++it) {

			walk(*it);
		}
	}

	if (p_node->value == "BLOCK") {
		sr.pop();
	}

	std::cout << p_node->value << std::endl;
}

void IRBuilder::fillTables() {
	sr.push();
	walk(p_tree);
	sr.pop();
}

} /* namespace Syntax */
