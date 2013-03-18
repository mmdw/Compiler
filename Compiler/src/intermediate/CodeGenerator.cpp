/*
 * CodeGenerator.cpp
 *
 *  Created on: 17.03.2013
 *      Author: user
 */
#include <iostream>
#include <cassert>
#include <set>

#include "../util/util.h"
#include "SymbolTable.h"
#include "CodeGenerator.h"
#include "TripleTranslator.h"

namespace Compiler {

CodeGenerator::CodeGenerator(ASTBuilder::TreeNode* p_root,
		ASTBuilder::SymbolTable* p_table) : p_root(p_root), p_table(p_table), output(std::cout) {

	validate();

	generateHeader();
	generateProcedures();
	generateConstSection();
	generateFooter();
}

void CodeGenerator::generateHeader() {
	output <<
		"format PE console\n"
		"entry start\n"
		"include 'win32a.inc'\n"
		"section '.code' code readable executable\n\n"
		"start: \n"
		"\tcall main\n"
		"\tccall [getchar]\n"
		"\tstdcall [ExitProcess], 0\n\n";
}

void CodeGenerator::generateConstSection() {
	output << "section '.data' readable \n";
	for (std::map<ASTBuilder::SymbolId, ASTBuilder::Symbol>::const_iterator it = p_table->begin(); it != p_table->end(); ++it) {
		if (it->second.allocationType == ASTBuilder::ALLOCATION_CONST_GLOBAL) {
			output << symbolToAddr(p_table, it->first) << '\t' << "dd\t" <<
					it->second.value << std::endl;
		}
	}

	output << "__format_int\tdb\t\"%d\",0" << std::endl;
	output << "__format_float\tdb\t\"%f\",0" << std::endl;

	output << std::endl;
}

std::string CodeGenerator::symbolToAddr(ASTBuilder::SymbolTable* p_table, ASTBuilder::SymbolId symbolId) {
	const ASTBuilder::Symbol& symbol = p_table->find(symbolId);

	Util::to_string(symbolId);

	const std::string strSymbol = Util::to_string(symbolId);

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
				output << symbolToAddr(p_table, (*argsIter)->symbolId);
			}
			output << std::endl;
		}

		TripleSequence tripleSequence;
		generateTripleSequence(returnType, (*it)->at(1), tripleSequence);
		if (returnType == SYMBOL_VOID) {
			if (tripleSequence.empty() || tripleSequence.back().op != TRIPLE_RETURN_PROCEDURE) {
				tripleSequence.push_back(Triple(TRIPLE_RETURN_PROCEDURE));
			}
		} else {
			if (tripleSequence.empty() || tripleSequence.back().op != TRIPLE_RETURN_FUNCTION) {
				throw std::string("function ") + symbol.value + " must return value";
			}
		}
		generateLocalVariables(tripleSequence);
		tripleTranslator.translate(p_table, output, tripleSequence);
//		printTripleSequence(output, tripleSequence);

		output << "endp" << std::endl << std::endl;
	}
}

static bool isVariableLocal(SymbolTable* p_table, SymbolId id) {
	return id != SYMBOL_UNDEFINED && p_table->find(id).allocationType == ALLOCATION_VARIABLE_LOCAL;
}

void CodeGenerator::generateLocalVariables(TripleSequence& tripleSequence) {
	std::set<SymbolId> locals;

	for (TripleSequence::iterator it = tripleSequence.begin(); it != tripleSequence.end(); ++it) {
		if (isVariableLocal(p_table, it->arg1)) {
			locals.insert(it->arg1);
		}

		if (isVariableLocal(p_table, it->arg2)) {
			locals.insert(it->arg2);
		}

		if (isVariableLocal(p_table, it->result)) {
			locals.insert(it->result);
		}
	}

	for (std::set<SymbolId>::iterator it = locals.begin(); it != locals.end(); ++it) {
		std::string size(p_table->find(*it).symbolType == SYMBOL_DOUBLE_FLOAT ? ":QWORD" : ":DWORD");

		output << '\t' << "local\t" << symbolToAddr(p_table, *it) << size << std::endl;
	}
}

