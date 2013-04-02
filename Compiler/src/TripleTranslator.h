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
class CodeGenerator;

class TripleTranslator {
public:
	TripleTranslator(const CodeGenerator& cg);
	void translate(ASTBuilder::SymbolTable* p_table, ASTBuilder::TypeTable* p_type, std::ostream& os, TripleSequence& tripleSequence);

	LabelId newLabel();

	virtual ~TripleTranslator();

private:
	const CodeGenerator& cg;
	LabelId labelCount;
	std::string cmpInstruction(TripleOp op);
};

}
#endif /* TRIPLETRANSLATOR_H_ */
