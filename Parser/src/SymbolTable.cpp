/*

 * SymbolTable.cpp
 *
 *  Created on: 17.03.2013
 *      Author: user
 */
#include <cassert>
#include <sstream>

#include "../headers/SymbolTable.h"

namespace Compiler {
namespace ASTBuilder {

std::string symbolKindToString(SymbolKind type) {
	switch (type) {
		case SYMBOL_FUNC:  			return "SYMBOL_FUNC";
		case SYMBOL_VALUE:			return "SYMBOL_VALUE";
		case SYMBOL_LABEL:			return "SYMBOL_LABEL";
		case SYMBOL_TYPEDEF:		return "SYMBOL_TYPEDEF";
		default:
			throw std::string("symbolKindToString");
	}
}

std::string allocationTypeToString(AllocationType type) {
	switch (type) {
	case ALLOCATION_CONST_GLOBAL: 		return "CONST_GLOBAL";
	case ALLOCATION_UNDEFINED:	 		return "UNDEFINED";
	case ALLOCATION_VARIABLE_ARGUMENT:	return "FUNC_ARG";
	case ALLOCATION_VARIABLE_GLOBAL:	return "VARIABLE_GLOBAL";
	case ALLOCATION_VARIABLE_LOCAL:		return "VARIABLE_LOCAL";
	default:
		throw std::string("allocationTypeToString");
	}
}

SymbolId SymbolTable::insert(const std::string& value, TypeId typeId, AllocationType allocationType) {
		Symbol row;
//		if (type == SYMBOL_TYPE_BOOL && allocationType == ALLOCATION_CONST_GLOBAL) {
//			if (value == "true") {
//				row.value = "-1";
//			} else if (value == "false") {
//				row.value = "0";
//			} else {
//				throw std::string("value \"") + value + "\" is not a boolean constant";
//			}
//		} else {
//			row.value = value;
//		}


		if (allocationType == ALLOCATION_CONST_GLOBAL) {
			for(SymbolMap::iterator it = table.begin(); it != table.end(); ++it) {
				if (row.value == it->second.value) {
					return it->first;
				}
			}
		}

		SymbolId id = table.size();

		row.value = value;
		row.typeId = typeId;

		row.allocationType = allocationType;

		table.insert(std::pair<SymbolId, Symbol>(table.size(), row));

		return id;
}

void SymbolTable::debug(std::ostream& os, TypeTable* p_type) {
	os << "-= SYMBOL TABLE =-" << std::endl;
	for(SymbolMap::iterator it = table.begin(); it != table.end(); ++it) {
		TypeId typeId = it->second.typeId;
		Compiler::ASTBuilder::TypeRow typeRow = p_type->get(typeId);

		os << it->first << '\t'
		   << (typeRow.getKind() != TYPE_KIND_PQUEUE ? typeRow.getName() : "PQUEUE") << '\t'
		   << it->second.value << '\t'
		   << allocationTypeToString(it->second.allocationType) << std::endl;
	}
}

std::map<SymbolId, Symbol>::const_iterator SymbolTable::begin() {
	return table.begin();
}

std::map<SymbolId, Symbol>::const_iterator SymbolTable::end() {
	return table.end();
}

const Symbol& SymbolTable::find(SymbolId symbolId) {
	if (table.find(symbolId) == table.end()) {
		throw std::string("symbol not found");
	}
	return table.at(symbolId);
}

SymbolId SymbolTable::insertTemp(TypeId typeId) {
	std::ostringstream oss;
	oss << table.size() + 1;

	return insert(std::string("__temp_") + oss.str(), typeId, ALLOCATION_VARIABLE_LOCAL);
}

SymbolId SymbolTable::insertLabel(const std::string& name, TypeId typeId) {
	return insert(name, typeId, ALLOCATION_UNDEFINED);
}

SymbolId SymbolTable::insertNewLabel(TypeId typeId) {
	std::ostringstream oss;
	oss << table.size() + 1;

	return insertLabel(std::string("__label_") + oss.str(), typeId);
}

}
}

