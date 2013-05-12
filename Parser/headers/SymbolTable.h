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

	SymbolMap 								table;

public:
	SymbolId 					insert			(const std::string& 		value,
									   	   	     TypeId		 				typeId,
									   	   	     AllocationType 			allocationType);

	SymbolId					insertTemp		(TypeId 				typeId);
	SymbolId					insertLabel		(const std::string& 		name, TypeId labelType);
	SymbolId					insertNewLabel	(TypeId labelType);

	SymbolMap::const_iterator 	begin();
	SymbolMap::const_iterator	end();

	Symbol& 				find(SymbolId symbolId);


	void debug(std::ostream& os, TypeTable* p_type);
};

}
}


#endif /* SYMBOLTABLE_H_ */
