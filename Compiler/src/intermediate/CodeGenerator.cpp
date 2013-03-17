/*
 * CodeGenerator.cpp
 *
 *  Created on: 17.03.2013
 *      Author: user
 */
#include <iostream>
#include <sstream>
#include <cassert>

#include "SymbolTable.h"
#include "CodeGenerator.h"
#include "TripleTranslator.h"

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
		ASTBuilder::SymbolType returnType = p_table->funcReturnType(id);

		if (symbol.symbolType != ASTBuilder::SYMBOL_FUNC) {
			continue;
		}

		output << "proc " << symbol.value << ' ';
		if (!(*it)->childs.empty()) {
			for (std::list<ASTBuilder::TreeNode*>::iterator argsIter = (*it)->at(0)->childs.begin();
					argsIter != (*it)->at(0)->childs.end(); ++argsIter) {

				if (argsIter != (*it)->at(0)->childs.begin()) {
					output << ", ";
				}
				output << symbolToAddr((*argsIter)->symbolId);
			}
			output << std::endl;
		}

		TripleSequence tripleSequence;
		generateTripleSequence(returnType, (*it)->at(1), tripleSequence);

		output << "endp" << std::endl;
	}

}

void CodeGenerator::generateTripleSequence(ASTBuilder::SymbolType returnType, ASTBuilder::TreeNode* p_node,
		std::list<Triple>& tripleSequence) {

	switch (p_node->nodeType) {
	case ASTBuilder::NODE_STATEMENT_BLOCK:
	case ASTBuilder::NODE_STATEMENT_SEQUENCE:
		break;
	case ASTBuilder::NODE_RETURN:
		tripleSequence.push_back(Triple(returnType == ASTBuilder::SYMBOL_VOID ? TRIPLE_CALL_PROCEDURE : TRIPLE_CALL_FUNCTION));
		return;
	case ASTBuilder::NODE_ASSIGN: {
			ASTBuilder::TreeNode* p_right = p_node->at(1);
			generateTripleSequence(returnType, p_right, tripleSequence);

			Addr rightAddr = tripleSequence.back().result;
			Addr leftAddr = symbolToAddr(p_node->at(0)->symbolId);

			return;
		}
	case ASTBuilder::NODE_PLUS: {
			ASTBuilder::TreeNode* p_right = p_node->at(1);
			ASTBuilder::TreeNode* p_left = p_node->at(0);

			Addr rightAddr;
			if (p_right->nodeType != ASTBuilder::NODE_SYMBOL) {
				generateTripleSequence(returnType, p_right, tripleSequence);
				rightAddr = tripleSequence.back().result;
			} else {
				rightAddr = symbolToAddr(p_right->symbolId);
			}

			Addr leftAddr;
			if (p_left->nodeType != ASTBuilder::NODE_SYMBOL) {
				generateTripleSequence(returnType, p_left, tripleSequence);
				leftAddr = tripleSequence.back().result;
			} else {
				leftAddr = symbolToAddr(p_left->symbolId);
			}

			Addr result = symbolToAddr(p_table->insertTemp(ASTBuilder::SYMBOL_INT));

			tripleSequence.push_back(Triple(TRIPLE_ADD, result, leftAddr, rightAddr));
		return;
	}

	case ASTBuilder::NODE_PRINTLN: {
		ASTBuilder::TreeNode* p_arg = p_node->at(0);
		Addr argAddr;
		if (p_arg->nodeType != ASTBuilder::NODE_SYMBOL) {
			generateTripleSequence(returnType, p_arg, tripleSequence);
			argAddr = tripleSequence.back().result;
		} else {
			argAddr = symbolToAddr(p_arg->symbolId);
		}
		tripleSequence.push_back(Triple(TRIPLE_PRINTLN, argAddr));
		return;
	}

	default:
		throw ASTBuilder::nodeTypeToString(p_node->nodeType);
	}

	for (std::list<ASTBuilder::TreeNode*>::iterator it = p_node->childs.begin(); it != p_node->childs.end(); ++it) {
		generateTripleSequence(returnType, *it, tripleSequence);
	}

}

CodeGenerator::~CodeGenerator() {
	// TODO Auto-generated destructor stub
}

} /* namespace Compiler */
