/*

 * SymbolTable.cpp
 *
 *  Created on: 17.03.2013
 *      Author: user
 */
#include <cassert>
#include "../headers/SymbolTable.h"

namespace Compiler {
namespace ASTBuilder {

std::string symbolTypeToString(SymbolType type) {
	switch (type) {
		case SYMBOL_FLOAT: return "SYMBOL_FLOAT";
		case SYMBOL_FUNC:  return "SYMBOL_FUNC";
		case SYMBOL_INT:   return "SYMBOL_INT";
		case SYMBOL_VOID:  return "SYMBOL_VOID";
		default:
			throw std::string("symbolTypeToString");
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

SymbolId SymbolTable::insert(const std::string& value,
		SymbolType type, AllocationType allocationType) {

		assert(type != SYMBOL_FUNC);

		if (allocationType == ALLOCATION_CONST_GLOBAL) {
			for(TableType::iterator it = table.begin(); it != table.end(); ++it) {
				if (value == it->second.value) {
					return it->first;
				}
			}
		}

		SymbolId id = table.size();
		Symbol row;

		row.value = value;
		row.symbolType = type;
		row.allocationType = allocationType;

		table.insert(std::pair<SymbolId, Symbol>(table.size(), row));

		return id;
}

void SymbolTable::debug(std::ostream& os) {
	os << "-= SYMBOL TABLE =-" << std::endl;
	for(TableType::iterator it = table.begin(); it != table.end(); ++it) {
		os << it->first << '\t' << symbolTypeToString(it->second.symbolType) << '\t'
		   << allocationTypeToString(it->second.allocationType) << '\t' << it->second.value << std::endl;
	}
}

std::map<SymbolId, Symbol>::const_iterator SymbolTable::begin() {
	return table.begin();
}

std::map<SymbolId, Symbol>::const_iterator SymbolTable::end() {
	return table.end();
}

const Symbol& SymbolTable::find(SymbolId symbolId) {
	return table.at(symbolId);
}

SymbolType SymbolTable::funcReturnType(SymbolId funcId) {
	assert(find(funcId).symbolType == SYMBOL_FUNC);

	return funcTable.at(funcId);

}

SymbolId SymbolTable::insertFunc(const std::string& name, SymbolType returnType) {
	SymbolId id = table.size();
	Symbol row;

	row.allocationType = ALLOCATION_UNDEFINED;
	row.symbolType = SYMBOL_FUNC;
	row.value = name;

	table.insert(std::pair<SymbolId, Symbol>(table.size(), row));
	funcTable.insert(FuncTableType::value_type(id, returnType));

	return id;
}

}
}


