/*
 * TripleTranslator.h
 *
 *  Created on: 16.03.2013
 *      Author: user
 */

#ifndef TRIPLETRANSLATOR_H_
#define TRIPLETRANSLATOR_H_

#include <list>
#include <ostream>

#include "Triple.h"
#include "SymbolTable.h"

namespace Compiler {

class TripleTranslator {
public:
	TripleTranslator();
	void translate(ASTBuilder::SymbolTable* p_table, std::ostream& os, std::list<Triple>& tripleSequence);

	virtual ~TripleTranslator();
};

}
#endif /* TRIPLETRANSLATOR_H_ */
