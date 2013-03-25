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

//		row.kind = kind;
		row.value = value;
		row.typeId = typeId;

		row.allocationType = allocationType;

		table.insert(std::pair<SymbolId, Symbol>(table.size(), row));

		return id;
}

void SymbolTable::debug(std::ostream& os, TypeTable* p_type) {
	os << "-= SYMBOL TABLE =-" << std::endl;
	for(SymbolMap::iterator it = table.begin(); it != table.end(); ++it) {
		os << it->first << '\t'
		   << p_type->get(it->second.typeId).getName() << '\t'
		   << it->second.value << '\t'
		   << allocationTypeToString(it->second.allocationType) << std::endl;
//		os << it->first << '\t' << symbolKindToString(it->second.kind) << '\t'
//		   << symbolTypeToString(it->second.type) << '\t' << allocationTypeToString(it->second.allocationType) << '\t' << it->second.value << std::endl;
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

//SymbolType SymbolTable::funcReturnType(SymbolId funcId) {
//	assert(find(funcId).kind == SYMBOL_FUNC);
//
//	return funcTable.at(funcId).first;
//
//}

//void SymbolTable::insertCustomTypedSymbol(SymbolId variableId,
//		SymbolId typeId) {
//
//	customType.insert(std::pair<SymbolId, SymbolId>(variableId, typeId));
//}

//SymbolId SymbolTable::insertFunc(const std::string& name, SymbolType returnType) {
//	SymbolId id = table.size();
//	Symbol row(FUNCTION);
//
//	row.allocationType = ALLOCATION_UNDEFINED;
//	row.kind = SYMBOL_FUNC;
//	row.type = returnType;
//	row.value = name;
//
//	table.insert(std::pair<SymbolId, Symbol>(table.size(), row));
//
//	return id;
//}

//void SymbolTable::insertFuncArgs(SymbolId funcId, SymbolType returnType, const std::list<SymbolId>& args) {
//	funcTable.insert(FuncTableType::value_type(funcId,
//			std::pair<SymbolType, std::list<SymbolId> > (returnType, args)));
//}

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

//const std::list<SymbolId>& SymbolTable::funcArgList(SymbolId funcId) {
//	assert(find(funcId).kind == SYMBOL_FUNC);
//
//	return funcTable.at(funcId).second;
//}

//SymbolId SymbolTable::findCustomType(SymbolId varId) {
//	return customType.at(varId);
//}

}
}

