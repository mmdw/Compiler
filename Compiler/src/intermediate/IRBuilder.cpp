/*
 * IRBuilder.cpp
 *
 *  Created on: 13.03.2013
 *      Author: user
 */

#include <algorithm>
#include <iostream>
#include <sstream>

#include "IRBuilder.h"

namespace Compiler  {

IRBuilder::IRBuilder(ASTBuilder::TreeNode* p_tree) : p_tree(p_tree) {
	fillTables();
	if (!hasMainFunc()) {
		throw std::string("main function not found");
	}
	std::cout << "-ASM---" << std::endl;
	std::cout << output.str();
}

void IRBuilder::walkGlobal(ASTBuilder::TreeNode* p_node) {
	output << "format PE console" 			<< std::endl;
	output << "entry start " 				<< std::endl;
	output << "include 'win32a.inc'      "  << std::endl;
	output << "section '.code' code readable executable\n" << std::endl;


	for (std::list<Compiler::ASTBuilder::TreeNode*>::iterator it = p_node->childs.begin();
			it != p_node->childs.end(); ++it) {

		ASTBuilder::TreeNode* p_child = *it;

		if (p_child->value == "DEFINE_VARIABLE") {
			processVariable(p_child);
		} else if (p_child->value == "DEFINE_FUNCTION") {
			processFunction(p_child);
		}
	}

	output << "start:" 	   << std::endl;
	output << "\tcall main" << std::endl;
    output << "\tccall [getchar] " << std::endl;
	output << "\tstdcall [ExitProcess],0\n" << std::endl;

	constTable.generateCode(output);
	output << "__format_int\t\tdb \"%d\",0" << std::endl << std::endl;

	output <<
		" section '.idata' import data readable \n"
		"library kernel,'kernel32.dll',msvcrt,'msvcrt.dll' \n"
		"import kernel, ExitProcess,'ExitProcess' \n"
		"import msvcrt, printf,'printf', getchar, '_fgetchar' \n";

}

SymbolId IRBuilder::processVariable(ASTBuilder::TreeNode* p_variableNode) {
	const std::string& typeName   = p_variableNode->at(0)->value;
	const std::string& identifier = p_variableNode->at(1)->value;

	return resolver.insert(identifier, stringToType(typeName));
}

SymbolType IRBuilder::stringToType(const std::string& val) {
	if (val == "int") {
		return SYMBOL_INT;
	}

	if (val == "float") {
		return SYMBOL_FLOAT;
	}

	if (val == "void") {
		return SYMBOL_VOID;
	}

	throw std::string("unknown typename: ") + val;
}

SymbolId IRBuilder::processFunction(ASTBuilder::TreeNode* p_node) {
	const std::string& typeName   = p_node->at(0)->value;
	const std::string& identifier = p_node->at(1)->value;

	FuncTableRow row;

	row.symbolId = resolver.insert(identifier, SYMBOL_FUNC);
	row.type     = stringToType(typeName);

	// args
	ASTBuilder::TreeNode* p_args = p_node->at(2);

	resolver.push();
	for (std::list<ASTBuilder::TreeNode*>::iterator it = p_args->childs.begin();
			it != p_args->childs.end(); ++it) {

		row.args.push_back(processVariable(*it));
	}

	funcTable[row.symbolId] = row;

	output << "proc " << identifier;
	for (std::list<SymbolId>::iterator it = row.args.begin(); it != row.args.end(); ++it) {
		if (it != row.args.begin()) {
			output << ",";
		}

		output << " " << symbolIdToAddr(*it);
	}
	output << std::endl;

	ASTBuilder::TreeNode* p_block = p_node->at(3);
	TripleSequence tripleSequence;
	if (!p_block->childs.empty()) {
		walk(row.type, p_block->at(0), tripleSequence);

		std::list<SymbolId> scopedVars = resolver.getScopedVariables();
		for (std::list<SymbolId>::iterator it = scopedVars.begin(); it != scopedVars.end(); ++it) {
			if (std::find(row.args.begin(), row.args.end(), *it) == row.args.end()) {
				output << "\tlocal\t" << symbolIdToAddr(*it) << ":DWORD" << std::endl;
			}
		}

		translator.translate(output, tripleSequence);
//		printTripleSequence(std::cout, trineSequence);
	}

	output << "ret" << std::endl;
	output << "endp\n" << std::endl;
	resolver.pop();

	return row.symbolId;
}

Addr IRBuilder::processExpression(ASTBuilder::TreeNode* p_expression, TripleSequence& tripleSequence) {
	if (p_expression->value == "ASSIGN") {
		Addr left  = processExpression(p_expression->at(0), tripleSequence);
		Addr right = processExpression(p_expression->at(1), tripleSequence);

		tripleSequence.push_back(Triple(TRIPLE_COPY, left, right));

		return left;
	}

	if (p_expression->value == "IDENTIFIER") {
		return symbolIdToAddr(resolver.findInCurrentScope(p_expression->at(0)->value));
	}

	if (p_expression->value == "CONST_INT") {
		ConstId constId = constTable.put(p_expression->at(0)->value, SYMBOL_INT);

		return constIdToAddr(constId);
	}

	if (p_expression->value == "CALL") {
		return processCall(p_expression, tripleSequence);
	}

	if (p_expression->value == "+") {
		Addr left = processExpression(p_expression->at(0), tripleSequence);
		Addr right = processExpression(p_expression->at(1), tripleSequence);

		SymbolId tempId = resolver.insertTemp(SYMBOL_INT);

		tripleSequence.push_back(Triple(TRIPLE_ADD, symbolIdToAddr(tempId), left, right));
		return symbolIdToAddr(tempId);
	}

	throw std::string("unknown operation: ") + p_expression->value;
}

Addr IRBuilder::constIdToAddr(ConstId constId) {
	std::ostringstream oss;
	oss << constId;

	return std::string("__const_") + oss.str();
}

Addr IRBuilder::symbolIdToAddr(SymbolId symbolId) {
	std::ostringstream oss;
	oss << symbolId;

	return std::string("__var_") + oss.str();
}

void IRBuilder::processReturn(ASTBuilder::TreeNode* p_return,
		TripleSequence& threeTupleSequence) {

	if (!p_return->childs.empty()) {
		ASTBuilder::TreeNode* p_exp = p_return->at(0);

		threeTupleSequence.push_back(Triple(TRIPLE_RETURN_FUNCTION, processExpression(p_exp, threeTupleSequence)));
	} else {
		threeTupleSequence.push_back(Triple(TRIPLE_RETURN_PROCEDURE));
	}
}

bool IRBuilder::hasMainFunc() {
	if (!resolver.exists("main")) {
		return false;
	}

	return funcTable.find(resolver.find("main")) != funcTable.end();
}

Addr IRBuilder::processCall(ASTBuilder::TreeNode* p_expression,
		TripleSequence& tripleSequence) {

	const std::string& identifier = p_expression->at(0)->value;
	ASTBuilder::TreeNode* p_args = p_expression->at(1);

	if (!resolver.existInCurrentScope(identifier)) {
		throw std::string("function ") + identifier + " does not exists in current scope";
	}

	SymbolId funcId = resolver.findInCurrentScope(identifier);

	FuncTable::iterator it = funcTable.find(funcId);
	if (it == funcTable.end()) {
		throw std::string("symbol ") + identifier + " is not a function";
	}

	SymbolType returnType = it->second.type;
	std::list<SymbolId> args = it->second.args;

	if (args.size() != p_args->childs.size()) {
		throw std::string("wrong number of arguments");
	}

	for (std::list<ASTBuilder::TreeNode*>::iterator it = p_args->childs.begin(); it != p_args->childs.end(); ++it) {
		tripleSequence.push_back(Triple(TRIPLE_PUSH, processExpression(*it, tripleSequence)));
	}

	if (returnType != SYMBOL_VOID) {
		Addr returnInto = symbolIdToAddr(resolver.insertTemp(returnType));
		tripleSequence.push_back(Triple(TRIPLE_CALL_FUNCTION, returnInto, identifier));
		return returnInto;
	}

	return "";
}

void IRBuilder::processPrintln(ASTBuilder::TreeNode* p_node,
		TripleSequence& ts) {

	Addr arg = processExpression(p_node->at(0), ts);
	ts.push_back(Triple(TRIPLE_PRINTLN, arg));
}

IRBuilder::~IRBuilder() {
}

void IRBuilder::walk(SymbolType returnType, ASTBuilder::TreeNode* p_node, TripleSequence& trineSequence) {
	if (p_node->value == "BLOCK") {
		resolver.push();
	}

	// TODO: call function
	if (p_node->value == "DEFINE_VARIABLE") {
		processVariable(p_node);
	} else if (p_node->value == "IDENTIFIER") {
		if (!resolver.existInCurrentScope(p_node->at(0)->value)) {
			throw std::string("symbol \"").append(p_node->at(0)->value).append("\" does not exist in this lookup");
		}
	} else if (p_node->value == "ASSIGN") {
		processExpression(p_node, trineSequence);
	} else if (p_node->value == "RETURN") {
		processReturn(p_node, trineSequence);
	} else if (p_node->value == "PRINTLN") {
		processPrintln(p_node, trineSequence);
	} else {
		for (std::list<Compiler::ASTBuilder::TreeNode*>::iterator it = p_node->childs.begin();
				it != p_node->childs.end(); ++it) {

			walk(returnType, *it, trineSequence);
		}
	}

	if (p_node->value == "BLOCK") {
		resolver.pop();
	}
}

void IRBuilder::fillTables() {
	resolver.push();
	walkGlobal(p_tree);
	resolver.pop();
}

} /* namespace Syntax */
