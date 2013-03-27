/*
 * CodeGenerator.cpp
 *
 *  Created on: 17.03.2013
 *      Author: user
 */
#include <iostream>
#include <cassert>
#include <set>

#include "util/util.h"
#include "CodeGenerator.h"
#include "TripleTranslator.h"

namespace Compiler {

CodeGenerator::CodeGenerator(ASTBuilder::TreeNode* p_root,
		ASTBuilder::SymbolTable* p_table, ASTBuilder::TypeTable* p_type)
			: p_root(p_root), p_table(p_table), p_type(p_type), output(std::cout),
			  tripleTranslator(*this) {

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
		tripleTranslator.translate(p_table, p_type, output, ts);
	}

	TripleSequence ts;
	for (std::map<ASTBuilder::SymbolId, ASTBuilder::Symbol>::const_iterator it = p_table->begin(); it != p_table->end(); ++it) {
		if (it->second.allocationType == ASTBuilder::ALLOCATION_VARIABLE_GLOBAL
				&& p_type->get(it->second.typeId).getKind() == TYPE_KIND_PQUEUE) {

			ts.push_back(Triple(TRIPLE_PQUEUE_INIT, it->first));
		}
	}
	tripleTranslator.translate(p_table, p_type, output, ts);

	output << "\tcall main\n";
	output << "ret\n";
	output << "endp\n\n";
}

void CodeGenerator::generateConstSection() {
	output << "section '.data' readable \n";
	for (std::map<ASTBuilder::SymbolId, ASTBuilder::Symbol>::const_iterator it = p_table->begin(); it != p_table->end(); ++it) {
		if (it->second.allocationType == ASTBuilder::ALLOCATION_CONST_GLOBAL) {
			output << symbolToAddr(it->first) << '\t' << "dd\t" <<
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
			output << symbolToAddr(it->first) << '\t' << "dd\t0"  << std::endl;
		}
	}

	output << std::endl;
}