void CodeGenerator::generateTripleSequence(ASTBuilder::SymbolType returnType, ASTBuilder::TreeNode* p_node,
		std::list<Triple>& tripleSequence) {

	switch (p_node->nodeType) {
	case ASTBuilder::NODE_STATEMENT_BLOCK:
	case ASTBuilder::NODE_STATEMENT_SEQUENCE:
		break;
	case ASTBuilder::NODE_RETURN:
		tripleSequence.push_back(Triple(returnType == ASTBuilder::SYMBOL_VOID ? TRIPLE_RETURN_PROCEDURE : TRIPLE_RETURN_FUNCTION));
		return;
	case ASTBuilder::NODE_ASSIGN: {
			ASTBuilder::TreeNode* p_right = p_node->at(1);

			SymbolId leftAddr = p_node->at(0)->symbolId;
			SymbolType leftType = p_table->find(leftAddr).symbolType;

			if (p_node->at(0)->nodeType != NODE_SYMBOL || p_table->find(leftAddr).allocationType == ALLOCATION_CONST_GLOBAL) {
				throw std::string("left part is not l-value");
			}

			SymbolId rightAddr = SYMBOL_UNDEFINED;

			if (p_right->nodeType != NODE_SYMBOL) {
				generateTripleSequence(returnType, p_right, tripleSequence);
				rightAddr = castSymbol(leftType, tripleSequence.back().result, tripleSequence);
			} else {
				rightAddr = castSymbol(leftType, p_node->at(1)->symbolId, tripleSequence);
			}

			tripleSequence.push_back(Triple(TRIPLE_COPY, leftAddr, rightAddr));

			return;
		}
	case ASTBuilder::NODE_DIV:
	case ASTBuilder::NODE_MUL:
	case ASTBuilder::NODE_SUB:
	case ASTBuilder::NODE_ADD: {
			ASTBuilder::TreeNode* p_right = p_node->at(1);
			ASTBuilder::TreeNode* p_left = p_node->at(0);

			SymbolId rightAddr;
			if (p_right->nodeType != ASTBuilder::NODE_SYMBOL) {
				generateTripleSequence(returnType, p_right, tripleSequence);
				rightAddr = tripleSequence.back().result;
			} else {
				rightAddr = p_right->symbolId;
			}

			SymbolId leftAddr;
			if (p_left->nodeType != ASTBuilder::NODE_SYMBOL) {
				generateTripleSequence(returnType, p_left, tripleSequence);
				leftAddr = tripleSequence.back().result;
			} else {
				leftAddr = p_left->symbolId;
			}

			const SymbolType leftType = p_table->find(leftAddr).symbolType;
			const SymbolType rightType = p_table->find(rightAddr).symbolType;

			SymbolId result = SYMBOL_UNDEFINED;
			if (leftType == SYMBOL_FLOAT || rightType == SYMBOL_FLOAT) {
				result = p_table->insertTemp(ASTBuilder::SYMBOL_FLOAT);

				leftAddr = castSymbol(SYMBOL_FLOAT, leftAddr, tripleSequence);
				rightAddr = castSymbol(SYMBOL_FLOAT, rightAddr, tripleSequence);

				switch (p_node->nodeType) {
				case NODE_ADD:
					tripleSequence.push_back(Triple(TRIPLE_ADD_FLOAT, result, leftAddr, rightAddr));
					break;
				case NODE_SUB:
					tripleSequence.push_back(Triple(TRIPLE_SUB_FLOAT, result, leftAddr, rightAddr));
					break;
				case NODE_DIV:
					tripleSequence.push_back(Triple(TRIPLE_DIV_FLOAT, result, leftAddr, rightAddr));
					break;
				case NODE_MUL:
					tripleSequence.push_back(Triple(TRIPLE_MUL_FLOAT, result, leftAddr, rightAddr));
					break;
				default:
					throw ASTBuilder::nodeTypeToString(p_node->nodeType);
				}
			} else {
				result = p_table->insertTemp(ASTBuilder::SYMBOL_INT);
				switch (p_node->nodeType) {
				case NODE_ADD:
					tripleSequence.push_back(Triple(TRIPLE_ADD_INT, result, leftAddr, rightAddr));
					break;
				case NODE_SUB:
					tripleSequence.push_back(Triple(TRIPLE_SUB_INT, result, leftAddr, rightAddr));
					break;
				case NODE_DIV:
					tripleSequence.push_back(Triple(TRIPLE_DIV_INT, result, leftAddr, rightAddr));
					break;
				case NODE_MUL:
					tripleSequence.push_back(Triple(TRIPLE_MUL_INT, result, leftAddr, rightAddr));
					break;
				default:
					throw ASTBuilder::nodeTypeToString(p_node->nodeType);
				}
			}
		return;
	}

	case ASTBuilder::NODE_PRINTLN: {
		ASTBuilder::TreeNode* p_arg = p_node->at(0);
		SymbolId argAddr = SYMBOL_UNDEFINED;
		if (p_arg->nodeType != ASTBuilder::NODE_SYMBOL) {
			generateTripleSequence(returnType, p_arg, tripleSequence);
			argAddr = tripleSequence.back().result;
		} else {
			argAddr = p_arg->symbolId;
		}

		const SymbolType type = p_table->find(argAddr).symbolType;
		switch (type) {
			case SYMBOL_FLOAT: {
				SymbolId tempDouble = castSymbol(SYMBOL_DOUBLE_FLOAT, argAddr, tripleSequence);
				tripleSequence.push_back(Triple(TRIPLE_PRINTLN_DOUBLE_FLOAT, tempDouble));
				break;
			}
			case SYMBOL_INT:
				tripleSequence.push_back(Triple(TRIPLE_PRINTLN_INT, argAddr));
				break;

			default:
				throw std::string("NODE_PRINTLN unknown type of the argument");
		}
		return;
	}

	default:
		throw ASTBuilder::nodeTypeToString(p_node->nodeType);
	}

	for (std::list<ASTBuilder::TreeNode*>::iterator it = p_node->childs.begin(); it != p_node->childs.end(); ++it) {
		generateTripleSequence(returnType, *it, tripleSequence);
	}

}

