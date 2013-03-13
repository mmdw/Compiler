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

	bool exists(const std::string& identifier);
	void insert(const std::string& identifier, SymbolType type);
	SymbolId find(const std::string& identifier);

	void push();
	void pop();

	void debug(std::ostream& os);
};

} /* namespace Compiler */
#endif /* SYMBOLRESOLVER_H_ */
