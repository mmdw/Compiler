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
	void translate(ASTBuilder::SymbolTable* p_table, ASTBuilder::TypeTable* p_type, std::ostream& os, std::list<Triple>& tripleSequence);

	LabelId newLabel();

	virtual ~TripleTranslator();

private:
	LabelId labelCount;
	std::string cmpInstruction(TripleOp op);
};

}
#endif /* TRIPLETRANSLATOR_H_ */
