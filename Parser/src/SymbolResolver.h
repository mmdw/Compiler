/*
 * SymbolResolver.h
 *
 *  Created on: 17.03.2013
 *      Author: user
 */

#ifndef SYMBOLRESOLVER_H_
#define SYMBOLRESOLVER_H_

#include <map>
#include <string>
#include <deque>

#include "../headers/SymbolTable.h"

namespace Compiler {
namespace ASTBuilder {

class SymbolResolver {
	SymbolTable* p_table;

	typedef std::map<std::string, SymbolId> IdentifierTable;
	std::deque<IdentifierTable> tableStack;

public:
	SymbolResolver(SymbolTable* p_table) : p_table(p_table) {

	}

	SymbolId insert(const std::string& identifier, SymbolType type, AllocationType allocationType = ALLOCATION_UNDEFINED) {
		IdentifierTable& table = tableStack.back();

		if (table.find(identifier) != table.end()) {
			throw std::string("identifier ") + identifier + std::string(" already exists in this scope");
		}

		SymbolId id = p_table->insert(identifier, type, allocationType);
		table.insert(IdentifierTable::value_type(identifier, id));

		return id;
	}

	SymbolId insertFunction(SymbolType returnType, const std::string& identifier, const std::list<SymbolId>& args) {
		IdentifierTable& table = tableStack.back();

		if (table.find(identifier) != table.end()) {
			throw std::string("identifier ") + identifier + std::string(" already exists in this scope");
		}

		SymbolId id = p_table->insertFunc(identifier, returnType, args);
		table.insert(IdentifierTable::value_type(identifier, id));

		return id;
	}

	SymbolId insertVariable(const std::string& typeName, const std::string& identifier, AllocationType allocationType) {
		if (typeName == "int") {
			return insert(identifier, SYMBOL_INT, allocationType);
		} else if (typeName == "float") {
			return insert(identifier, SYMBOL_FLOAT, allocationType);
		} else {
			throw std::string("unknown type: ") + typeName;
		}
	}

	SymbolId insertFunction(const std::string& returnType, const std::string& identifier, const std::list<SymbolId>& args) {
		if (returnType == "void") {
			return insertFunction(SYMBOL_VOID, identifier, args);
		}

		if (returnType == "int") {
			return insertFunction(SYMBOL_INT, identifier, args);
		}

		if (returnType == "float") {
			return insertFunction(SYMBOL_FLOAT, identifier, args);
		}
	}

	SymbolId resolve(const std::string& identifier) {
		for (std::deque<IdentifierTable>::iterator it = tableStack.begin();
				it != tableStack.end(); ++it) {

			IdentifierTable& table = *it;

			IdentifierTable::iterator row = table.find(identifier);
			if (row != table.end()) {
				return row->second;
			}
		}

		throw std::string("SymbolResolver::resolve failed");
	}

	SymbolId insertConst(const std::string& value, SymbolType type) {
		return p_table->insert(value, type, ALLOCATION_CONST_GLOBAL);
	}

	void push() {
		tableStack.push_back(IdentifierTable());
	}

	void pop() {
		tableStack.pop_back();
	}

	void debug(std::ostream& os) {
		p_table->debug(os);
	}
};

}
}

#endif /* SYMBOLRESOLVER_H_ */
