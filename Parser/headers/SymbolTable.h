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

#include "definitions.h"
#include "TypeTable.h"

namespace Compiler {
namespace ASTBuilder {

std::string symbolKindToString		(SymbolKind 	type);
std::string allocationTypeToString	(AllocationType type);
int			typeSize				(SymbolKind		type);

struct Symbol {
	std::string		value;
	TypeId			typeId;
	AllocationType 	allocationType;
};

class SymbolTable {
	typedef std::map<SymbolId, Symbol> 						SymbolMap;
//	typedef std::pair<SymbolType, std::list<SymbolId> > 	FuncTableRow;
//	typedef std::map<SymbolId, FuncTableRow>				FuncTableType;
//	typedef std::map<SymbolId, SymbolId>					SymbolTypedefTable;

	SymbolMap 								table;
//	FuncTableType							funcTable;
//	SymbolTypedefTable						customType;

public:
	SymbolId 					insert			(const std::string& 		value,
//									   	   	     SymbolKind 				kind,
									   	   	     TypeId		 				typeId,
									   	   	     AllocationType 			allocationType);

//	SymbolId					insertFunc		(const std::string& 		name,
//										   	   	 SymbolType 				returnType);

//	void						insertFuncArgs	(SymbolId 					funcId,
//									 	 	   	 SymbolType 				returnType,
//									 	 	   	 const std::list<SymbolId>& args);

	SymbolId					insertTemp		(TypeId 				typeId);
	SymbolId					insertLabel		(const std::string& 		name, TypeId labelType);
	SymbolId					insertNewLabel	(TypeId labelType);

//	void 						insertCustomTypedSymbol(SymbolId variableId, SymbolId typeId);

	SymbolMap::const_iterator 	begin();
	SymbolMap::const_iterator	end();

//	SymbolId					findCustomType(SymbolId varId);
	const Symbol& 				find(SymbolId symbolId);
//	SymbolType					funcReturnType(SymbolId funcId);
//	const std::list<SymbolId>&	funcArgList(SymbolId symbolId);


	void debug(std::ostream& os, TypeTable* p_type);
};

}
}


#endif /* SYMBOLTABLE_H_ */
