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
		ASTBuilder::SymbolTable* p_table, ASTBuilder::TypeTable* p_type, std::ostream& os)
			: p_root(p_root), p_table(p_table), p_type(p_type), output(os),
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

			ts.append(Triple(TRIPLE_PQUEUE_INIT, TripleArg::sym(it->first), -1));
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
	output << "__format_print_int\tdb\t\"%d \",0" << std::endl;
	output << "__format_print_float\tdb\t\"%f \",0" << std::endl;
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

std::string CodeGenerator::tripleArgToAddr(TripleArg ta) const {
	if (ta.isSymbol()) {
		return symbolToAddr(ta.getSymbolId());
	} else {
		return "_tr_" + Util::to_string(ta.getPos());
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

		if (returnType != p_type->BASIC_VOID) {
			output << "__ret_ref";

			if (!args.empty()) {
				output << ", ";
			}
		}

		if (!args.empty()) {
			for (std::list<SymbolId>::const_iterator it = args.begin(); it != args.end(); ++it) {
				if (it != args.begin()) {
					output << ", ";
				}

				output << symbolToAddr(*it);
			}
		}

		output << std::endl;

		TripleSequence tripleSequence;
		generateTripleSequence(targetFuncId, (*it)->at(0), tripleSequence);
		if (returnType ==  p_type->BASIC_VOID) {
			if (tripleSequence.isEmpty() || tripleSequence.last().op != TRIPLE_RETURN_PROCEDURE) {
				tripleSequence.append(Triple(TRIPLE_RETURN_PROCEDURE, -1));
			}
		} else {
			if (tripleSequence.isEmpty() || tripleSequence.last().op != TRIPLE_RETURN_FUNCTION) {
				std::cout << tripleOpToString(tripleSequence.last().op);
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
		if (it->arg1.isSymbol() && isVariableLocal(p_table, it->arg1.getSymbolId())) {
			locals.insert(it->arg1.getSymbolId());
		}

		if (it->arg2.isSymbol() && isVariableLocal(p_table, it->arg2.getSymbolId())) {
			locals.insert(it->arg2.getSymbolId());
		}
	}

	for (std::set<SymbolId>::iterator it = locals.begin(); it != locals.end(); ++it) {
		std::string size(p_table->find(*it).typeId == p_type->BASIC_DOUBLE_FLOAT ? ":QWORD" : ":DWORD");

		output << '\t' << "local\t" << symbolToAddr(*it) << size << std::endl;

		if (p_type->get(p_table->find(*it).typeId).getKind() == TYPE_KIND_PQUEUE) {
			tripleSequence.append_front(Triple(TRIPLE_PQUEUE_INIT, TripleArg::sym(*it), -1));
		}
	}
}

void CodeGenerator::generateTripleSequence(SymbolId targetFuncId, ASTBuilder::TreeNode* p_node,
		TripleSequence& tripleSequence) {

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
				tripleSequence.append(Triple(TRIPLE_RETURN_PROCEDURE, -1));
			} else  {
				if (p_node->childs.empty()) {
					throw std::string("function must return value");
				}

				TreeNode* p_right = p_node->at(0);

				TypeId returnTypeId = p_type->get(p_table->find(targetFuncId).typeId).getReturnType();

				TripleArg rightAddr;
				if (p_right->nodeType != NODE_SYMBOL) {
					generateTripleSequence(targetFuncId, p_right, tripleSequence);
					rightAddr = castSymbolToType(returnTypeId, TripleArg::pos(tripleSequence.lastIndex()), tripleSequence);
				} else {
					rightAddr = castSymbolToType(returnTypeId, TripleArg::sym(p_right->symbolId), tripleSequence);
				}

				tripleSequence.append(Triple(TRIPLE_RETURN_FUNCTION, rightAddr, -1));
			}
		}
		return;
	case ASTBuilder::NODE_ASSIGN: {
			ASTBuilder::TreeNode* p_right = p_node->at(1);

			TripleArg leftAddr = TripleArg::sym(p_node->at(0)->symbolId);

			if (p_node->at(0)->nodeType != NODE_SYMBOL || p_table->find(leftAddr.getSymbolId()).allocationType == ALLOCATION_CONST_GLOBAL) {
				throw std::string("left part is not l-value");
			}

			TripleArg rightAddr;

			TypeId leftTypeId = p_table->find(leftAddr.getSymbolId()).typeId;
			if (p_right->nodeType != NODE_SYMBOL) {
				generateTripleSequence(targetFuncId, p_right, tripleSequence);

				rightAddr = castSymbolToType(leftTypeId, tripleSequence.argBack(), tripleSequence);
			} else {
				rightAddr = castSymbolToType(leftTypeId, TripleArg::sym(p_node->at(1)->symbolId), tripleSequence);
			}

			tripleSequence.append(Triple(TRIPLE_COPY, leftAddr, rightAddr, getTripleType(leftAddr, tripleSequence)));

			return;
		}
	case ASTBuilder::NODE_DIV:
	case ASTBuilder::NODE_MUL:
	case ASTBuilder::NODE_SUB:
	case ASTBuilder::NODE_ADD: {
			ASTBuilder::TreeNode* p_right = p_node->at(1);
			ASTBuilder::TreeNode* p_left = p_node->at(0);

			TripleArg rightAddr = maybeEval(targetFuncId, p_right, tripleSequence);
			TripleArg leftAddr = maybeEval(targetFuncId, p_left, tripleSequence);

			const TypeId leftType  = maybeGetReferencedType(leftAddr, tripleSequence);
			const TypeId rightType = maybeGetReferencedType(rightAddr, tripleSequence);

			if (leftType == p_type->BASIC_FLOAT || rightType == p_type->BASIC_FLOAT ) {
				leftAddr = castSymbolToType(p_type->BASIC_FLOAT, leftAddr, tripleSequence);
				rightAddr = castSymbolToType(p_type->BASIC_FLOAT, rightAddr, tripleSequence);

				switch (p_node->nodeType) {
				case NODE_ADD:
					tripleSequence.append(Triple(TRIPLE_ADD_FLOAT, leftAddr, rightAddr, p_type->BASIC_FLOAT));
					break;
				case NODE_SUB:
					tripleSequence.append(Triple(TRIPLE_SUB_FLOAT, leftAddr, rightAddr, p_type->BASIC_FLOAT));
					break;
				case NODE_DIV:
					tripleSequence.append(Triple(TRIPLE_DIV_FLOAT, leftAddr, rightAddr, p_type->BASIC_FLOAT));
					break;
				case NODE_MUL:
					tripleSequence.append(Triple(TRIPLE_MUL_FLOAT, leftAddr, rightAddr, p_type->BASIC_FLOAT));
					break;
				default:
					throw ASTBuilder::nodeTypeToString(p_node->nodeType);
				}
			} else {
				switch (p_node->nodeType) {
				case NODE_ADD:
					tripleSequence.append(Triple(TRIPLE_ADD_INT, leftAddr, rightAddr, p_type->BASIC_INT));
					break;
				case NODE_SUB:
					tripleSequence.append(Triple(TRIPLE_SUB_INT, leftAddr, rightAddr, p_type->BASIC_INT));
					break;
				case NODE_DIV:
					tripleSequence.append(Triple(TRIPLE_DIV_INT,  leftAddr, rightAddr, p_type->BASIC_INT));
					break;
				case NODE_MUL:
					tripleSequence.append(Triple(TRIPLE_MUL_INT,  leftAddr, rightAddr, p_type->BASIC_INT));
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

		TripleArg right = maybeEval(targetFuncId, p_right, tripleSequence);
		TripleArg left  = maybeEval(targetFuncId, p_left, tripleSequence);

		const TypeId leftType = getTripleType(left, tripleSequence);
		const TypeId rightType = getTripleType(right, tripleSequence);

		if (leftType != p_type->BASIC_BOOL) {
			throw std::string("left part of ") + nodeTypeToString(p_node->nodeType) + " must be BOOL expression";
		}

		if (rightType != p_type->BASIC_BOOL) {
			throw std::string("right part of ") + nodeTypeToString(p_node->nodeType) + " must be BOOL expression";
		}

		switch (p_node->nodeType) {
		case NODE_AND:
			tripleSequence.append(Triple(TRIPLE_AND, left, right, p_type->BASIC_BOOL));
			break;
		case NODE_OR:
			tripleSequence.append(Triple(TRIPLE_OR,  left, right, p_type->BASIC_BOOL));
			break;
		default:
			throw std::string(nodeTypeToString(p_node->nodeType));
		}

		break;
	}
	case ASTBuilder::NODE_PRINT:
	case ASTBuilder::NODE_PRINTLN: {
		ASTBuilder::TreeNode* p_arg = p_node->at(0);
		TripleArg argAddr = maybeEval(targetFuncId, p_arg, tripleSequence);

		TypeId type = getTripleType(argAddr, tripleSequence);
		if (type == p_type->BASIC_FLOAT) {
			TripleArg tempDouble = castSymbolToType(p_type->BASIC_DOUBLE_FLOAT, argAddr, tripleSequence);
			tripleSequence.append(
				Triple(p_node->nodeType == NODE_PRINTLN ? TRIPLE_PRINTLN_DOUBLE_FLOAT : TRIPLE_PRINT_DOUBLE_FLOAT, tempDouble, -1)
			);
		} else
		if (type == p_type->BASIC_INT) {
			tripleSequence.append(
				Triple(p_node->nodeType == NODE_PRINTLN ? TRIPLE_PRINTLN_INT : TRIPLE_PRINT_INT, argAddr, -1)
			);
		} else
		if (type == p_type->BASIC_BOOL) {
			tripleSequence.append(
				Triple(p_node->nodeType == NODE_PRINTLN ? TRIPLE_PRINTLN_BOOL : TRIPLE_PRINT_BOOL, argAddr, -1)
			);
		} else {
			throw std::string("NODE_PRINTLN unknown type of the argument");
		}
		return;
	}
	case NODE_READLN: {
		ASTBuilder::TreeNode* p_arg = p_node->at(0);
		TripleArg argAddr;

		if (p_arg->nodeType != ASTBuilder::NODE_SYMBOL ||
				(p_table->find(p_arg->symbolId).allocationType != ALLOCATION_VARIABLE_GLOBAL &&
				p_table->find(p_arg->symbolId).allocationType != ALLOCATION_VARIABLE_LOCAL)) {

			throw std::string("readln: argument is not l-value");
		}

		argAddr = TripleArg::sym(p_arg->symbolId);
		TypeId type = p_table->find(p_arg->symbolId).typeId;

		if (type == p_type->BASIC_FLOAT) {
			tripleSequence.append(Triple(TRIPLE_READLN_FLOAT, argAddr, -1));
		} else
		if (type == p_type->BASIC_INT) {
			tripleSequence.append(Triple(TRIPLE_READLN_INT, argAddr, -1));
		} else
		if (type == p_type->BASIC_BOOL) {
			tripleSequence.append(Triple(TRIPLE_READLN_BOOL, argAddr, -1));
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
//		TripleArg retReference;
//		if (retType != p_type->BASIC_VOID) {
//			retReference = TripleArg::sym(p_table->insertTemp(retType));
//			tripleSequence.append(Triple(TRIPLE_PUSH_PTR, retReference, -1));
//		}

		int i = p_node->at(1)->childs.size() - 1;
		for (std::list<SymbolId>::const_reverse_iterator it = funcArgs.rbegin(); it != funcArgs.rend(); ++it, --i) {
			TypeId argType = p_table->find(*it).typeId;
			TreeNode* p_param = p_node->at(1)->at(i);

			TripleArg paramId;
			if (p_param->nodeType != NODE_SYMBOL) {
				generateTripleSequence(targetFuncId, p_param, tripleSequence);
				paramId = castSymbolToType(argType, tripleSequence.argBack(), tripleSequence);
			} else {
				paramId = castSymbolToType(argType, TripleArg::sym(p_param->symbolId), tripleSequence);
			}

			TypeId lowlevelType = maybeGetReferencedType(TripleArg::sym(*it), tripleSequence);
			if (lowlevelType == p_type->BASIC_FLOAT) {
				tripleSequence.append(Triple(TRIPLE_PUSH_FLOAT, paramId, -1));
			} else
			if (lowlevelType == p_type->BASIC_INT) {
				tripleSequence.append(Triple(TRIPLE_PUSH_INT, paramId, -1));
			} else
			if (lowlevelType == p_type->BASIC_BOOL) {
				tripleSequence.append(Triple(TRIPLE_PUSH_BOOL, paramId, -1));
			} else {
				throw std::string("NODE_CALL: unknown type");
			}
		}

		if (retType != p_type->BASIC_VOID) {
			tripleSequence.append(Triple(TRIPLE_CALL_FUNCTION, TripleArg::sym(funcId), retType));
		} else {
			tripleSequence.append(Triple(TRIPLE_CALL_PROCEDURE, TripleArg::sym(funcId), retType));
		}
		break;
	}

	case NODE_UMINUS: {
		TreeNode* p_arg = p_node->at(0);
		TripleArg arg = maybeEval(targetFuncId, p_arg, tripleSequence);
		const TypeId argType = getTripleType(arg, tripleSequence);

		if (argType == p_type->BASIC_INT) {
			tripleSequence.append(Triple(TRIPLE_NEG_INT, arg, p_type->BASIC_INT));
		} else
		if (argType == p_type->BASIC_FLOAT) {
			tripleSequence.append(Triple(TRIPLE_NEG_FLOAT, arg, p_type->BASIC_FLOAT));
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
		TripleArg argId = maybeEval(targetFuncId, p_arg, tripleSequence);

		TypeId argType = getTripleType(argId, tripleSequence);
		if (argType != p_type->BASIC_BOOL) {
			throw std::string("value must be boolean");
		}

		tripleSequence.append(Triple(TRIPLE_NOT, argId,p_type->BASIC_BOOL));
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

		TripleArg rightAddr = maybeEval(targetFuncId, p_right, tripleSequence);
		TripleArg leftAddr = maybeEval(targetFuncId, p_left, tripleSequence);

		const TypeId leftType = getTripleType(leftAddr, tripleSequence);
		const TypeId rightType = getTripleType(rightAddr, tripleSequence);

		if (leftType == p_type->BASIC_FLOAT || rightType == p_type->BASIC_FLOAT) {
			leftAddr = castSymbolToType(p_type->BASIC_FLOAT, leftAddr, tripleSequence);
			rightAddr = castSymbolToType(p_type->BASIC_FLOAT, rightAddr, tripleSequence);

			switch (p_node->nodeType) {
			case NODE_LESS:
				tripleSequence.append(Triple(TRIPLE_LESS_FLOAT, leftAddr, rightAddr, p_type->BASIC_BOOL));
				break;
			case NODE_GREATER:
				tripleSequence.append(Triple(TRIPLE_GREATER_FLOAT, leftAddr, rightAddr, p_type->BASIC_BOOL));
				break;
			case NODE_LESS_EQUAL:
				tripleSequence.append(Triple(TRIPLE_LESS_EQUAL_FLOAT, leftAddr, rightAddr, p_type->BASIC_BOOL));
				break;
			case NODE_GREATER_EQUAL:
				tripleSequence.append(Triple(TRIPLE_GREATER_EQUAL_FLOAT, leftAddr, rightAddr, p_type->BASIC_BOOL));
				break;
			case NODE_EQUAL:
				tripleSequence.append(Triple(TRIPLE_EQUAL_FLOAT, leftAddr, rightAddr, p_type->BASIC_BOOL));
				break;
			case NODE_NOT_EQUAL:
				tripleSequence.append(Triple(TRIPLE_NOT_EQUAL_FLOAT, leftAddr, rightAddr, p_type->BASIC_BOOL));
				break;
			default:
				throw ASTBuilder::nodeTypeToString(p_node->nodeType);
			}
		} else {
			switch (p_node->nodeType) {
			case NODE_LESS:
				tripleSequence.append(Triple(TRIPLE_LESS_INT, leftAddr, rightAddr, p_type->BASIC_BOOL));
				break;
			case NODE_GREATER:
				tripleSequence.append(Triple(TRIPLE_GREATER_INT, leftAddr, rightAddr, p_type->BASIC_BOOL));
				break;
			case NODE_LESS_EQUAL:
				tripleSequence.append(Triple(TRIPLE_LESS_EQUAL_INT, leftAddr, rightAddr, p_type->BASIC_BOOL));
				break;
			case NODE_GREATER_EQUAL:
				tripleSequence.append(Triple(TRIPLE_GREATER_EQUAL_INT, leftAddr, rightAddr, p_type->BASIC_BOOL));
				break;
			case NODE_EQUAL:
				tripleSequence.append(Triple(TRIPLE_EQUAL_INT, leftAddr, rightAddr, p_type->BASIC_BOOL));
				break;
			case NODE_NOT_EQUAL:
				tripleSequence.append(Triple(TRIPLE_NOT_EQUAL_INT, leftAddr, rightAddr, p_type->BASIC_BOOL));
				break;
			default:
				throw ASTBuilder::nodeTypeToString(p_node->nodeType);
			}
		}
		break;
	}
	case NODE_WHILE_STATEMENT: {
		TripleArg cycleStart = TripleArg::sym(p_table->insertNewLabel(p_type->LABEL));
		TripleArg cycleEnd = TripleArg::sym(p_table->insertNewLabel(p_type->LABEL));

		tripleSequence.append(Triple(TRIPLE_LABEL, cycleStart, -1));

		TripleArg expResult = maybeEval(targetFuncId, p_node->at(0), tripleSequence);
		tripleSequence.append(Triple(TRIPLE_JZ, cycleEnd, expResult, -1));

		generateTripleSequence(targetFuncId, p_node->at(1), tripleSequence);
		tripleSequence.append(Triple(TRIPLE_JMP, cycleStart, -1));
		tripleSequence.append(Triple(TRIPLE_LABEL, cycleEnd, -1));

		break;
	}
	case NODE_IF: {
		TripleArg ifEnd = TripleArg::sym(p_table->insertNewLabel(p_type->LABEL));
		TripleArg expResult = maybeEval(targetFuncId, p_node->at(0), tripleSequence);
		tripleSequence.append(Triple(TRIPLE_JZ, ifEnd, expResult, -1));
		generateTripleSequence(targetFuncId, p_node->at(1), tripleSequence);
		tripleSequence.append(Triple(TRIPLE_LABEL, ifEnd, -1));
		break;
	}
	case NODE_IF_ELSE: {
		TripleArg ifElse = TripleArg::sym(p_table->insertNewLabel(p_type->LABEL));
		TripleArg ifEnd  = TripleArg::sym(p_table->insertNewLabel(p_type->LABEL));

		TripleArg expResult = maybeEval(targetFuncId, p_node->at(0), tripleSequence);
		tripleSequence.append(Triple(TRIPLE_JZ, ifElse, expResult, -1));
		generateTripleSequence(targetFuncId, p_node->at(1), tripleSequence);
		tripleSequence.append(Triple(TRIPLE_JMP, ifEnd, -1));

		tripleSequence.append(Triple(TRIPLE_LABEL, ifElse, -1));
		generateTripleSequence(targetFuncId, p_node->at(2), tripleSequence);
		tripleSequence.append(Triple(TRIPLE_LABEL, ifEnd, -1));

		break;
	}
	case NODE_CAST: {
		TypeId targetTypeId = p_node->getTypeId();

		TripleArg result = castSymbolToType(targetTypeId,
				maybeEval(targetFuncId, p_node->at(0), tripleSequence), tripleSequence, true);
		TripleArg tempRef =TripleArg::sym(p_table->insertTemp(targetTypeId));

		tripleSequence.append(Triple(TRIPLE_COPY, tempRef, result, getTripleType(result, tripleSequence)));

		break;
	}
	case NODE_PQUEUE_SIZE: {
		TripleArg symbolId = maybeEval(targetFuncId, p_node->at(0), tripleSequence);

		tripleSequence.append(Triple(TRIPLE_PQUEUE_SIZE, symbolId, p_type->BASIC_INT));
		break;
	}

	case NODE_PQUEUE_PUSH: {
		SymbolId pqueueId	= p_node->at(0)->symbolId;
		TripleArg itemId 	= maybeEval(targetFuncId, p_node->at(1), tripleSequence);
		TripleArg priorityId = maybeEval(targetFuncId, p_node->at(2), tripleSequence);

		TypeId itemType = p_type->get(p_table->find(pqueueId).typeId).getItemType();

		TripleArg castedPriority = castSymbolToType(p_type->BASIC_INT, priorityId, tripleSequence);
		TripleArg castedItem 	= castSymbolToType(itemType, itemId, tripleSequence);

		tripleSequence.append(Triple(TRIPLE_PUSH_INT, castedItem, -1));
		tripleSequence.append(Triple(TRIPLE_PUSH_INT, castedPriority, -1));
		tripleSequence.append(Triple(TRIPLE_PQUEUE_PUSH, TripleArg::sym(pqueueId), -1));
		break;
	}

	case NODE_PQUEUE_POP: {
		TripleArg pqueueId = maybeEval(targetFuncId, p_node->at(0), tripleSequence);
		tripleSequence.append(Triple(TRIPLE_PQUEUE_POP, pqueueId, -1));
		break;
	}

	case NODE_PQUEUE_TOP: {
		TripleArg pqueueId = maybeEval(targetFuncId, p_node->at(0), tripleSequence);
		TypeId itemTypeId = p_type->get(getTripleType(pqueueId, tripleSequence)).getItemType();

		tripleSequence.append(Triple(TRIPLE_PQUEUE_TOP, pqueueId, itemTypeId));
		break;
	}

	case NODE_PQUEUE_TOP_PRIORITY: {
		TripleArg pqueueId = maybeEval(targetFuncId, p_node->at(0), tripleSequence);
		tripleSequence.append(Triple(TRIPLE_PQUEUE_TOP_PRIORITY, pqueueId, p_type->BASIC_INT));
		break;
	}

	default:
		throw ASTBuilder::nodeTypeToString(p_node->nodeType);
	}
}

TypeId CodeGenerator::getTripleType(const TripleArg& ta, TripleSequence& tripleSequence) {
	if (ta.isSymbol()) {
		return p_table->find(ta.getSymbolId()).typeId;
	}

	return tripleSequence.at(ta.getPos()).returnType;
}

TripleArg CodeGenerator::castSymbolToType(TypeId targetTypeId, TripleArg tripleArg,
		TripleSequence& tripleSequence, bool force) {

	const TypeRow targetType = p_type->get(targetTypeId);
	TypeId tripleTypeId = getTripleType(tripleArg, tripleSequence);
	const TypeRow symbolType = p_type->get(tripleTypeId);

	TypeId basicTargetTypeId = targetTypeId;
	TypeId basicSymbolTypeId = tripleTypeId;

	if (force) {
		if (targetType.getKind() == TYPE_KIND_TYPEDEF) {
			basicTargetTypeId = p_type->getReferencedType(targetTypeId);
		}

		if (symbolType.getKind() == TYPE_KIND_TYPEDEF) {
			basicSymbolTypeId = p_type->getReferencedType(tripleTypeId);
		}
	}

	if (basicSymbolTypeId == basicTargetTypeId) {
		return tripleArg;
	}

	if (basicSymbolTypeId == p_type->BASIC_INT) {
		if (basicTargetTypeId == p_type->BASIC_FLOAT) {
			tripleSequence.append(Triple(TRIPLE_INT_TO_FLOAT, tripleArg, p_type->BASIC_FLOAT));
			TripleArg result = TripleArg::pos(tripleSequence.lastIndex());

			return result;
		}

		if (basicTargetTypeId == p_type->BASIC_DOUBLE_FLOAT) {
			tripleSequence.append(Triple(TRIPLE_INT_TO_DOUBLE_FLOAT, tripleArg, p_type->BASIC_DOUBLE_FLOAT));
			TripleArg result = TripleArg::pos(tripleSequence.lastIndex());

			return result;
		}
	}

	if (basicSymbolTypeId == p_type->BASIC_FLOAT) {
		if (basicTargetTypeId == p_type->BASIC_DOUBLE_FLOAT) {
			tripleSequence.append(Triple(TRIPLE_FLOAT_TO_DOUBLE_FLOAT, tripleArg, p_type->BASIC_DOUBLE_FLOAT));
			TripleArg result = TripleArg::pos(tripleSequence.lastIndex());

			return result;
		}

		if (basicTargetTypeId == p_type->BASIC_INT) {
			tripleSequence.append(Triple(TRIPLE_FLOAT_TO_INT, tripleArg, p_type->BASIC_INT));
			TripleArg result = TripleArg::pos(tripleSequence.lastIndex());

			return result;
		}
	}

	throw std::string("castSymbolToType:cannot cast from \"") + p_type->get(tripleTypeId).getName()
			+ "\" to \"" + p_type->get(targetTypeId).getName() + '"';
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

TripleArg CodeGenerator::maybeEval(SymbolId targetFuncId,
		TreeNode* p_node, TripleSequence& tripleSequence) {

	if (p_node->nodeType != ASTBuilder::NODE_SYMBOL) {
		generateTripleSequence(targetFuncId, p_node, tripleSequence);
		return TripleArg::pos(tripleSequence.lastIndex());
	}

	return TripleArg::sym(p_node->symbolId);
}

TypeId CodeGenerator::maybeGetReferencedType(TripleArg ta, TripleSequence& ts) {
	int typeId;
	if (ta.isSymbol()) {
		typeId = p_table->find(ta.getSymbolId()).typeId;
	} else {
		assert(ts.lastIndex() >= ta.getPos());
		Triple triple =  ts.at(ta.getPos());
		typeId = triple.returnType;
	}

	if (p_type->get(typeId).getKind() != TYPE_KIND_TYPEDEF) {
		return typeId;
	}

	return p_type->getReferencedType(typeId);
}

CodeGenerator::~CodeGenerator() {
}

} /* namespace Compiler */
