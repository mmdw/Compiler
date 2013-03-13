/*
 * SymbolResolver.cpp
 *
 *  Created on: 13.03.2013
 *      Author: user
 */

#include "SymbolResolver.h"
#include <algorithm>

namespace Compiler {

SymbolResolver::SymbolResolver() : symbolCount(0) {

}

static void freeTable(SymbolTable* p_table) {
	delete p_table;
}

SymbolResolver::~SymbolResolver() {
	std::for_each(symbolTables.begin(), symbolTables.end(), freeTable);
}

bool SymbolResolver::exists(const std::string& identifier) {

}

void SymbolResolver::insert(const std::string& identifier, SymbolType type) {
	SymbolTable* p_table = tableStack.back();

	if (p_table->find(identifier) != p_table->end()) {
		throw std::string("identifier ") + identifier + std::string(" already exists in this scope");
	}

	SymbolTableRow row(newId(), identifier, type);
	p_table->insert(std::pair<std::string, SymbolTableRow>(identifier, row));
}

SymbolId SymbolResolver::find(const std::string& identifier) {
	for (std::deque<SymbolTable*>::iterator it = tableStack.begin();
			it != tableStack.end(); ++it) {

		SymbolTable* p_table = *it;

		SymbolTable::iterator row = p_table->find(identifier);
		if (row != p_table->end()) {
			return row->second.id;
		}
	}

	throw std::string("SymbolResolver::find failed");
}

void SymbolResolver::push() {
	SymbolTable* p_table = new SymbolTable();

	symbolTables.push_back(p_table);
	tableStack.push_back(p_table);
}

SymbolId SymbolResolver::newId() {
	return ++symbolCount;
}

void SymbolResolver::pop() {
	tableStack.pop_back();
}

void SymbolResolver::debug(std::ostream& os) {
	int i = 0;

	for (std::list<SymbolTable*>::iterator it = symbolTables.begin();
			it != symbolTables.end(); ++it) {

		os << "TABLE " << i << std::endl;

		for (SymbolTable::iterator rowIter = (*it)->begin(); rowIter != (*it)->end(); ++rowIter) {
			os << rowIter->first << "\t" << rowIter->second.id << std::endl;
		}

		i++;
	}
}

} /* namespace Compiler */
