/*
 * definitions.h
 *
 *  Created on: 24.03.2013
 *      Author: user
 */

#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

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

	enum AllocationType {
		ALLOCATION_UNDEFINED,

		ALLOCATION_VARIABLE_GLOBAL,
		ALLOCATION_VARIABLE_LOCAL,
		ALLOCATION_CONST_GLOBAL,
		ALLOCATION_VARIABLE_ARGUMENT //?
	};
}
}


#endif /* DEFINITIONS_H_ */
