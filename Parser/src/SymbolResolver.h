/*
 * SymbolResolver.h
 *
 *  Created on: 17.03.2013
 *      Author: user
 */

#ifndef SYMBOLRESOLVER_H_
#define SYMBOLRESOLVER_H_

#include <iostream>
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

	SymbolId findCustomType(const std::string& strType) {
		for (std::map<ASTBuilder::SymbolId, ASTBuilder::Symbol>::const_iterator it = p_table->begin(); it != p_table->end(); ++it) {
			if (it->second.kind == SYMBOL_TYPEDEF && it->second.value == strType) {
				return it->first;
			}
		}

		return -1; // FIXME
	}

	SymbolId insert(const std::string& identifier, SymbolKind kind, SymbolType type, AllocationType allocationType = ALLOCATION_UNDEFINED) {
		IdentifierTable& table = tableStack.back();

		if (table.find(identifier) != table.end()) {
			throw std::string("identifier ") + identifier + std::string(" already exists in this scope");
		}

		SymbolId id = p_table->insert(identifier, kind, type, allocationType);
		table.insert(IdentifierTable::value_type(identifier, id));

		return id;
	}

	SymbolId insertTypename(const std::string& strType, const std::string& strNewType) {
		SymbolType t = stringToSymbolType(strType);
		if (t == SYMBOL_TYPE_UNDEFINED) {
			SymbolId id = findCustomType(strType);
			if (id != -1) { // FIXME
				return insert(strNewType, SYMBOL_TYPEDEF, p_table->find(id).type);
			}

			throw std::string("unknown type:\"") + strType + "\"";
		}

		return insert(strNewType, SYMBOL_TYPEDEF, stringToSymbolType(strType));
	}

	SymbolId insertFunction(SymbolType returnType, const std::string& identifier, const std::list<SymbolId>& args) {
		IdentifierTable& table = tableStack.back();

		if (table.find(identifier) != table.end()) {
			throw std::string("identifier ") + identifier + std::string(" already exists in this scope");
		}

		SymbolId id = p_table->insertFunc(identifier, returnType);

		push(); // FIXME

		p_table->insertFuncArgs(id, returnType, args);
		table.insert(IdentifierTable::value_type(identifier, id));

		return id;
	}

	SymbolId insertVariable(const std::string& typeName, const std::string& identifier, AllocationType allocationType) {
		SymbolType type = stringToSymbolType(typeName);
		if (type == SYMBOL_TYPE_UNDEFINED) {
			SymbolId typeId = findCustomType(typeName);

			if (typeId != -1) { // FIXME
				type = p_table->find(typeId).type;
			} else {
				throw std::string("unknown type: ") + typeName;
			}

			SymbolId id = insert(identifier, SYMBOL_VALUE, SYMBOL_TYPE_CUSTOM, allocationType);
			p_table->insertCustomTypedSymbol(id, typeId);

			return id;
		}

		return insert(identifier, SYMBOL_VALUE, type, allocationType);
	}

	SymbolId insertFunction(const std::string& returnType, const std::string& identifier, const std::list<SymbolId>& args) {
		SymbolType type = stringToSymbolType(returnType);
		if (type == SYMBOL_TYPE_UNDEFINED) {
			SymbolId typeId = findCustomType(returnType);

			if (typeId != -1) { // FIXME
				type = p_table->find(typeId).type;
			} else {
				throw std::string("unknown type: ") + returnType;
			}

			SymbolId id = insertFunction(SYMBOL_TYPE_CUSTOM, identifier, args);
			p_table->insertCustomTypedSymbol(id, typeId);

			return id;
		}

		return insertFunction(type, identifier, args);
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

		debug(std::cout);
		throw std::string("SymbolResolver::resolve failed: ") + identifier;
	}

	SymbolId insertConst(const std::string& value, SymbolType type) {
		return p_table->insert(value, SYMBOL_VALUE, type, ALLOCATION_CONST_GLOBAL);
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
