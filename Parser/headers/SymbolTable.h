/*
 * SymbolTable.h
 *
 *  Created on: 17.03.2013
 *      Author: user
 */

#ifndef SYMBOLTABLE_H_
#define SYMBOLTABLE_H_

#include <list>
#include <ostream>
#include <string>
#include <map>

namespace Compiler {
namespace ASTBuilder {


typedef signed SymbolId;
typedef signed TypeId;

enum SymbolKind {
	SYMBOL_FUNC,
	SYMBOL_VALUE,
	SYMBOL_LABEL,
	SYMBOL_TYPEDEF,
};

enum SymbolType {
	SYMBOL_TYPE_UNDEFINED,

	SYMBOL_TYPE_BOOL,
	SYMBOL_TYPE_VOID,
	SYMBOL_TYPE_INT,
	SYMBOL_TYPE_FLOAT,
	SYMBOL_TYPE_DOUBLE_FLOAT,

	SYMBOL_TYPE_CUSTOM,
};

enum AllocationType {
	ALLOCATION_UNDEFINED,

	ALLOCATION_VARIABLE_GLOBAL,
	ALLOCATION_VARIABLE_LOCAL,
	ALLOCATION_CONST_GLOBAL,
	ALLOCATION_VARIABLE_ARGUMENT //?
};

SymbolType	stringToSymbolType		(const std::string& val);
std::string symbolTypeToString		(SymbolType 	type);
std::string symbolKindToString		(SymbolKind 	type);
std::string allocationTypeToString	(AllocationType type);
int			typeSize				(SymbolKind		type);

struct Symbol {
	std::string		value;
	SymbolKind 		kind;
	SymbolType		type;
	AllocationType 	allocationType;
};

class SymbolTable {
	typedef std::map<SymbolId, Symbol> 						SymbolMap;
	typedef std::pair<SymbolType, std::list<SymbolId> > 	FuncTableRow;
	typedef std::map<SymbolId, FuncTableRow>				FuncTableType;
	typedef std::map<SymbolId, SymbolId>					SymbolTypedefTable;

	SymbolMap 								table;
	FuncTableType							funcTable;
	SymbolTypedefTable						customType;

public:
	SymbolId 					insert			(const std::string& 		value,
									   	   	     SymbolKind 				kind,
									   	   	     SymbolType 				type,
									   	   	     AllocationType 			allocationType);

	SymbolId					insertFunc		(const std::string& 		name,
										   	   	 SymbolType 				returnType);

	void						insertFuncArgs	(SymbolId 					funcId,
									 	 	   	 SymbolType 				returnType,
									 	 	   	 const std::list<SymbolId>& args);

	SymbolId					insertTemp		(SymbolType 				type);
	SymbolId					insertLabel		(const std::string& 		name);
	SymbolId					insertNewLabel	();

	void 						insertCustomTypedSymbol(SymbolId variableId, SymbolId typeId);

	SymbolMap::const_iterator 	begin();
	SymbolMap::const_iterator	end();

	SymbolId					findCustomType(SymbolId varId);
	const Symbol& 				find(SymbolId symbolId);
	SymbolType					funcReturnType(SymbolId funcId);
	const std::list<SymbolId>&	funcArgList(SymbolId symbolId);


	void debug(std::ostream& os);
};

}
}


#endif /* SYMBOLTABLE_H_ */
