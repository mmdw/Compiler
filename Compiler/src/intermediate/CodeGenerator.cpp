/*
 * CodeGenerator.cpp
 *
 *  Created on: 17.03.2013
 *      Author: user
 */
#include <iostream>
#include <sstream>

#include "SymbolTable.h"
#include "CodeGenerator.h"

namespace Compiler {

CodeGenerator::CodeGenerator(ASTBuilder::TreeNode* p_root,
		ASTBuilder::SymbolTable* p_table) : p_root(p_root), p_table(p_table), output(std::cout) {

	generateHeader();
	generateProcedures();
	generateConstSection();
}

void CodeGenerator::generateHeader() {
	output <<
		"format PE console\n"
		"entry start\n"
		"include 'win32a.inc'"
		"section '.code' code readable executable\n\n"
		"start: \n"
		"\tcall main\n"
		"\tccall [getchar]\n"
		"\tstdcall [ExitProcess], 0\n\n";
}

void CodeGenerator::generateConstSection() {
	output << "section '.data' readable \n";
	for (std::map<ASTBuilder::SymbolId, ASTBuilder::Symbol>::const_iterator it = p_table->begin(); it != p_table->end(); ++it) {
		if (it->second.allocationType == ASTBuilder::ALLOCATION_CONST_GLOBAL)
		output << symbolToAddr(it->first) << '\t' << "dd\t" <<
			it->second.value << std::endl;
	}
}

std::string CodeGenerator::symbolToAddr(ASTBuilder::SymbolId symbolId) {
	const ASTBuilder::Symbol& symbol = p_table->find(symbolId);
	std::ostringstream oss;
	oss << symbolId;
	const std::string strSymbol = oss.str();

	switch (symbol.allocationType) {
	case ASTBuilder::ALLOCATION_VARIABLE_LOCAL: 	return std::string("__local_") + strSymbol;
	case ASTBuilder::ALLOCATION_CONST_GLOBAL:		return std::string("__const_") + strSymbol;
	case ASTBuilder::ALLOCATION_VARIABLE_ARGUMENT:	return std::string("__arg_") + strSymbol;
	default:
		throw std::string("symbolToAddr");
	}
}

void CodeGenerator::generateProcedures() {
	for (std::list<ASTBuilder::TreeNode*>::iterator it = p_root->childs.begin(); it != p_root->childs.end(); ++it) {
		const ASTBuilder::SymbolId id = (*it)->symbolId;
		const ASTBuilder::Symbol& symbol = p_table->find(id);

		if (symbol.symbolType != ASTBuilder::SYMBOL_FUNC) {
			continue;
		}

		output << "proc " << symbol.value << ' ';
		if (!(*it)->childs.empty()) {
			for (std::list<ASTBuilder::TreeNode*>::iterator argsIter = (*it)->at(0)->childs.begin(); argsIter != (*it)->at(0)->childs.end(); ++argsIter) {
				if (argsIter != (*it)->at(0)->childs.begin()) {
					output << ", ";
				}
				output << symbolToAddr((*argsIter)->symbolId);
			}
			output << std::endl;
		}

//		output << id << '\t' << symbol.value << std::endl;
		output << "endp" << std::endl;
	}

}

CodeGenerator::~CodeGenerator() {
	// TODO Auto-generated destructor stub
}

} /* namespace Compiler */
