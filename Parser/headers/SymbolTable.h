/*
 * SymbolTable.h
 *
 *  Created on: 17.03.2013
 *      Author: user
 */

#ifndef SYMBOLTABLE_H_
#define SYMBOLTABLE_H_

#include <ostream>

#include <string>
#include <map>

namespace Compiler {
namespace ASTBuilder {

typedef signed SymbolId;

enum SymbolType {
	SYMBOL_INT,
	SYMBOL_FLOAT,
	SYMBOL_DOUBLE_FLOAT,
	SYMBOL_FUNC,
	SYMBOL_VOID
};

enum AllocationType {
	ALLOCATION_VARIABLE_GLOBAL,
	ALLOCATION_VARIABLE_LOCAL,
	ALLOCATION_CONST_GLOBAL,
	ALLOCATION_UNDEFINED,
	ALLOCATION_VARIABLE_ARGUMENT //?
};

std::string symbolTypeToString		(SymbolType 	type);
std::string allocationTypeToString	(AllocationType type);

struct Symbol {
	std::string		value;
	SymbolType 		symbolType;
	AllocationType 	allocationType;
};

class SymbolTable {
	typedef std::map<SymbolId, Symbol> 		TableType;
	typedef std::map<SymbolId, SymbolType>	FuncTableType;

	TableType 								table;
	FuncTableType							funcTable;

public:
	SymbolId 					insert(const std::string& value, SymbolType type, AllocationType allocationType);
	SymbolId					insertFunc(const std::string& name, SymbolType returnType);
	SymbolId					insertTemp(SymbolType type);
	TableType::const_iterator 	begin();
	TableType::const_iterator	end();
	const Symbol& 				find(SymbolId symbolId);
	SymbolType					funcReturnType(SymbolId funcId);



	void debug(std::ostream& os);
};

}
}


#endif /* SYMBOLTABLE_H_ */
