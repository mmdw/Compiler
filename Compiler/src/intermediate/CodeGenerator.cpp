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
	generateGlobalVariableSection();
	generateFooter();
}

void CodeGenerator::generateHeader() {
	output <<
		"format PE console\n"
		"entry start\n"
		"include 'win32a.inc'\n"
		"section '.code' code readable executable\n\n"
		"start: \n"
		"\tcall __init\n"
		"\tccall [getchar]\n"
		"\tstdcall [ExitProcess], 0\n\n";

	generateInitializationGlobalsCode();

	output <<
		"proc __print_bool _arg\n"
		"	 mov\tecx, dword [_arg]\n"
		"	 jecxz\t_label_false\n\n"
		"	 ccall\t[printf], __str_true\n"
		"	 ret\n\n"
		"\t_label_false:\n"
		"	 ccall [printf], __str_false\n\n"
		"	 ret\n"
		"endp\n\n";
}

void CodeGenerator::generateInitializationGlobalsCode() {
	output << "proc __init\n";
	for (std::list<ASTBuilder::TreeNode*>::iterator it = p_root->childs.begin(); it != p_root->childs.end(); ++it) {
		if ((*it)->nodeType != NODE_ASSIGN) {
			continue;
		}

		TreeNode* p_node = (*it);

		TripleSequence ts;
		generateTripleSequence(SYMBOL_UNDEFINED, p_node, ts);
		generateLocalVariables(ts);
		tripleTranslator.translate(p_table, output, ts);
	}

	output << "\tcall main\n";
	output << "ret\n";
	output << "endp\n\n";
}

void CodeGenerator::generateConstSection() {
	output << "section '.data' readable \n";
	for (std::map<ASTBuilder::SymbolId, ASTBuilder::Symbol>::const_iterator it = p_table->begin(); it != p_table->end(); ++it) {
		if (it->second.allocationType == ASTBuilder::ALLOCATION_CONST_GLOBAL) {
			output << symbolToAddr(p_table, it->first) << '\t' << "dd\t" <<
					it->second.value << std::endl;
		}
	}

	output << "__newline\tdb\t10,0" << std::endl;
	output << "__format_print_int\tdb\t\"%d\",0" << std::endl;
	output << "__format_print_float\tdb\t\"%f\",0" << std::endl;
	output << "__format_int\tdb\t\"%d\",0" << std::endl;
	output << "__format_float\tdb\t\"%f\",0" << std::endl;
	output << "__str_true\tdb\t\"true\",0" << std::endl;
	output << "__str_false\tdb\t\"false\",0" << std::endl;

	output << std::endl;
}

void CodeGenerator::generateGlobalVariableSection() {
	bool firstLine = true;

	for (std::map<ASTBuilder::SymbolId, ASTBuilder::Symbol>::const_iterator it = p_table->begin(); it != p_table->end(); ++it) {
		if (it->second.allocationType == ASTBuilder::ALLOCATION_VARIABLE_GLOBAL) {
			if (firstLine) {
				output << "section '.data' readable writable\n";
				firstLine = false;
			}
			output << symbolToAddr(p_table, it->first) << '\t' << "dd\t0"  << std::endl;
		}
	}

	output << std::endl;
}

std::string CodeGenerator::symbolToAddr(ASTBuilder::SymbolTable* p_table, ASTBuilder::SymbolId symbolId) {
	const ASTBuilder::Symbol& symbol = p_table->find(symbolId);

	Util::to_string(symbolId);

	const std::string strSymbol = Util::to_string(symbolId);

	switch (symbol.allocationType) {
	case ASTBuilder::ALLOCATION_VARIABLE_LOCAL: 	return std::string("__local_") + strSymbol;
	case ASTBuilder::ALLOCATION_CONST_GLOBAL:		return std::string("__const_") + strSymbol;
	case ASTBuilder::ALLOCATION_VARIABLE_GLOBAL:	return std::string("__global_") + strSymbol;
	case ASTBuilder::ALLOCATION_VARIABLE_ARGUMENT:	return std::string("__arg_") + strSymbol;
	case ASTBuilder::ALLOCATION_UNDEFINED:
		switch(symbol.kind) {
		case SYMBOL_FUNC:
			return symbol.value;
		case SYMBOL_LABEL:
			return std::string("__label_") + strSymbol;
		default:
			break;
		}
		break;
	default:
		throw std::string("symbolToAddr");
	}
}

