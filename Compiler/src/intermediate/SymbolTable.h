/*
 * SymbolTable.h
 *
 *  Created on: 13.03.2013
 *      Author: user
 */

#ifndef SYMBOLTABLE_H_
#define SYMBOLTABLE_H_

#include <string>
#include <map>
#include "SymbolType.h"

namespace Compiler {

struct SymbolTableRow;

typedef unsigned long SymbolId;
typedef std::map<std::string, SymbolTableRow> SymbolTable;


struct SymbolTableRow {
	SymbolId 	id;
	std::string	identifier;
	SymbolType 	type;

	SymbolTableRow(const SymbolTableRow& rhs) : id(rhs.id), identifier(rhs.identifier), type(rhs.type) {

	}

	SymbolTableRow(SymbolId id, const std::string& identifier, SymbolType type)
	 : id(id), identifier(identifier), type(type) {

	}
};


} /* namespace Compiler */
#endif /* SYMBOLTABLE_H_ */
