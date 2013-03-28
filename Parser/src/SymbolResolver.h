/*
 * SymbolResolver.h
 *
 *  Created on: 17.03.2013
 *      Author: user
 */

#ifndef SYMBOLRESOLVER_H_
#define SYMBOLRESOLVER_H_

#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <deque>

#include "../headers/TypeTable.h"
#include "../headers/SymbolTable.h"
#include "../headers/TreeNode.h"

namespace Compiler {
namespace ASTBuilder {

class SymbolResolver {
	SymbolTable* p_table;
	TypeTable*   p_type;

	typedef std::map<std::string, SymbolId> IdentifierTable;
	std::deque<IdentifierTable> tableStack;

	IdentifierTable& checkIdentifierExistence(const std::string& identifier) {
		IdentifierTable& table = tableStack.back();
		if (table.find(identifier) != table.end()) {
			throw std::string("identifier ") + identifier + std::string(" already exists in this scope");
		}
		return table;
	}

public:
	SymbolResolver(SymbolTable* p_table, TypeTable* p_type) : p_table(p_table), p_type(p_type) {

	}

	TypeTable* type() {
		return p_type;
	}

	SymbolId findCustomType(const std::string& strType) {
		for (std::map<ASTBuilder::SymbolId, ASTBuilder::Symbol>::const_iterator it = p_table->begin(); it != p_table->end(); ++it) {
			if (p_type->get(it->second.typeId).getKind() == TYPE_KIND_TYPEDEF && it->second.value == strType) {
				return it->first;
			}
		}

		return -1; // FIXME
	}

	SymbolId insertVariable(TypeId typeId, const std::string& identifier, AllocationType allocationType) {
		checkIdentifierExistence(identifier);
		SymbolId id = p_table->insert(identifier, typeId, allocationType);
		tableStack.back().insert(IdentifierTable::value_type(identifier, id));

		return id;
	}

	SymbolId insertFunction(TypeId returnType, const std::string& identifier) {
		checkIdentifierExistence(identifier);

		TypeId typeId = p_type->insertFunction(returnType, identifier);
		SymbolId id = p_table->insert(identifier, typeId, ALLOCATION_UNDEFINED);

		tableStack.back().insert(IdentifierTable::value_type(identifier, id));
		push(); // FIXME

		return id;
	}

	void insertFunctionArgs(SymbolId functionId, TreeNode* p_args) {
	  assert(p_args->nodeType == NODE_FUNCTION_ARGUMENTS);

	  std::list<SymbolId> args;
	  for (std::list<TreeNode*>::const_iterator it = p_args->childs.begin(); it != p_args->childs.end(); ++it) {
		args.push_back((*it)->symbolId);
	  }

		p_type->get(p_table->find(functionId).typeId).setArguments(args);
	}

	SymbolId resolve(const std::string& identifier) {
		for (std::deque<IdentifierTable>::reverse_iterator it = tableStack.rbegin();
				it != tableStack.rend(); ++it) {

			IdentifierTable& table = *it;

			IdentifierTable::iterator row = table.find(identifier);
			if (row != table.end()) {
				return row->second;
			}
		}

		debug(std::cout);
		throw std::string("SymbolResolver::resolve failed: ") + identifier;
	}

	SymbolId insertConst(const std::string& value, TypeId type) {
		return p_table->insert(value, type, ALLOCATION_CONST_GLOBAL);
	}

	void push() {
		tableStack.push_back(IdentifierTable());
	}

	void pop() {
		tableStack.pop_back();
	}

	void debug(std::ostream& os) {
		p_table->debug(os, p_type);
	}
};

}
}

#endif /* SYMBOLRESOLVER_H_ */
