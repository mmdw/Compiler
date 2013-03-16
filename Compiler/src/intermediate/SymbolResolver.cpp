/*
 * SymbolResolver.cpp
 *
 *  Created on: 13.03.2013
 *      Author: user
 */

#include <algorithm>
#include "SymbolResolver.h"
#include <sstream>

namespace Compiler {

SymbolResolver::SymbolResolver() : symbolCount(0) {

}

static void freeTable(SymbolTable* p_table) {
	delete p_table;
}

SymbolResolver::~SymbolResolver() {
	std::for_each(symbolTables.begin(), symbolTables.end(), freeTable);
}

SymbolId SymbolResolver::insert(const std::string& identifier, SymbolType type) {
	SymbolTable* p_table = tableStack.back();

	if (p_table->find(identifier) != p_table->end()) {
		throw std::string("identifier ") + identifier + std::string(" already exists in this scope");
	}

	SymbolId id = newId();
	SymbolTableRow row(id, identifier, type);
	p_table->insert(std::pair<std::string, SymbolTableRow>(identifier, row));

	return id;
}

SymbolId SymbolResolver::insertTemp(SymbolType type) {
	SymbolTable* p_table = tableStack.back();

	SymbolId id = newId();

	std::ostringstream oss;
	oss << id;

	SymbolTableRow row(id, std::string("__temp_") + oss.str(), type);
	p_table->insert(std::pair<std::string, SymbolTableRow>(std::string("__temp_") + oss.str(), row));

	return id;
}

/**
 * TODO: don't repeat yourself
 */
bool SymbolResolver::existInCurrentScope(const std::string& identifier) {
	for (std::deque<SymbolTable*>::iterator it = tableStack.begin();
			it != tableStack.end(); ++it) {

		SymbolTable* p_table = *it;

		SymbolTable::iterator row = p_table->find(identifier);
		if (row != p_table->end()) {
			return true;
		}
	}

	return false;
}

SymbolId SymbolResolver::findInCurrentScope(const std::string& identifier) {
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

std::list<SymbolId> SymbolResolver::getScopedVariables() {
	std::list<SymbolId> result;

	std::list<SymbolTable*>::iterator tableIter = symbolTables.end();
	do {
		--tableIter;
		SymbolTable* p_table = *tableIter;

		for (SymbolTable::iterator rowIter = p_table->begin(); rowIter != p_table->end(); ++rowIter) {
			result.push_back(rowIter->second.id);
		}

	} while (*tableIter != tableStack.back());

	return result;
}

/**
 * Todo: DRY
 */
bool SymbolResolver::exists(const std::string& identifier) {
	for (std::list<SymbolTable*>::iterator it = symbolTables.begin(); it != symbolTables.end(); ++it) {
		SymbolTable* p_table = *it;

		if (p_table->find(identifier) != p_table->end()) {
			return true;
		}
	}

	return false;
}

SymbolId SymbolResolver::find(const std::string& identifier) {
	for (std::list<SymbolTable*>::iterator it = symbolTables.begin(); it != symbolTables.end(); ++it) {
		SymbolTable* p_table = *it;

		SymbolTable::iterator rowIter = p_table->find(identifier);

		if (rowIter != p_table->end()) {
			return rowIter->second.id;
		}
	}

	throw std::string("find failed");
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