std::string CodeGenerator::symbolToAddr(ASTBuilder::SymbolId symbolId) const {
	const ASTBuilder::Symbol& symbol = p_table->find(symbolId);

	Util::to_string(symbolId);

	const std::string strSymbol = Util::to_string(symbolId);

	switch (symbol.allocationType) {
	case ASTBuilder::ALLOCATION_VARIABLE_LOCAL: 	return std::string("__local_") + strSymbol;
	case ASTBuilder::ALLOCATION_CONST_GLOBAL:		return std::string("__const_") + strSymbol;
	case ASTBuilder::ALLOCATION_VARIABLE_GLOBAL:	return std::string("__global_") + strSymbol;
	case ASTBuilder::ALLOCATION_VARIABLE_ARGUMENT:	return std::string("__arg_") + strSymbol;
	case ASTBuilder::ALLOCATION_UNDEFINED:
		if (p_type->get(symbol.typeId).getKind() == TYPE_KIND_FUNCTION) {
			return symbol.value;
		} else
		if (p_type->get(symbol.typeId).getKind() == TYPE_KIND_LABEL) {
			return std::string("__label_") + strSymbol;
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
		const Compiler::ASTBuilder::TypeRow& typeRow = p_type->get(symbol.typeId);

		TypeId returnType = typeRow.getReturnType();
		const std::list<SymbolId>& args = typeRow.getArguments();

		if (typeRow.getKind() != TYPE_KIND_FUNCTION) {
			continue;
		}

		output << "proc " << typeRow.getName() << ' ';
		if (!args.empty()) {
			for (std::list<SymbolId>::const_iterator it = args.begin(); it != args.end(); ++it) {
				if (it != args.begin()) {
					output << ", ";
				}

				output << symbolToAddr(*it);
			}
		}

		if (returnType != p_type->BASIC_VOID) {
			output << (!args.empty() ? ", " : "") << "__ret_ref";
		}

		output << std::endl;

		TripleSequence tripleSequence;
		generateTripleSequence(targetFuncId, (*it)->at(0), tripleSequence);
		if (returnType ==  p_type->BASIC_VOID) {
			if (tripleSequence.empty() || tripleSequence.back().op != TRIPLE_RETURN_PROCEDURE) {
				tripleSequence.push_back(Triple(TRIPLE_RETURN_PROCEDURE));
			}
		} else {
			if (tripleSequence.empty() || tripleSequence.back().op != TRIPLE_RETURN_FUNCTION) {
				std::cout << tripleOpToString(tripleSequence.back().op);
				throw std::string("function ") + symbol.value + " must return value";
			}
		}
		generateLocalVariables(tripleSequence);
		tripleTranslator.translate(p_table, p_type, output, tripleSequence);

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
		std::string size(p_table->find(*it).typeId == p_type->BASIC_DOUBLE_FLOAT ? ":QWORD" : ":DWORD");

		output << '\t' << "local\t" << symbolToAddr(*it) << size << std::endl;

		if (p_type->get(p_table->find(*it).typeId).getKind() == TYPE_KIND_PQUEUE) {
			tripleSequence.push_front(Triple(TRIPLE_PQUEUE_INIT, *it));
		}
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
			if (p_table->find(targetFuncId).typeId == p_type->BASIC_VOID) {
				if (!p_node->childs.empty()) {
					throw std::string("procedure cannot return value");
				}
				tripleSequence.push_back(Triple(TRIPLE_RETURN_PROCEDURE));
			} else  {
				if (p_node->childs.empty()) {
					throw std::string("function must return value");
				}

				TreeNode* p_right = p_node->at(0);

				TypeId returnTypeId = p_type->get(p_table->find(targetFuncId).typeId).getReturnType();

				SymbolId rightAddr;
				if (p_right->nodeType != NODE_SYMBOL) {
					generateTripleSequence(targetFuncId, p_right, tripleSequence);
					rightAddr = castSymbolToType(returnTypeId, tripleSequence.back().result, tripleSequence);
				} else {
					rightAddr = castSymbolToType(returnTypeId, p_right->symbolId, tripleSequence);
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

			TypeId leftTypeId = p_table->find(leftAddr).typeId;
			if (p_right->nodeType != NODE_SYMBOL) {
				generateTripleSequence(targetFuncId, p_right, tripleSequence);

				rightAddr = castSymbolToType(leftTypeId, tripleSequence.back().result, tripleSequence);
			} else {
				rightAddr = castSymbolToType(leftTypeId, p_node->at(1)->symbolId, tripleSequence);
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

			const TypeId leftType  = maybeGetReferencedType(leftAddr);
			const TypeId rightType = maybeGetReferencedType(leftAddr);

			SymbolId result = SYMBOL_UNDEFINED;
			if (leftType == p_type->BASIC_FLOAT || rightType == p_type->BASIC_FLOAT ) {
				result = p_table->insertTemp(p_type->BASIC_FLOAT);

				leftAddr = castSymbolToType(p_type->BASIC_FLOAT, leftAddr, tripleSequence);
				rightAddr = castSymbolToType(p_type->BASIC_FLOAT, rightAddr, tripleSequence);

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
				result = p_table->insertTemp(p_type->BASIC_INT);
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
		SymbolId result = p_table->insertTemp(p_type->BASIC_BOOL);

		const TypeId leftType = p_table->find(left).typeId;
		const TypeId rightType = p_table->find(right).typeId;

		if (leftType != p_type->BASIC_BOOL) {
			throw std::string("left part of ") + nodeTypeToString(p_node->nodeType) + " must be BOOL expression";
		}

		if (rightType != p_type->BASIC_BOOL) {
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
		SymbolId argAddr;
		if (p_arg->nodeType != ASTBuilder::NODE_SYMBOL) {
			generateTripleSequence(targetFuncId, p_arg, tripleSequence);
			argAddr = tripleSequence.back().result;
		} else {
			argAddr = p_arg->symbolId;
		}

		TypeId type = p_table->find(argAddr).typeId;
		if (type == p_type->BASIC_FLOAT) {
			SymbolId tempDouble = castSymbolToType(p_type->BASIC_DOUBLE_FLOAT, argAddr, tripleSequence);
			tripleSequence.push_back(
				Triple(p_node->nodeType == NODE_PRINTLN ? TRIPLE_PRINTLN_DOUBLE_FLOAT : TRIPLE_PRINT_DOUBLE_FLOAT, tempDouble)
			);
		} else
		if (type == p_type->BASIC_INT) {
			tripleSequence.push_back(
				Triple(p_node->nodeType == NODE_PRINTLN ? TRIPLE_PRINTLN_INT : TRIPLE_PRINT_INT, argAddr)
			);
		} else
		if (type == p_type->BASIC_BOOL) {
			tripleSequence.push_back(
				Triple(p_node->nodeType == NODE_PRINTLN ? TRIPLE_PRINTLN_BOOL : TRIPLE_PRINT_BOOL, argAddr)
			);
		} else {
			throw std::string("NODE_PRINTLN unknown type of the argument");
		}
		return;
	}
	case NODE_READLN: {
		ASTBuilder::TreeNode* p_arg = p_node->at(0);
		SymbolId argAddr = SYMBOL_UNDEFINED;

		if (p_arg->nodeType != ASTBuilder::NODE_SYMBOL ||
				(p_table->find(p_arg->symbolId).allocationType != ALLOCATION_VARIABLE_GLOBAL &&
				p_table->find(p_arg->symbolId).allocationType != ALLOCATION_VARIABLE_LOCAL)) {

			throw std::string("readln: argument is not l-value");
		}

		argAddr = p_arg->symbolId;
		TypeId type = p_table->find(argAddr).typeId;

		if (type == p_type->BASIC_FLOAT) {
			tripleSequence.push_back(Triple(TRIPLE_READLN_FLOAT, argAddr));
		} else
		if (type == p_type->BASIC_INT) {
			tripleSequence.push_back(Triple(TRIPLE_READLN_INT, argAddr));
		} else
		if (type == p_type->BASIC_BOOL) {
			tripleSequence.push_back(Triple(TRIPLE_READLN_BOOL, argAddr));
		}
		else {
			throw std::string("NODE_PRINTLN unknown type of the argument");
		}
		return;

		break;
	}
	case NODE_CALL: {
		SymbolId funcId = p_node->at(0)->symbolId;
		TypeId retType = p_type->get(p_table->find(funcId).typeId).getReturnType();

		const std::list<SymbolId>& funcArgs = p_type->get(p_table->find(funcId).typeId).getArguments();

		if (p_node->at(1)->childs.size() != funcArgs.size()) {
			throw std::string("wrong call func \"") + p_table->find(funcId).value + "\": different number of arguments";
		}
		SymbolId retReference = SYMBOL_UNDEFINED;
		if (retType != p_type->BASIC_VOID) {
			retReference = p_table->insertTemp(retType);
			tripleSequence.push_back(Triple(TRIPLE_PUSH_PTR, retReference));
		}

		int i = p_node->at(1)->childs.size() - 1;
		for (std::list<SymbolId>::const_reverse_iterator it = funcArgs.rbegin(); it != funcArgs.rend(); ++it, --i) {
			TypeId argType = p_table->find(*it).typeId;
			TreeNode* p_param = p_node->at(1)->at(i);

			SymbolId paramId = SYMBOL_UNDEFINED;
			if (p_param->nodeType != NODE_SYMBOL) {
				generateTripleSequence(targetFuncId, p_param, tripleSequence);
				paramId = castSymbolToType(argType, tripleSequence.back().result, tripleSequence);
			} else {
				paramId = castSymbolToType(argType, p_param->symbolId, tripleSequence);
			}

			TypeId lowlevelType = maybeGetReferencedType(*it);
			if (lowlevelType == p_type->BASIC_FLOAT) {
				tripleSequence.push_back(Triple(TRIPLE_PUSH_FLOAT, paramId));
			} else
			if (lowlevelType == p_type->BASIC_INT) {
				tripleSequence.push_back(Triple(TRIPLE_PUSH_INT, paramId));
			} else
			if (lowlevelType == p_type->BASIC_BOOL) {
				tripleSequence.push_back(Triple(TRIPLE_PUSH_BOOL, paramId));
			} else {
				throw std::string("NODE_CALL: unknown type");
			}
		}

		if (retType != p_type->BASIC_VOID) {
			tripleSequence.push_back(Triple(TRIPLE_CALL_FUNCTION, retReference, funcId));
		} else {
			tripleSequence.push_back(Triple(TRIPLE_CALL_PROCEDURE, funcId));
		}
		break;
	}

	case NODE_UMINUS: {
		TreeNode* p_arg = p_node->at(0);
		SymbolId arg = maybeEval(targetFuncId, p_arg, tripleSequence);
		const TypeId argType = p_table->find(arg).typeId;
		SymbolId res = p_table->insertTemp(argType);

		if (argType == p_type->BASIC_INT) {
			tripleSequence.push_back(Triple(TRIPLE_NEG_INT, res, arg));
		} else
		if (argType == p_type->BASIC_FLOAT) {
			tripleSequence.push_back(Triple(TRIPLE_NEG_FLOAT, res, arg));
		} else
		if (argType == p_type->BASIC_BOOL) {
			throw std::string("minus sign before BOOL value");
		} else {
			throw std::string("NODE_MINUS");
		}
		break;
	}

	case NODE_NOT: {
		TreeNode* p_arg = p_node->at(0);
		SymbolId argId = maybeEval(targetFuncId, p_arg, tripleSequence);
		SymbolId result = p_table->insertTemp(p_type->BASIC_BOOL);

		SymbolId argType = p_table->find(argId).typeId;
		if (argType != p_type->BASIC_BOOL) {
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

		const TypeId leftType = p_table->find(leftAddr).typeId;
		const TypeId rightType = p_table->find(rightAddr).typeId;

		SymbolId result = p_table->insertTemp(p_type->BASIC_BOOL);
		if (leftType == p_type->BASIC_FLOAT || rightType == p_type->BASIC_FLOAT) {
			leftAddr = castSymbolToType(p_type->BASIC_FLOAT, leftAddr, tripleSequence);
			rightAddr = castSymbolToType(p_type->BASIC_FLOAT, rightAddr, tripleSequence);

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
		SymbolId cycleStart = p_table->insertNewLabel(p_type->LABEL);
		SymbolId cycleEnd = p_table->insertNewLabel(p_type->LABEL);

		tripleSequence.push_back(Triple(TRIPLE_LABEL, cycleStart));

		SymbolId expResult = maybeEval(targetFuncId, p_node->at(0), tripleSequence);
		tripleSequence.push_back(Triple(TRIPLE_JZ, cycleEnd, expResult));

		generateTripleSequence(targetFuncId, p_node->at(1), tripleSequence);
		tripleSequence.push_back(Triple(TRIPLE_JMP, cycleStart));
		tripleSequence.push_back(Triple(TRIPLE_LABEL, cycleEnd));

		break;
	}
	case NODE_IF: {
		SymbolId ifEnd = p_table->insertNewLabel(p_type->LABEL);
		SymbolId expResult = maybeEval(targetFuncId, p_node->at(0), tripleSequence);
		tripleSequence.push_back(Triple(TRIPLE_JZ, ifEnd, expResult));
		generateTripleSequence(targetFuncId, p_node->at(1), tripleSequence);
		tripleSequence.push_back(Triple(TRIPLE_LABEL, ifEnd));
		break;
	}
	case NODE_IF_ELSE: {
		SymbolId ifElse = p_table->insertNewLabel(p_type->LABEL);
		SymbolId ifEnd  = p_table->insertNewLabel(p_type->LABEL);

		SymbolId expResult = maybeEval(targetFuncId, p_node->at(0), tripleSequence);
		tripleSequence.push_back(Triple(TRIPLE_JZ, ifElse, expResult));
		generateTripleSequence(targetFuncId, p_node->at(1), tripleSequence);
		tripleSequence.push_back(Triple(TRIPLE_JMP, ifEnd));

		tripleSequence.push_back(Triple(TRIPLE_LABEL, ifElse));
		generateTripleSequence(targetFuncId, p_node->at(2), tripleSequence);
		tripleSequence.push_back(Triple(TRIPLE_LABEL, ifEnd));

		break;
	}
	case NODE_CAST: {
		TypeId targetTypeId = p_node->getTypeId();

		SymbolId result = castSymbolToType(targetTypeId,
				maybeEval(targetFuncId, p_node->at(0), tripleSequence), tripleSequence, true);
		SymbolId tempRef = p_table->insertTemp(targetTypeId);

		tripleSequence.push_back(Triple(TRIPLE_COPY, tempRef, result));

		break;
	}
	case NODE_PQUEUE_SIZE: {
		SymbolId symbolId = p_node->at(0)->symbolId;
		SymbolId result = p_table->insertTemp(p_type->BASIC_INT);

		tripleSequence.push_back(Triple(TRIPLE_PQUEUE_SIZE, result, symbolId));
		break;
	}

	case NODE_PQUEUE_PUSH: {
		SymbolId pqueueId	= p_node->at(0)->symbolId;
		SymbolId itemId 	= maybeEval(targetFuncId, p_node->at(1), tripleSequence);
		SymbolId priorityId = maybeEval(targetFuncId, p_node->at(2), tripleSequence);

		TypeId itemType = p_type->get(p_table->find(pqueueId).typeId).getItemType();

		SymbolId castedPriority = castSymbolToType(p_type->BASIC_INT, priorityId, tripleSequence);
		SymbolId castedItem 	= castSymbolToType(itemType, itemId, tripleSequence);

		tripleSequence.push_back(Triple(TRIPLE_PUSH_INT, castedItem));
		tripleSequence.push_back(Triple(TRIPLE_PUSH_INT, castedPriority));
		tripleSequence.push_back(Triple(TRIPLE_PQUEUE_PUSH, pqueueId));
		break;
	}

	case NODE_PQUEUE_POP: {
		SymbolId pqueueId = p_node->at(0)->symbolId;
		tripleSequence.push_back(Triple(TRIPLE_PQUEUE_POP, pqueueId));
		break;
	}

	case NODE_PQUEUE_TOP: {
		SymbolId pqueueId = maybeEval(targetFuncId, p_node->at(0), tripleSequence);
		TypeId itemTypeId = p_type->get(p_table->find(pqueueId).typeId).getItemType();

		SymbolId resultId = p_table->insertTemp(itemTypeId);
		tripleSequence.push_back(Triple(TRIPLE_PQUEUE_TOP, resultId, pqueueId));
		break;
	}

	default:
		throw ASTBuilder::nodeTypeToString(p_node->nodeType);
	}
}

SymbolId CodeGenerator::castSymbolToType(TypeId targetTypeId, SymbolId symbolId,
		TripleSequence& tripleSequence, bool force) {

	const TypeRow targetType = p_type->get(targetTypeId);
	TypeId symbolTypeId = p_table->find(symbolId).typeId;
	const TypeRow symbolType = p_type->get(symbolTypeId);

	TypeId basicTargetTypeId = targetTypeId;
	TypeId basicSymbolTypeId = symbolTypeId;

	if (force) {
		if (targetType.getKind() == TYPE_KIND_TYPEDEF) {
			basicTargetTypeId = p_type->getReferencedType(targetTypeId);
		}

		if (symbolType.getKind() == TYPE_KIND_TYPEDEF) {
			basicSymbolTypeId = p_type->getReferencedType(symbolTypeId);
		}
	}

	if (basicSymbolTypeId == basicTargetTypeId) {
		return symbolId;
	}

	if (basicSymbolTypeId == p_type->BASIC_INT) {
		if (basicTargetTypeId == p_type->BASIC_FLOAT) {
			SymbolId result = p_table->insertTemp(basicTargetTypeId);
			tripleSequence.push_back(Triple(TRIPLE_INT_TO_FLOAT, result, symbolId));

			return result;
		}

		if (basicTargetTypeId == p_type->BASIC_DOUBLE_FLOAT) {
			SymbolId result = p_table->insertTemp(basicTargetTypeId);
			tripleSequence.push_back(Triple(TRIPLE_INT_TO_DOUBLE_FLOAT, result, symbolId));

			return result;
		}
	}

	if (basicSymbolTypeId == p_type->BASIC_FLOAT) {
		if (basicTargetTypeId == p_type->BASIC_DOUBLE_FLOAT) {
			SymbolId result = p_table->insertTemp(basicTargetTypeId);
			tripleSequence.push_back(Triple(TRIPLE_FLOAT_TO_DOUBLE_FLOAT, result, symbolId));

			return result;
		}
	}

	throw std::string("castSymbolToType: not implemented yet");
}

void CodeGenerator::generateFooter() {
	output <<
		"section '.idata' import data readable\n"
		"library kernel,'kernel32.dll',msvcrt,'msvcrt.dll', pqueue, 'libpqueue.dll'\n"
		"import kernel, ExitProcess,'ExitProcess' \n"
		"import msvcrt, printf,'printf', getchar, '_fgetchar', scanf, 'scanf'\n"
		"import pqueue, \\\n"
			"\tpqueue_alloc, 'pqueue_alloc', \\\n"
			"\tpqueue_size, 'pqueue_size', \\\n"
			"\tpqueue_push, 'pqueue_push', \\\n"
			"\tpqueue_pop, 'pqueue_pop', \\\n"
			"\tpqueue_top_priority, 'pqueue_top_priority', \\\n"
			"\tpqueue_top_value, 'pqueue_top_value'\n";
}

void CodeGenerator::validate() {
	for (std::map<SymbolId, Symbol>::const_iterator it = p_table->begin(); it != p_table->end(); ++it) {
		if (p_type->get(it->second.typeId).getKind() == TYPE_KIND_FUNCTION && it->second.value == "main") {
			return;
		}
	}

	throw std::string("main() undefined");
}

SymbolId CodeGenerator::maybeEval(SymbolId targetFuncId,
		TreeNode* p_node, TripleSequence& tripleSequence) {

	if (p_node->nodeType != ASTBuilder::NODE_SYMBOL) {
		generateTripleSequence(targetFuncId, p_node, tripleSequence);
		return tripleSequence.back().result;
	}

	return p_node->symbolId;
}

TypeId CodeGenerator::maybeGetReferencedType(SymbolId id) {
	const signed int typeId = p_table->find(id).typeId;

	if (p_type->get(typeId).getKind() != TYPE_KIND_TYPEDEF) {
		return p_table->find(id).typeId;
	}

	return p_type->getReferencedType(p_table->find(id).typeId);
}

CodeGenerator::~CodeGenerator() {
}

} /* namespace Compiler */