void CodeGenerator::generateProcedures() {
	for (std::list<ASTBuilder::TreeNode*>::iterator it = p_root->childs.begin(); it != p_root->childs.end(); ++it) {
		if ((*it)->nodeType != NODE_FUNCTION_DEFINITION) {
			continue;
		}

		const ASTBuilder::SymbolId targetFuncId = (*it)->symbolId;
		const ASTBuilder::Symbol& symbol = p_table->find(targetFuncId);

		ASTBuilder::SymbolType returnType = p_table->funcReturnType(targetFuncId);
		const std::list<SymbolId>& args = p_table->funcArgList(targetFuncId);

		if (symbol.kind != ASTBuilder::SYMBOL_FUNC) {
			continue;
		}

		output << "proc " << symbol.value << ' ';
		if (!args.empty()) {
			for (std::list<SymbolId>::const_iterator it = args.begin(); it != args.end(); ++it) {
				if (it != args.begin()) {
					output << ", ";
				}

				output << symbolToAddr(p_table, (*it));
			}
		}

		if (returnType != SYMBOL_TYPE_VOID) {
			output << (!args.empty() ? ", " : "") << "__ret_ref";
		}

		output << std::endl;

		TripleSequence tripleSequence;
		generateTripleSequence(targetFuncId, (*it)->at(0), tripleSequence);
		if (returnType == SYMBOL_TYPE_VOID) {
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
		std::string size(p_table->find(*it).type == SYMBOL_TYPE_DOUBLE_FLOAT ? ":QWORD" : ":DWORD");

		output << '\t' << "local\t" << symbolToAddr(p_table, *it) << size << std::endl;
	}
}

void CodeGenerator::generateTripleSequence(SymbolId targetFuncId, ASTBuilder::TreeNode* p_node,
		std::list<Triple>& tripleSequence) {

	switch (p_node->nodeType) {
	case ASTBuilder::NODE_STATEMENT_BLOCK:
	case ASTBuilder::NODE_STATEMENT_SEQUENCE:
		for (std::list<ASTBuilder::TreeNode*>::iterator it = p_node->childs.begin(); it != p_node->childs.end(); ++it) {
			generateTripleSequence(targetFuncId, *it, tripleSequence);
		}
		break;
	case ASTBuilder::NODE_RETURN: {
			if (p_table->funcReturnType(targetFuncId) == SYMBOL_TYPE_VOID) {
				if (!p_node->childs.empty()) {
					throw std::string("procedure cannot return value");
				}
				tripleSequence.push_back(Triple(TRIPLE_RETURN_PROCEDURE));
			} else  {
				if (p_node->childs.empty()) {
					throw std::string("function must return value");
				}

				TreeNode* p_right = p_node->at(0);

				SymbolId rightAddr;
				if (p_right->nodeType != NODE_SYMBOL) {
					generateTripleSequence(targetFuncId, p_right, tripleSequence);
					rightAddr = castSymbolToSymbol(targetFuncId, tripleSequence.back().result, tripleSequence);
				} else {
					rightAddr = castSymbolToSymbol(targetFuncId, p_right->symbolId, tripleSequence);
				}

				tripleSequence.push_back(Triple(TRIPLE_RETURN_FUNCTION, rightAddr));
			}
		}
		return;
	case ASTBuilder::NODE_ASSIGN: {
			ASTBuilder::TreeNode* p_right = p_node->at(1);

			SymbolId leftAddr = p_node->at(0)->symbolId;

			if (p_node->at(0)->nodeType != NODE_SYMBOL || p_table->find(leftAddr).allocationType == ALLOCATION_CONST_GLOBAL) {
				throw std::string("left part is not l-value");
			}

			SymbolId rightAddr = SYMBOL_UNDEFINED;

			if (p_right->nodeType != NODE_SYMBOL) {
				generateTripleSequence(targetFuncId, p_right, tripleSequence);
				rightAddr = castSymbolToSymbol(leftAddr, tripleSequence.back().result, tripleSequence);
			} else {
				rightAddr = castSymbolToSymbol(leftAddr, p_node->at(1)->symbolId, tripleSequence);
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

			SymbolId rightAddr = maybeEval(targetFuncId, p_right, tripleSequence);
			SymbolId leftAddr = maybeEval(targetFuncId, p_left, tripleSequence);

			const SymbolType leftType = maybeUpcastFormCustomType(leftAddr);
			const SymbolType rightType = maybeUpcastFormCustomType(rightAddr);

			SymbolId result = SYMBOL_UNDEFINED;
			if (leftType == SYMBOL_TYPE_FLOAT || rightType == SYMBOL_TYPE_FLOAT) {
				result = p_table->insertTemp(ASTBuilder::SYMBOL_TYPE_FLOAT);

				leftAddr = castSymbolToSymbol(result, leftAddr, tripleSequence);
				rightAddr = castSymbolToSymbol(result, rightAddr, tripleSequence);

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
				result = p_table->insertTemp(ASTBuilder::SYMBOL_TYPE_INT);
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
	case NODE_AND:
	case NODE_OR: {
		ASTBuilder::TreeNode* p_right = p_node->at(1);
		ASTBuilder::TreeNode* p_left = p_node->at(0);

		SymbolId right = maybeEval(targetFuncId, p_right, tripleSequence);
		SymbolId left  = maybeEval(targetFuncId, p_left, tripleSequence);
		SymbolId result = p_table->insertTemp(SYMBOL_TYPE_BOOL);

		const SymbolType leftType = p_table->find(left).type;
		const SymbolType rightType = p_table->find(right).type;

		if (leftType != SYMBOL_TYPE_BOOL) {
			throw std::string("left part of ") + nodeTypeToString(p_node->nodeType) + " must be BOOL expression";
		}

		if (rightType != SYMBOL_TYPE_BOOL) {
			throw std::string("right part of ") + nodeTypeToString(p_node->nodeType) + " must be BOOL expression";
		}

		switch (p_node->nodeType) {
		case NODE_AND:
			tripleSequence.push_back(Triple(TRIPLE_AND, result, left, right));
			break;
		case NODE_OR:
			tripleSequence.push_back(Triple(TRIPLE_OR, result, left, right));
			break;
		default:
			throw std::string(nodeTypeToString(p_node->nodeType));
		}

		break;
	}
	case ASTBuilder::NODE_PRINT:
	case ASTBuilder::NODE_PRINTLN: {
		ASTBuilder::TreeNode* p_arg = p_node->at(0);
		SymbolId argAddr = SYMBOL_UNDEFINED;
		if (p_arg->nodeType != ASTBuilder::NODE_SYMBOL) {
			generateTripleSequence(targetFuncId, p_arg, tripleSequence);
			argAddr = tripleSequence.back().result;
		} else {
			argAddr = p_arg->symbolId;
		}

		SymbolType type = p_table->find(argAddr).type;
		if (type == SYMBOL_TYPE_CUSTOM) {
			type = p_table->find(p_table->findCustomType(argAddr)).type;
		}
		switch (type) {
			case SYMBOL_TYPE_FLOAT: {
				SymbolId tempDouble = castSymbolToPrimaryType(SYMBOL_TYPE_DOUBLE_FLOAT, argAddr, tripleSequence);
				tripleSequence.push_back(
					Triple(p_node->nodeType == NODE_PRINTLN ? TRIPLE_PRINTLN_DOUBLE_FLOAT : TRIPLE_PRINT_DOUBLE_FLOAT, tempDouble)
				);
				break;
			}
			case SYMBOL_TYPE_INT:
				tripleSequence.push_back(
					Triple(p_node->nodeType == NODE_PRINTLN ? TRIPLE_PRINTLN_INT : TRIPLE_PRINT_INT, argAddr)
				);
				break;
			case SYMBOL_TYPE_BOOL:
				tripleSequence.push_back(
					Triple(p_node->nodeType == NODE_PRINTLN ? TRIPLE_PRINTLN_BOOL : TRIPLE_PRINT_BOOL, argAddr)
				);
				break;
			default:
				throw std::string("NODE_PRINTLN unknown type of the argument");
		}
		return;
	}
	case NODE_READLN: {
		ASTBuilder::TreeNode* p_arg = p_node->at(0);
		SymbolId argAddr = SYMBOL_UNDEFINED;

		if (p_arg->nodeType != ASTBuilder::NODE_SYMBOL ||
				p_table->find(p_arg->symbolId).allocationType != ALLOCATION_VARIABLE_GLOBAL &&
				p_table->find(p_arg->symbolId).allocationType != ALLOCATION_VARIABLE_LOCAL) {

			throw std::string("readln: argument is not l-value");
		}

		argAddr = p_arg->symbolId;
		SymbolType type = p_table->find(argAddr).type;

		if (type == SYMBOL_TYPE_CUSTOM) {
			type = p_table->find(p_table->findCustomType(argAddr)).type;
		}

		switch (type) {
			case SYMBOL_TYPE_FLOAT:
				tripleSequence.push_back(Triple(TRIPLE_READLN_FLOAT, argAddr));
				break;
			case SYMBOL_TYPE_INT:
				tripleSequence.push_back(Triple(TRIPLE_READLN_INT, argAddr));
				break;
			case SYMBOL_TYPE_BOOL:
				tripleSequence.push_back(Triple(TRIPLE_READLN_BOOL, argAddr));
				break;
			default:
				throw std::string("NODE_PRINTLN unknown type of the argument");
		}
		return;

		break;
	}
	case NODE_CALL: {
		SymbolId funcId = p_node->at(0)->symbolId;
		SymbolType retType = p_table->funcReturnType(funcId);

		const std::list<SymbolId>& funcArgs = p_table->funcArgList(funcId);

		if (p_node->at(1)->childs.size() != funcArgs.size()) {
			throw std::string("wrong call func \"") + p_table->find(funcId).value + "\": different number of arguments";
		}
		SymbolId retReference = SYMBOL_UNDEFINED;
		if (retType != SYMBOL_TYPE_VOID) {
			retReference = p_table->insertTemp(maybeUpcastFormCustomType(funcId));
			tripleSequence.push_back(Triple(TRIPLE_PUSH_PTR, retReference));
		}

		int i = p_node->at(1)->childs.size() - 1;
		for (std::list<SymbolId>::const_reverse_iterator it = funcArgs.rbegin(); it != funcArgs.rend(); ++it, --i) {
			SymbolType targetType = p_table->find(*it).type;
			TreeNode* p_param = p_node->at(1)->at(i);

			SymbolId paramId = SYMBOL_UNDEFINED;
			if (p_param->nodeType != NODE_SYMBOL) {
				generateTripleSequence(targetFuncId, p_param, tripleSequence);
				paramId = castSymbolToPrimaryType(targetType, tripleSequence.back().result, tripleSequence);
			} else {
				paramId = castSymbolToPrimaryType(targetType, p_param->symbolId, tripleSequence);
			}

			switch (targetType) {
			case SYMBOL_TYPE_FLOAT:
				tripleSequence.push_back(Triple(TRIPLE_PUSH_FLOAT, paramId));
				break;
			case SYMBOL_TYPE_INT:
				tripleSequence.push_back(Triple(TRIPLE_PUSH_INT, paramId));
				break;
			case SYMBOL_TYPE_BOOL:
				tripleSequence.push_back(Triple(TRIPLE_PUSH_BOOL, paramId));
				break;
			default:
				throw std::string("NODE_CALL: unknown type");
			}
		}

		if (retType != SYMBOL_TYPE_VOID) {
			tripleSequence.push_back(Triple(TRIPLE_CALL_FUNCTION, retReference, funcId));
		} else {
			tripleSequence.push_back(Triple(TRIPLE_CALL_PROCEDURE, funcId));
		}
		break;
	}

	case NODE_UMINUS: {
		TreeNode* p_arg = p_node->at(0);
		SymbolId arg = maybeEval(targetFuncId, p_arg, tripleSequence);
		const SymbolType argType = p_table->find(arg).type;
		SymbolId res = p_table->insertTemp(maybeUpcastFormCustomType(arg));

		switch (argType) {
		case SYMBOL_TYPE_INT:
			tripleSequence.push_back(Triple(TRIPLE_NEG_INT, res, arg));
			break;
		case SYMBOL_TYPE_FLOAT:
			tripleSequence.push_back(Triple(TRIPLE_NEG_FLOAT, res, arg));
			break;
		case SYMBOL_TYPE_BOOL:
			throw std::string("minus sign before BOOL value");
			break;
		default:
			throw std::string("NODE_MINUS");
		}
		break;
	}
	case NODE_NOT: {
		TreeNode* p_arg = p_node->at(0);
		SymbolId argId = maybeEval(targetFuncId, p_arg, tripleSequence);
		SymbolId result = p_table->insertTemp(SYMBOL_TYPE_BOOL);

		SymbolType argType = p_table->find(argId).type;
		if (argType != SYMBOL_TYPE_BOOL) {
			throw std::string("value must be boolean");
		}

		tripleSequence.push_back(Triple(TRIPLE_NOT, result, argId));
		break;
	}

	case NODE_EQUAL:
	case NODE_NOT_EQUAL:
	case NODE_GREATER_EQUAL:
	case NODE_GREATER:
	case NODE_LESS_EQUAL:
	case NODE_LESS: {
		ASTBuilder::TreeNode* p_right = p_node->at(1);
		ASTBuilder::TreeNode* p_left = p_node->at(0);

		SymbolId rightAddr = maybeEval(targetFuncId, p_right, tripleSequence);
		SymbolId leftAddr = maybeEval(targetFuncId, p_left, tripleSequence);

		const SymbolType leftType = p_table->find(leftAddr).type;
		const SymbolType rightType = p_table->find(rightAddr).type;

		SymbolId result = p_table->insertTemp(SYMBOL_TYPE_BOOL);
		if (leftType == SYMBOL_TYPE_FLOAT || rightType == SYMBOL_TYPE_FLOAT) {
			leftAddr = castSymbolToPrimaryType(SYMBOL_TYPE_FLOAT, leftAddr, tripleSequence);
			rightAddr = castSymbolToPrimaryType(SYMBOL_TYPE_FLOAT, rightAddr, tripleSequence);

			switch (p_node->nodeType) {
			case NODE_LESS:
				tripleSequence.push_back(Triple(TRIPLE_LESS_FLOAT, result, leftAddr, rightAddr));
				break;
			case NODE_GREATER:
				tripleSequence.push_back(Triple(TRIPLE_GREATER_FLOAT, result, leftAddr, rightAddr));
				break;
			case NODE_LESS_EQUAL:
				tripleSequence.push_back(Triple(TRIPLE_LESS_EQUAL_FLOAT, result, leftAddr, rightAddr));
				break;
			case NODE_GREATER_EQUAL:
				tripleSequence.push_back(Triple(TRIPLE_GREATER_EQUAL_FLOAT, result, leftAddr, rightAddr));
				break;
			case NODE_EQUAL:
				tripleSequence.push_back(Triple(TRIPLE_EQUAL_FLOAT, result, leftAddr, rightAddr));
				break;
			case NODE_NOT_EQUAL:
				tripleSequence.push_back(Triple(TRIPLE_NOT_EQUAL_FLOAT, result, leftAddr, rightAddr));
				break;
			default:
				throw ASTBuilder::nodeTypeToString(p_node->nodeType);
			}
		} else {
			switch (p_node->nodeType) {
			case NODE_LESS:
				tripleSequence.push_back(Triple(TRIPLE_LESS_INT, result, leftAddr, rightAddr));
				break;
			case NODE_GREATER:
				tripleSequence.push_back(Triple(TRIPLE_GREATER_INT, result, leftAddr, rightAddr));
				break;
			case NODE_LESS_EQUAL:
				tripleSequence.push_back(Triple(TRIPLE_LESS_EQUAL_INT, result, leftAddr, rightAddr));
				break;
			case NODE_GREATER_EQUAL:
				tripleSequence.push_back(Triple(TRIPLE_GREATER_EQUAL_INT, result, leftAddr, rightAddr));
				break;
			case NODE_EQUAL:
				tripleSequence.push_back(Triple(TRIPLE_EQUAL_INT, result, leftAddr, rightAddr));
				break;
			case NODE_NOT_EQUAL:
				tripleSequence.push_back(Triple(TRIPLE_NOT_EQUAL_INT, result, leftAddr, rightAddr));
				break;
			default:
				throw ASTBuilder::nodeTypeToString(p_node->nodeType);
			}
		}
		break;
	}
	case NODE_WHILE_STATEMENT: {
		SymbolId cycleStart = p_table->insertNewLabel();
		SymbolId cycleEnd = p_table->insertNewLabel();

		tripleSequence.push_back(Triple(TRIPLE_LABEL, cycleStart));

		SymbolId expResult = maybeEval(targetFuncId, p_node->at(0), tripleSequence);
		tripleSequence.push_back(Triple(TRIPLE_JZ, cycleEnd, expResult));

		generateTripleSequence(targetFuncId, p_node->at(1), tripleSequence);
		tripleSequence.push_back(Triple(TRIPLE_JMP, cycleStart));
		tripleSequence.push_back(Triple(TRIPLE_LABEL, cycleEnd));

		break;
	}
	case NODE_IF: {
		SymbolId ifEnd = p_table->insertNewLabel();
		SymbolId expResult = maybeEval(targetFuncId, p_node->at(0), tripleSequence);
		tripleSequence.push_back(Triple(TRIPLE_JZ, ifEnd, expResult));
		generateTripleSequence(targetFuncId, p_node->at(1), tripleSequence);
		tripleSequence.push_back(Triple(TRIPLE_LABEL, ifEnd));
		break;
	}
	case NODE_IF_ELSE: {
		SymbolId ifElse = p_table->insertNewLabel();
		SymbolId ifEnd  = p_table->insertNewLabel();

		SymbolId expResult = maybeEval(targetFuncId, p_node->at(0), tripleSequence);
		tripleSequence.push_back(Triple(TRIPLE_JZ, ifElse, expResult));
		generateTripleSequence(targetFuncId, p_node->at(1), tripleSequence);
		tripleSequence.push_back(Triple(TRIPLE_JMP, ifEnd));

		tripleSequence.push_back(Triple(TRIPLE_LABEL, ifElse));
		generateTripleSequence(targetFuncId, p_node->at(2), tripleSequence);
		tripleSequence.push_back(Triple(TRIPLE_LABEL, ifEnd));

		break;
	}
	default:
		throw ASTBuilder::nodeTypeToString(p_node->nodeType);
	}
}

void CodeGenerator::generateFooter() {
	output <<
		"section '.idata' import data readable\n"
		"library kernel,'kernel32.dll',msvcrt,'msvcrt.dll'\n"
		"import kernel, ExitProcess,'ExitProcess' \n"
		"import msvcrt, printf,'printf', getchar, '_fgetchar', scanf, 'scanf'\n";
}

void CodeGenerator::validate() {
	for (std::map<SymbolId, Symbol>::const_iterator it = p_table->begin(); it != p_table->end(); ++it) {
		if (it->second.kind == SYMBOL_FUNC && it->second.value == "main") {
			return;
		}
	}

	throw std::string("main() undefined");
}

SymbolType CodeGenerator::maybeUpcastFormCustomType(SymbolId id) {
	const Symbol& symbol = p_table->find(id);
	SymbolType type = symbol.type;

	if (type == SYMBOL_TYPE_CUSTOM) {
		type = p_table->find(p_table->findCustomType(id)).type;
	}
	return type;
}

SymbolId CodeGenerator::castSymbolToPrimaryType(SymbolType targetType, SymbolId id, TripleSequence& tripleSequence) {
	assert(id != SYMBOL_UNDEFINED);

	const Symbol& symbol = p_table->find(id);

	if (targetType == symbol.type) {
		return id;
	}

	if (targetType == SYMBOL_TYPE_FLOAT) {
		SymbolType type = maybeUpcastFormCustomType(id);
		if (type == SYMBOL_TYPE_INT) {
			SymbolId tempFloat = p_table->insertTemp(targetType);
			tripleSequence.push_back(Triple(TRIPLE_INT_TO_FLOAT, tempFloat, id));
			return tempFloat;
		}
	}

	if (targetType == SYMBOL_TYPE_DOUBLE_FLOAT) {
		SymbolType type = maybeUpcastFormCustomType(id);
		if (type == SYMBOL_TYPE_INT) {
			SymbolId tempDouble = p_table->insertTemp(targetType);
			tripleSequence.push_back(Triple(TRIPLE_INT_TO_DOUBLE_FLOAT, tempDouble, id));
			return tempDouble;
		}

		if (type == SYMBOL_TYPE_FLOAT) {
			SymbolId tempDouble = p_table->insertTemp(targetType);
			tripleSequence.push_back(Triple(TRIPLE_FLOAT_TO_DOUBLE_FLOAT, tempDouble, id));
			return tempDouble;
		}
	}


	throw std::string("cant convert from ") + symbolTypeToString(symbol.type) + " to " + symbolTypeToString(targetType);
}

SymbolId CodeGenerator::castSymbolToSymbol(SymbolId targetId,
		SymbolId symbolId, TripleSequence& tripleSequence) {

	const Symbol& targetSymbol = p_table->find(targetId);
	const Symbol& symbol 	   = p_table->find(symbolId);

	if (symbol.type == SYMBOL_TYPE_CUSTOM && targetSymbol.type == SYMBOL_TYPE_CUSTOM) {
		if (p_table->findCustomType(targetId) != p_table->findCustomType(symbolId)) {
			throw std::string("different types");
		}

		return symbolId;
	}

	if (symbol.type == SYMBOL_TYPE_CUSTOM && targetSymbol.type != SYMBOL_TYPE_CUSTOM) {
		if (maybeUpcastFormCustomType(symbolId) == targetSymbol.type) {
			return symbolId;
		}

		throw std::string("cannot cast");
	}

	if (symbol.type != SYMBOL_TYPE_CUSTOM && targetSymbol.type == SYMBOL_TYPE_CUSTOM) {
		return castSymbolToPrimaryType(p_table->find(p_table->findCustomType(targetId)).type, symbolId, tripleSequence);
	}

	return castSymbolToPrimaryType(targetSymbol.type, symbolId, tripleSequence);
}

SymbolId CodeGenerator::maybeEval(SymbolId targetFuncId,
		TreeNode* p_node, TripleSequence& tripleSequence) {

	if (p_node->nodeType != ASTBuilder::NODE_SYMBOL) {
		generateTripleSequence(targetFuncId, p_node, tripleSequence);
		return tripleSequence.back().result;
	}

	return p_node->symbolId;
}

CodeGenerator::~CodeGenerator() {
	// TODO Auto-generated destructor stub
}

} /* namespace Compiler */