void CodeGenerator::generateFooter() {
	output <<
		"section '.idata' import data readable\n"
		"library kernel,'kernel32.dll',msvcrt,'msvcrt.dll'\n"
		"import kernel, ExitProcess,'ExitProcess' \n"
		"import msvcrt, printf,'printf', getchar, '_fgetchar'\n";
}

void CodeGenerator::validate() {
	for (std::map<SymbolId, Symbol>::const_iterator it = p_table->begin(); it != p_table->end(); ++it) {
		if (it->second.symbolType == SYMBOL_FUNC && it->second.value == "main") {
			return;
		}
	}

	throw std::string("main() undefined");
}

SymbolId CodeGenerator::castSymbol(SymbolType targetType, SymbolId id, TripleSequence& tripleSequence) {
	const Symbol& symbol = p_table->find(id);

	if (targetType == symbol.symbolType) {
		return id;
	}

	if (targetType == SYMBOL_FLOAT) {
		if (symbol.symbolType == SYMBOL_INT) {
			SymbolId tempFloat = p_table->insertTemp(targetType);
			tripleSequence.push_back(Triple(TRIPLE_INT_TO_FLOAT, tempFloat, id));
			return tempFloat;
		}
	}

	if (targetType == SYMBOL_DOUBLE_FLOAT) {
		if (symbol.symbolType == SYMBOL_INT) {
			SymbolId tempDouble = p_table->insertTemp(targetType);
			tripleSequence.push_back(Triple(TRIPLE_INT_TO_DOUBLE_FLOAT, tempDouble, id));
			return tempDouble;
		}

		if (symbol.symbolType == SYMBOL_FLOAT) {
			SymbolId tempDouble = p_table->insertTemp(targetType);
			tripleSequence.push_back(Triple(TRIPLE_FLOAT_TO_DOUBLE_FLOAT, tempDouble, id));
			return tempDouble;
		}
	}

	throw std::string("cant convert from ") + symbolTypeToString(targetType) + " to " + symbolTypeToString(symbol.symbolType);
}

CodeGenerator::~CodeGenerator() {
	// TODO Auto-generated destructor stub
}

} /* namespace Compiler */
