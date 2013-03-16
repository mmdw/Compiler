/*
 * SymbolResolver.h
 *
 *  Created on: 13.03.2013
 *      Author: user
 */

#ifndef SYMBOLRESOLVER_H_
#define SYMBOLRESOLVER_H_

#include <list>
#include <stack>
#include <ostream>
#include "SymbolTable.h"

namespace Compiler {

class SymbolResolver {
	std::list<SymbolTable*> symbolTables;
	std::deque<SymbolTable*> tableStack;

	SymbolId symbolCount;
	SymbolId newId();

public:
	SymbolResolver();
	virtual ~SymbolResolver();

	bool existInCurrentScope(const std::string& identifier);
	bool exists(const std::string& identifier);

	SymbolId insert(const std::string& identifier, SymbolType type);
	SymbolId findInCurrentScope(const std::string& identifier);
	SymbolId find(const std::string& identifier);
	SymbolId insertTemp(SymbolType type);

	std::list<SymbolId> getScopedVariables();

	void push();
	void pop();

	void debug(std::ostream& os);
};

} /* namespace Compiler */
#endif /* SYMBOLRESOLVER_H_ */
