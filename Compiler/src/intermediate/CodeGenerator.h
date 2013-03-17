/*
 * CodeGenerator.h
 *
 *  Created on: 17.03.2013
 *      Author: user
 */

#ifndef CODEGENERATOR_H_
#define CODEGENERATOR_H_

#include <ostream>

#include "TreeNode.h"
#include "SymbolTable.h"
#include "Triple.h"

namespace Compiler {

class CodeGenerator {
	ASTBuilder::TreeNode* 		p_root;
	ASTBuilder::SymbolTable*	p_table;

	std::ostream& 				output;

	void generateHeader();
	void generateProcedures();
	void generateConstSection();

	void generateTripleSequence(ASTBuilder::SymbolType returnType, ASTBuilder::TreeNode* p_node, TripleSequence& tripleSequence);

	std::string symbolToAddr(ASTBuilder::SymbolId symbolId);
public:
	CodeGenerator( ASTBuilder::TreeNode* p_root, ASTBuilder::SymbolTable* p_table);
	virtual ~CodeGenerator();
};

} /* namespace Compiler */
#endif /* CODEGENERATOR_H_ */
